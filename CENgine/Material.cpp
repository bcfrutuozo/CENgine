#include "Material.h"
#include "BindableCommon.h"
#include "DynamicConstant.h"
#include "DynamicConstantBuffer.h"
#include "TransformCbufScaling.h"
#include "Stencil.h"
#include "Channels.h"

#include <filesystem>

Material::Material(Graphics& graphics, const aiMaterial& material, const std::filesystem::path& modelPath) NOXND
	:
modelPath(modelPath.string())
{
	const auto rootPath = modelPath.parent_path().string() + "\\";
	{
		aiString temp;
		material.Get(AI_MATKEY_NAME, temp);
		name = temp.C_Str();
	}

	// Phong technique
	{
		Technique phong{ "Phong", Channel::main };
		Step step("lambertian");
		std::string shaderCode = "Phong";
		aiString texFileName;

		// Common (pre)
		vtxLayout.Append(CENgineexp::VertexLayout::Position3D);
		vtxLayout.Append(CENgineexp::VertexLayout::Normal);
		DRR::IncompleteLayout pscLayout;
		bool hasTexture = false;
		bool hasGlossAlpha = false;

		// Diffuse
		{
			bool hasAlpha = false;
			if(material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += "Dif";
				vtxLayout.Append(CENgineexp::VertexLayout::Texture2D);
				auto tex = Bind::Texture::Resolve(graphics, rootPath + texFileName.C_Str());
				if(tex->HasAlpha())
				{
					hasAlpha = true;
					shaderCode += "Msk";
				}
				step.AddBindable(std::move(tex));
			}
			else
			{
				pscLayout.Add<DRR::Float3>("materialColor");
			}

			step.AddBindable(Bind::Rasterizer::Resolve(graphics, hasAlpha));
		}

		// Specular
		{
			if(material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += "Spc";
				vtxLayout.Append(CENgineexp::VertexLayout::Texture2D);
				auto tex = Bind::Texture::Resolve(graphics, rootPath + texFileName.C_Str(), 1);
				hasGlossAlpha = tex->HasAlpha();
				step.AddBindable(std::move(tex));
				pscLayout.Add<DRR::Bool>("useGlossAlpha");
				pscLayout.Add<DRR::Bool>("useSpecularMap");
			}

			pscLayout.Add<DRR::Float3>("specularColor");
			pscLayout.Add<DRR::Float>("specularWeight");
			pscLayout.Add<DRR::Float>("specularGloss");
		}

		// Normal
		{
			if(material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += "Nrm";
				vtxLayout.Append(CENgineexp::VertexLayout::Texture2D);
				vtxLayout.Append(CENgineexp::VertexLayout::Tangent);
				vtxLayout.Append(CENgineexp::VertexLayout::Bitangent);
				step.AddBindable(Bind::Texture::Resolve(graphics, rootPath + texFileName.C_Str(), 2));
				pscLayout.Add<DRR::Bool>("useNormalMap");
				pscLayout.Add<DRR::Float>("normalMapWeight");
			}
		}

		// Common (post)
		{
			step.AddBindable(std::make_shared<Bind::TransformCbuf>(graphics, 0u));
			auto pvs = Bind::VertexShader::Resolve(graphics, shaderCode + "_VS.cso");
			step.AddBindable(Bind::InputLayout::Resolve(graphics, vtxLayout, *pvs));

			step.AddBindable(std::move(pvs));
			step.AddBindable(Bind::PixelShader::Resolve(graphics, shaderCode + "_PS.cso"));

			if(hasTexture)
			{
				step.AddBindable(Bind::Sampler::Resolve(graphics));
			}

			// PS material parameters (constant buffer)
			DRR::Buffer buffer{ std::move(pscLayout) };
			if(auto r = buffer["materialColor"]; r.Exists())
			{
				aiColor3D color = { 0.45f, 0.45f, 0.85f };
				material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}

			buffer["useGlossAlpha"].SetIfExists(hasGlossAlpha);
			buffer["useSpecularMap"].SetIfExists(true);
			if(auto r = buffer["specularColor"]; r.Exists())
			{
				aiColor3D color = { 0.18f, 0.18f, 0.18f };
				material.Get(AI_MATKEY_COLOR_SPECULAR, color);
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}

			buffer["specularWeight"].SetIfExists(1.0f);
			if(auto r = buffer["specularGloss"]; r.Exists())
			{
				float gloss = 8.0f;
				material.Get(AI_MATKEY_SHININESS, gloss);
				r = gloss;
			}

			buffer["useNormalMap"].SetIfExists(true);
			buffer["normalMapWeight"].SetIfExists(1.0f);
			step.AddBindable(std::make_unique<Bind::DynamicCachingPixelConstantBuffer>(graphics, std::move(buffer), 1u));
		}


		phong.AddStep(std::move(step));
		techniques.push_back(std::move(phong));
	}

	// Outline technique
	{
		Technique outline{ "Outline", Channel::main, false };
		{
			Step mask("outlineMask");

			mask.AddBindable(Bind::InputLayout::Resolve(graphics, vtxLayout, *Bind::VertexShader::Resolve(graphics, "Solid_VS.cso")));
			mask.AddBindable(std::make_shared<Bind::TransformCbuf>(graphics));

			outline.AddStep(std::move(mask));
		}
		{
			Step draw("outlineDraw");

			{
				DRR::IncompleteLayout lay;
				lay.Add<DRR::Float3>("materialColor");
				auto buffer = DRR::Buffer(std::move(lay));
				buffer["materialColor"] = DirectX::XMFLOAT3{ 1.0f, 0.4f, 0.4f };
				draw.AddBindable(std::make_shared<Bind::DynamicCachingPixelConstantBuffer>(graphics, buffer, 1u));
			}


			draw.AddBindable(Bind::InputLayout::Resolve(graphics, vtxLayout, *Bind::VertexShader::Resolve(graphics, "Solid_VS.cso")));

			draw.AddBindable(std::make_shared<Bind::TransformCbuf>(graphics));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			outline.AddStep(std::move(draw));

		}

		techniques.push_back(std::move(outline));
	}
}

CENgineexp::VertexBuffer Material::ExtractVertices(const aiMesh& mesh) const noexcept
{
	return { vtxLayout, mesh };
}

std::vector<unsigned short> Material::ExtractIndices(const aiMesh& mesh) const noexcept
{
	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for(unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
	return indices;
}
std::shared_ptr<Bind::VertexBuffer> Material::MakeVertexBindable(Graphics& graphics, const aiMesh& mesh, float scale) const NOXND
{
	auto vtc = ExtractVertices(mesh);

	if(scale != 1.0f)
	{
		for(auto i = 0u; i < vtc.Size(); i++)
		{
			DirectX::XMFLOAT3& pos = vtc[i].Attr<CENgineexp::VertexLayout::ElementType::Position3D>();
			pos.x *= scale;
			pos.y *= scale;
			pos.z *= scale;
		}
	}

	return Bind::VertexBuffer::Resolve(graphics, MakeMeshTag(mesh), std::move(vtc));
}
std::shared_ptr<Bind::IndexBuffer> Material::MakeIndexBindable(Graphics& graphics, const aiMesh& mesh) const NOXND
{
	return Bind::IndexBuffer::Resolve(graphics, MakeMeshTag(mesh), ExtractIndices(mesh));
}
std::string Material::MakeMeshTag(const aiMesh& mesh) const noexcept
{
	return modelPath + "%" + mesh.mName.C_Str();
}

std::vector<Technique> Material::GetTechniques() const noexcept
{
	return techniques;
}