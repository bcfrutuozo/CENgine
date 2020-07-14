#pragma once

#include "Math.h"

constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

template<typename T>
constexpr auto Square(const T& x) noexcept
{
	return x * x;
}

template<typename T>
T WrapAngle(T theta) noexcept
{
	constexpr T twoPi = (T)2 * (T)PI_D;
	const T mod = (T)fmod(theta, twoPi);

	if(mod > (T)PI_D)
	{
		return mod - twoPi;
	}
	else if(mod < -(T)PI_D)
	{
		return mod + twoPi;
	}

	return mod;
}

template<typename T>
constexpr T Interpolate(const T& source, const T& destination, float alpha) noexcept
{
	return source + (destination - source) * alpha;
}

template<typename T>
constexpr T ToRadian(T degree) noexcept
{
	return degree * PI / (T)(180.0);
}

template<typename T>
constexpr T Gauss(T x, T sigma) noexcept
{
	const auto ss = Square(sigma);
	return ((T)1.0 / Square((T)2.0 * (T)PI_D*ss)) * exp(-Square(x) / (T)2.0 * ss);
}