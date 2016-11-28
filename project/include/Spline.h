#pragma once

#include <cstddef>  // for NULL
#include <math.h>  // for ceilf
#include <vector>
#include <assert.h>

struct TimePoint
{
	float distance;  // total arc length from t=0 to this time point
	unsigned int segment_idx;  // segment
	float u;  // segment time
};

template <typename T>
class CatmullRom
{
private:
	static constexpr float smX[5] = { 0.0000000000f, 0.5384693101f, -0.5384693101f, 0.9061798459f, -0.9061798459f };
	static constexpr float smC[5] = { 0.5688888889f, 0.4786286705f, 0.4786286705f, 0.2369268850f, 0.2369268850f };

public:
	CatmullRom() {}
	CatmullRom(const std::vector<T>& control_points, bool duplicate_ends)
	{
		set_control_points(control_points, duplicate_ends);
	}

	void clear()
	{
		mPoints.clear();
		mTimePoints.clear();
	}

	void set_control_points(const std::vector<T>& controlPoints, bool duplicate_ends)
	{
		assert(controlPoints.size() >= 2);

		mPoints = controlPoints;
		if (duplicate_ends || mPoints.size() < 4)
		{
			// duplicate start and end
			mPoints.insert(mPoints.begin(), mPoints.at(0));
			mPoints.push_back(mPoints.at(mPoints.size() - 1));
		}

		mTimePoints = BuildTimeMap(mPoints);
		mTotalLength = mTimePoints.back().distance;
	}

	void evaluate(float d, T* c0_out, T* c1_out) const
	{
		TimePoint tp = timepoint_at_distance(d);
		evaluate_segment(tp.segment_idx, tp.u, c0_out, c1_out);
	}

	void evaluate_segment(int segment, float u, T* c0_out, T* c1_out) const
	{
		assert(n_control_points() >= 4);
		assert(u >= 0 && u <= 1 && segment >= 1 && segment < (int)mPoints.size() - 2);

		int idx0, idx1, idx2, idx3;
		idx0 = segment - 1;
		idx1 = segment;
		idx2 = segment + 1;
		idx3 = segment + 2;

		T A = 3.0f*mPoints[idx1]
			- mPoints[idx0]
			- 3.0f*mPoints[idx2]
			+ mPoints[idx3];

		T B = 2.0f*mPoints[idx0]
			- 5.0f*mPoints[idx1]
			+ 4.0f*mPoints[idx2]
			- mPoints[idx3];

		T C = mPoints[idx2] - mPoints[idx0];

		if (c0_out != NULL)  // position
			*c0_out = mPoints[segment] + (0.5f*u)*(C + u*(B + u*A));
		if (c1_out != NULL)  // velocity (first derivative)
			*c1_out = 0.5f*C + u*(B + 1.5f*u*A);
	}

	T evaluate(float t) const
	{
		T ret;
		evaluate(t, &ret, NULL);
		return ret;
	}

	T derivative(float t) const
	{
		T ret;
		evaluate(t, NULL, &ret);
		return ret;
	}

	// getters
	inline int n_control_points() const { return mPoints.size(); }
	inline const T& control_point(int i) const { return mPoints.at(i); }
	inline float total_length() const { return mTimePoints.back().distance; }
	inline unsigned int n_time_points() const { return mTimePoints.size(); }
	inline const TimePoint& time_point(unsigned int i) const { return mTimePoints[i]; }

private:
	static float segmentArcLength(const T* points, float u1, float u2)
	{
		if (u2 <= u1)
			return 0.0f;

		if (u1 < 0.0f)
			u1 = 0.0f;
		if (u2 > 1.0f)
			u2 = 1.0f;

		T A = 3.0f*points[1]
			- points[0]
			- 3.0f*points[2]
			+ points[3];
		T B = 2.0f*points[0]
			- 5.0f*points[1]
			+ 4.0f*points[2]
			- points[3];
		T C = points[2] - points[0];

		/*T a = points[1] + (0.5f*u1)*(C + u1*(B + u1*A));
		T b = points[1] + (0.5f*u2)*(C + u2*(B + u2*A));
		return (b - a).len();*/

		float sum = 0.0f;
		for (int j = 0; j < 5; ++j)
		{
			float u = 0.5f*((u2 - u1)*smX[j] + u2 + u1);
			T derivative = 0.5f*C + u*(B + 1.5f*u*A);
			sum += smC[j] * derivative.len();
		}
		sum *= 0.5f*(u2 - u1);
		return sum;
	}

