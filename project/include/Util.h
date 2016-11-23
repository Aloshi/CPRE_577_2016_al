#pragma once

template <typename T>
inline T clamp(T v, T min, T max) {
	if (v < min)
		v = min;
	else if (v > max)
		v = max;
	return v;
}