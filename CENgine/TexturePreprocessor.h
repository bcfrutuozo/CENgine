#pragma once

#include "Surface.h"

#include <string>
#include <DirectXMath.h>

class TexturePreprocessor
{
public:

	static void FlipYAllNormalMapsInObject(const std::string& path);
	static void FlipYNormalMap(const std::string& pathInput, const std::string& pathOutput);
	static void ValidateNormalMap(const std::string& pathInput, const float thresholdMin, const float thresholdMax);
	static void MakeStripes(const std::string& pathOutput, const unsigned int size, const unsigned int stripeWidth);
private:

	template<typename F>
	static void TransformFile(const std::string& pathInput, const std::string& pathOutput, F&& func);

	template<typename F>
	static void TransformSurface(Surface& surface, F && func);

	static DirectX::XMVECTOR ColorToVector(Surface::Color c) noexcept;
	static Surface::Color VectorToColor(DirectX::FXMVECTOR n) noexcept;
};