#include "TexturePreprocessor.h"
#include "Math.h"
#include "ModelException.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>
#include <sstream>

template<typename F>
inline void TexturePreprocessor::TransformFile(const std::string& pathInput, const std::string& pathOutput, F&& func)
{
	auto surface = Surface::FromFile(pathInput);
	TransformSurface(surface, func);
	surface.Save(pathOutput);
}

template<typename F>
inline void TexturePreprocessor::TransformSurface(Surface& surface, F&& func)
{
	const auto width = surface.GetWidth();
	const auto height = surface.GetHeight();
	for(unsigned int y = 0; y < height; y++)
	{
		for(unsigned int x = 0; x < width; x++)
		{
			const auto n = ColorToVector(surface.GetPixel(x, y));
			surface.PutPixel(x, y, VectorToColor(func(n, x, y)));
		}
	}
}

void TexturePreprocessor::FlipYAllNormalMapsInObject(const std::string& path)
{
	const auto rootPath = std::filesystem::path{ path }.parent_path().string() + "\\";

	// Load scene from .obj file to get our list of normal maps in the materials
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(path.c_str(), 0u);
	if (pScene == nullptr)
	{
		throw ModelException(__LINE__, __FILE__, imp.GetErrorString());
	}

	// Loop through materials and process any normal maps
	for (auto i = 0u; i < pScene->mNumMaterials; i++)
	{
		const auto& mat = *pScene->mMaterials[i];
		aiString texFileName;
		if (mat.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
		{
			const auto path = rootPath + texFileName.C_Str();
			FlipYNormalMap(path, path);
		}
	}
}

void TexturePreprocessor::FlipYNormalMap(const std::string& pathInput, const std::string& pathOutput)
{
	// Function for processing each normal in texture
	const auto flipY = DirectX::XMVectorSet(1.0f, -1.0f, 1.0f, 1.0f);
	const auto ProcessNormal = [&flipY](DirectX::FXMVECTOR n, int x, int y) -> DirectX::XMVECTOR
	{
		return DirectX::XMVectorMultiply(n, flipY);
	};

	// Execute processing over every texel in file
	TransformFile(pathInput, pathOutput, ProcessNormal);
}

void TexturePreprocessor::ValidateNormalMap(const std::string& pathInput, const float thresholdMin, const float thresholdMax)
{
	OutputDebugStringA(("Validating normal map [" + pathInput + "]\n").c_str());

	// Function for processing each normal in texture
	auto sum = DirectX::XMVectorZero();
	const auto ProcessNormal = [&thresholdMin, &thresholdMax, &sum](DirectX::FXMVECTOR n, int x, int y) -> DirectX::XMVECTOR
	{
		const float len = DirectX::XMVectorGetX(DirectX::XMVector3Length(n));
		const float z = DirectX::XMVectorGetZ(n);
		if (len < thresholdMin || len > thresholdMax)
		{
			DirectX::XMFLOAT3 vec;
			DirectX::XMStoreFloat3(&vec, n);
			std::ostringstream oss;
			oss << "Bad normal length: " << len << " at: (" << x << "," << y << ") normal: (" << vec.x << "," << vec.y << "," << vec.z << ")\n";
			OutputDebugStringA(oss.str().c_str());
		}
		if (z < 0.0f)
		{
			DirectX::XMFLOAT3 vec;
			DirectX::XMStoreFloat3(&vec, n);
			std::ostringstream oss;
			oss << "Bad normal Z direction at: (" << x << "," << y << ") normal: (" << vec.x << "," << vec.y << "," << vec.z << ")\n";
			OutputDebugStringA(oss.str().c_str());
		}
		sum = DirectX::XMVectorAdd(sum, n);
		return n;
	};

	// Execute the validation for each texel
	auto surface = Surface::FromFile(pathInput);
	TransformSurface(surface, ProcessNormal);

	// Output bias
	{
		DirectX::XMFLOAT2 sumv;
		DirectX::XMStoreFloat2(&sumv, sum);
		std::ostringstream oss;
		oss << "Normal map biases: (" << sumv.x << "," << sumv.y << ")\n";
		OutputDebugStringA(oss.str().c_str());
	}
}

void TexturePreprocessor::MakeStripes(const std::string& pathOutput, const unsigned int size, const unsigned int stripeWidth)
{
	// Make sure texture dimension is power of 2
	auto power = log2(size);
	assert(modf(power, &power) == 0.0);

	// Make sure stripe width enables at least 2 stripes
	assert(stripeWidth < (size / 2));

	Surface surface(size, size);
	for(unsigned int y = 0; y < size; y++)
	{
		for(unsigned int x = 0; x < size; x++)
		{
			Surface::Color c = { 0, 0, 0 };
			if((x / stripeWidth) % 2 == 0)
			{
				c = { 255, 255, 255 };
			}

			surface.PutPixel(x, y, c);
		}
	}

	surface.Save(pathOutput);
}

DirectX::XMVECTOR TexturePreprocessor::ColorToVector(Surface::Color c) noexcept
{
	auto n = DirectX::XMVectorSet(static_cast<float>(c.GetR()), static_cast<float>(c.GetG()), static_cast<float>(c.GetB()), 0.0f);
	const auto all255 = DirectX::XMVectorReplicate(2.0f / 255.0f);
	n = DirectX::XMVectorMultiply(n, all255);
	const auto all1 = DirectX::XMVectorReplicate(1.0f);
	n = DirectX::XMVectorSubtract(n, all1);
	return n;
}

Surface::Color TexturePreprocessor::VectorToColor(DirectX::FXMVECTOR n) noexcept
{
	const auto all1 = DirectX::XMVectorReplicate(1.0f);
	DirectX::XMVECTOR nOut = DirectX::XMVectorAdd(n, all1);
	const auto all255 = DirectX::XMVectorReplicate(255.0f / 2.0f);
	nOut = DirectX::XMVectorMultiply(nOut, all255);
	DirectX::XMFLOAT3 floats;
	DirectX::XMStoreFloat3(&floats, nOut);
	return {
		static_cast<unsigned char>(round(floats.x)),
		static_cast<unsigned char>(round(floats.y)),
		static_cast<unsigned char>(round(floats.z)),
	};
}
