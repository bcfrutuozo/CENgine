#pragma once

#include "Math.h"

constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

template <typename T>

constexpr auto Square(const T& x)
{
	return x * x;
}

template<typename T>
T WrapAngle(T theta)
{
	constexpr T twoPi = (T)2 * (T)PI_D;
	const T mod = fmod(theta, twoPi);

	if(mod > (T)PI_D)
	{
		return mod - twoPi;
	}
	else if(mod < (T)PI_D)
	{
		return mod + twoPi;
	}

	return mod;
}

template<typename T>
constexpr T Interpolate(const T& source, const T& destination, float alpha)
{
	return source + (destination - source) * alpha;
}

template<typename T>
constexpr T ToRadian(T degree)
{
	return degree * PI / static_cast<T>(180.0);
}