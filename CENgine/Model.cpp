#include "Model.h"
#include "ModelException.h"
#include "Node.h"
#include "Mesh.h"
#include "Material.h"
#include "Matrix.h"

#pragma warning(push)
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#pragma warning(pop)

Model::Model(Graphics& graphics, const std::string& path, const float scale)
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(path.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace);

	if(pScene == nullptr)
	{
		throw ModelException(__LINE__, __FILE__, imp.GetErrorString());
	}

	// Parse materials
	std::vector<Material> materials;
	materials.reserve(pScene->mNumMaterials);
	for(size_t i = 0; i < pScene->mNumMaterials; i++)
	{
		materials.emplace_back(graphics, *pScene->mMaterials[i], path);
	}

	for(size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		const auto& mesh = *pScene->mMeshes[i];
		psMeshes.push_back(std::make_unique<Mesh>(graphics, materials[mesh.mMaterialIndex], mesh, scale));
	}

	int nextId = 0;
	pRoot = ParseNode(nextId, *pScene->mRootNode, scale);
}

Model::~Model() noexcept
{ }

void Model::Submit(size_t channels) const NOXND
{
	pRoot->Submit(channels, DirectX::XMMatrixIdentity());
}

void Model::SetRootTransform(DirectX::FXMMATRIX transformMatrix) noexcept
{
	pRoot->SetAppliedTransform(transformMatrix);
}

void Model::Accept(ModelProbe& probe)
{
	pRoot->Accept(probe);
}

void Model::LinkTechniques(RGP::RenderGraph& renderGraph)
{
	for(auto& pMesh : psMeshes)
	{
		pMesh->LinkTechniques(renderGraph);
	}
}

std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node, float scale) noexcept
{
	const auto transform = ScaleTranslation(DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
	)), scale);

	std::vector<Mesh*> psCurrentMeshes;
	psCurrentMeshes.reserve(node.mNumMeshes);
	for(size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		psCurrentMeshes.push_back(psMeshes.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(psCurrentMeshes), transform);
	for(size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i], scale));
	}

	return pNode;
}