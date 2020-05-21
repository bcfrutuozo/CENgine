#include "Mesh.h"

// Mesh
Mesh::Mesh(Graphics& graphics,
	std::vector<std::unique_ptr<Bind::Bindable>> psBinds)
{
	if (!IsStaticInitialized())
	{
		AddStaticBind(std::make_unique<Bind::Topology>(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	for (auto& pb : psBinds)
	{
		if (auto pi = dynamic_cast<Bind::IndexBuffer*>(pb.get()))
		{
			AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer> {pi});
			pb.release();
		}
		else
		{
			AddBind(std::move(pb));
		}
	}

	AddBind(std::make_unique<Bind::TransformCbuf>(graphics, *this));
}

void Mesh::Draw(Graphics& graphics, DirectX::FXMMATRIX acculumatedTransform) const noexcept(!IS_DEBUG)
{
	DirectX::XMStoreFloat4x4(&transformMatrix, acculumatedTransform);
	Drawable::Draw(graphics);
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transformMatrix);
}

// Node
Node::Node(std::vector<Mesh*> psMeshes, const DirectX::XMMATRIX& transformMatrix) noexcept(!IS_DEBUG)
	:
	psMeshes(std::move(psMeshes))
{
	DirectX::XMStoreFloat4x4(&this->transformMatrix, transformMatrix);
}

void Node::Draw(Graphics& graphics, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
	const auto built = DirectX::XMLoadFloat4x4(&transformMatrix) * accumulatedTransform;
	for (const auto pm : psMeshes)
	{
		pm->Draw(graphics, built);
	}

	for (const auto& pc : psChildren)
	{
		pc->Draw(graphics, built);
	}
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG)
{
	assert(pChild);
	psChildren.push_back(std::move(pChild));
}

// Model

Model::Model(Graphics& graphics, const std::string filename)
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(filename.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices);

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		psMeshes.push_back(ParseMesh(graphics, *pScene->mMeshes[i]));
	}

	pRoot = ParseNode(*pScene->mRootNode);
}

void Model::Draw(Graphics& graphics, DirectX::FXMMATRIX transform) const
{
	pRoot->Draw(graphics, transform);
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& graphics, const aiMesh& mesh)
{
	using CENgineexp::VertexLayout;

	CENgineexp::VertexBuffer vBuffer(std::move(
		VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)));

	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vBuffer.EmplaceBack(
			*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]));
	}

	std::vector<unsigned short> indices;
	indices.reserve(3 * mesh.mNumFaces);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	std::vector<std::unique_ptr<Bind::Bindable>> psBindables;
	psBindables.push_back(std::make_unique<Bind::VertexBuffer>(graphics, vBuffer));
	psBindables.push_back(std::make_unique<Bind::IndexBuffer>(graphics, indices));

	auto pvs = std::make_unique<Bind::VertexShader>(graphics, L"PhongVS.cso");
	auto pvsbc = pvs->GetByteCode();
	psBindables.push_back(std::move(pvs));
	psBindables.push_back(std::make_unique<Bind::PixelShader>(graphics, L"PhongPS.cso"));
	psBindables.push_back(std::make_unique<Bind::InputLayout>(graphics, vBuffer.GetLayout().GetD3DLayout(), pvsbc));

	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color = { 0.6f,0.6f,0.8f };
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	} pmc;
	psBindables.push_back(std::make_unique<Bind::PixelConstantBuffer<PSMaterialConstant>>(graphics, pmc, 1u));

	return std::make_unique<Mesh>(graphics, std::move(psBindables));

}

std::unique_ptr<Node> Model::ParseNode(const aiNode& node)
{
	const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
	));

	std::vector<Mesh*> psCurrentMeshes;
	psCurrentMeshes.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		psCurrentMeshes.push_back(psMeshes.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(std::move(psCurrentMeshes), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(*node.mChildren[i]));
	}

	return pNode;
}