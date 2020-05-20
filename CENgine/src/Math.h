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
	const T modded = fmod(theta, static_cast<T>(2.0) * static_cast<T>(PI_D));
	return (modded > static_cast<T>(PI_D)) ?
				(modded - static_cast<T>(2.0) * static_cast<T>(PI_D)) :
				modded;
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