	static std::vector<TimePoint> BuildTimeMap(const std::vector<T>& points)
	{
		assert(points.size() >= 4);

		// first, calculate arc lengths (skip endpoints)
		std::vector<float> segmentLengths(points.size() - 1);
		{
			segmentLengths[0] = 0.0f;
			segmentLengths[points.size() - 2] = 0.0f;
			for (unsigned int i = 1; i < points.size() - 2; i++)
			{
				segmentLengths[i] = segmentArcLength(&points[i - 1], 0.0f, 1.0f);
			}
		}

		// now build a time table
		std::vector<TimePoint> timePoints;
		{
			static const float subdiv_length = 0.001f;
			static const float min_travel_distance = 0.05f;

			timePoints.push_back(TimePoint{ 0, 1, 0 });

			float dist = 0.0f;  // total distance traveled
			for (unsigned int i = 1; i < points.size() - 2; i++)
			{
				const int n_segment_subdivisions = static_cast<int>(ceilf(segmentLengths[i] / subdiv_length));
				float u_pre = 0.0f;
				for (int j = 1; j <= n_segment_subdivisions; j++)  // inclusive of n_segment_subdivisions
				{
					const float u = static_cast<float>(j) / n_segment_subdivisions;
					const float subdiv_dist = segmentArcLength(&points[i - 1], u_pre, u);
					dist += subdiv_dist;
					u_pre = u;
					if ((dist - timePoints.back().distance) < min_travel_distance)
						continue;

					timePoints.push_back(TimePoint{ dist, i, u, });
				}
			}
		}

		return timePoints;
	}

	TimePoint timepoint_at_distance(float d) const
	{
		assert(mPoints.size() >= 4);

		if (d <= 0)
			return TimePoint{ 0, 1, 0 };
		else if (d >= mTotalLength)
			return TimePoint{ mTotalLength, (unsigned int)mPoints.size() - 3, 1.0f };

		unsigned int min = 0;
		unsigned int max = mTimePoints.size() - 1;
		while (min <= max)
		{
			unsigned int mid = (min + max) / 2;
			float d_at_mid = mTimePoints[mid].distance;
			if (d_at_mid < d)
			{
				min = mid + 1;
			} else if (d_at_mid > d)
			{
				max = mid - 1;
			} else {
				// hit it dead on
				return mTimePoints[mid];
			}
		}

		/*int min, max;
		min = max = -1;
		for (unsigned int i = 1; i < mTimePoints.size(); i++) {
			if (mTimePoints.at(i).distance > d) {
				min = i - 1;
				max = i;
				break;
			}
		}*/

		const TimePoint tp_min = mTimePoints.at(max);
		const TimePoint tp_max = mTimePoints.at(min);
		const float u0 = tp_min.segment_idx + tp_min.u;
		const float u1 = tp_max.segment_idx + tp_max.u;
		float t = (d - tp_min.distance) / (tp_max.distance - tp_min.distance);
		const float target_u = u0 + (u1 - u0) * t;
		const unsigned int target_segment_idx = static_cast<unsigned int>(floorf(target_u));
		return TimePoint{ d, target_segment_idx, target_u - target_segment_idx };
		//return mTimePoints[max];
		//return mTimePoints[0];
	}

private:
	std::vector<T> mPoints;
	std::vector<TimePoint> mTimePoints;
	float mTotalLength;
};

template <typename T>
constexpr float CatmullRom<T>::smX[5];

template <typename T>
constexpr float CatmullRom<T>::smC[5];
