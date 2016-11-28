#pragma once

template <typename T>
inline T clamp(T v, T min, T max) {
	if (v < min)
		v = min;
	else if (v > max)
		v = max;
	return v;
}

inline float randf(float LO, float HI)
{
	return LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
}