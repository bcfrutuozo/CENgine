#include "Mesh.h"
#include "imgui/imgui.h"
#include "Surface.h"

#include <sstream>
#include <unordered_map>

ModelException::ModelException(const int line, const char* file, std::string note) noexcept
	:
	CENgineException(line, file),
	note(std::move(note))
{}

const char* ModelException::what() const noexcept
{
	std::ostringstream oss;
	oss << CENgineException::what() << std::endl
		<< "[Note] " << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ModelException::GetType() const noexcept
{
	return "CENgine Model Exception";
}

const std::string& ModelException::GetNote() const noexcept
{
	return note;
}

// Mesh
Mesh::Mesh(Graphics& graphics,
	std::vector<std::shared_ptr<Bind::Bindable>> psBinds)
{
	AddBind(Bind::Topology::Resolve(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	for (auto& pb : psBinds)
	{
		AddBind(std::move(pb));
	}

	AddBind(std::make_shared<Bind::TransformCbuf>(graphics, *this));
}

void Mesh::Draw(Graphics& graphics, DirectX::FXMMATRIX acculumatedTransform) const NOXND
{
	DirectX::XMStoreFloat4x4(&transformMatrix, acculumatedTransform);
	Drawable::Draw(graphics);
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transformMatrix);
}

// Node
Node::Node(int id, const std::string& name, std::vector<Mesh*> psMeshes, const DirectX::XMMATRIX& transformMatrix_in) NOXND
	:
	id(id),
	psMeshes(std::move(psMeshes)),
	name(name)
{
	DirectX::XMStoreFloat4x4(&transform, transformMatrix_in);
	DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
}

void Node::Draw(Graphics& graphics, DirectX::FXMMATRIX accumulatedTransform) const NOXND
{
	const auto built =
		DirectX::XMLoadFloat4x4(&appliedTransform) *
		DirectX::XMLoadFloat4x4(&transform) *
		accumulatedTransform;

	for (const auto pm : psMeshes)
	{
		pm->Draw(graphics, built);
	}

	for (const auto& pc : psChildren)
	{
		pc->Draw(graphics, built);
	}
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	DirectX::XMStoreFloat4x4(&appliedTransform, transform);
}


void Node::AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG)
{
	assert(pChild);
	psChildren.push_back(std::move(pChild));
}

void Node::ShowTree(Node*& pSelectedNode) const noexcept
{
	// If there's no selected node, set selectedId to an impossible value
	const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();

	// Build up flags for current node
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| (GetId() == selectedId ? ImGuiTreeNodeFlags_Selected : 0)
		| (psChildren.empty() ? ImGuiTreeNodeFlags_Leaf : 0);

	// If tree node expanded, recursively render all children
	const auto expanded = ImGui::TreeNodeEx(
		reinterpret_cast<void*>(static_cast<intptr_t>(GetId())),
		node_flags,
		name.c_str());

	// Processing for selected node
	if (ImGui::IsItemClicked())
	{
		pSelectedNode = const_cast<Node*>(this);
	}

	// Recursive rendering of open node's children
	if (expanded)
	{
		for (const auto& pChild : psChildren)
		{
			pChild->ShowTree(pSelectedNode);
		}
		ImGui::TreePop();
	}
}

int Node::GetId() const noexcept
{
	return id;
}

// Model
class ModelWindow // pImpl idiom, only defined in this .cpp
{
public:
	void Show(const char* windowName, const Node& root) noexcept
	{
		// window name defaults to "Model"
		windowName = windowName ? windowName : "Model";

		// Need an int to track node indices and selected node
		int nodeIndexTracker = 0;
		if (ImGui::Begin(windowName))
		{
			ImGui::Columns(2, nullptr, true);
			root.ShowTree(pSelectedNode);

			ImGui::NextColumn();
			if (pSelectedNode != nullptr)
			{
				auto& transform = transforms[pSelectedNode->GetId()];
				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
			}
		}
		ImGui::End();
	}
	DirectX::XMMATRIX GetTransform() const noexcept
	{
		assert(pSelectedNode != nullptr);

		const auto& transform = transforms.at(pSelectedNode->GetId());
		return DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}

	Node* GetSelectedNode() const noexcept
	{
		return pSelectedNode;
	}
private:

	std::optional<int> selectedIndex;

	Node* pSelectedNode;
	struct TransformParameters
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	std::unordered_map<int, TransformParameters> transforms;
};

Model::Model(Graphics& graphics, const std::string filename)
	:
	pWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(filename.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals);

	if (pScene == nullptr)
	{
		throw ModelException(__LINE__, __FILE__, imp.GetErrorString());
	}

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		psMeshes.push_back(ParseMesh(graphics, *pScene->mMeshes[i], pScene->mMaterials));
	}

	int nextId = 0;
	pRoot = ParseNode(nextId, *pScene->mRootNode);
}

Model::~Model() noexcept
{}

void Model::Draw(Graphics& graphics) const
{
	if (auto node = pWindow->GetSelectedNode())
	{
		node->SetAppliedTransform(pWindow->GetTransform());
	}

	pRoot->Draw(graphics, DirectX::XMMatrixIdentity());
}

void Model::ShowWindow(const char* windowName) noexcept
{
	pWindow->Show(windowName, *pRoot);
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& graphics, const aiMesh& mesh, const aiMaterial* const* psMaterials)
{
	using CENgineexp::VertexLayout;

	CENgineexp::VertexBuffer vBuffer(std::move(
		VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Texture2D)));

	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vBuffer.EmplaceBack(
			*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i]));
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

	std::vector<std::shared_ptr<Bind::Bindable>> psBindables;

	using namespace std::string_literals;
	const auto base = "Models\\nano_textured\\"s;

	bool hasSpecularMap = false;
	float shininess = 35.0f;
	if (mesh.mMaterialIndex >= 0)
	{
		auto& material = *psMaterials[mesh.mMaterialIndex];
		aiString texFilename;

		material.GetTexture(aiTextureType_DIFFUSE, 0, &texFilename);
		psBindables.push_back(Bind::Texture::Resolve(graphics, base + texFilename.C_Str()));

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFilename) == aiReturn_SUCCESS)
		{
			psBindables.push_back(Bind::Texture::Resolve(graphics, base + texFilename.C_Str(), 1));
			hasSpecularMap = true;
		}
		else
		{
			material.Get(AI_MATKEY_SHININESS, shininess);
		}

		psBindables.push_back(Bind::Sampler::Resolve(graphics));
	}

	auto meshTag = base + "%" + mesh.mName.C_Str();

	psBindables.push_back(Bind::VertexBuffer::Resolve(graphics, meshTag, vBuffer));
	psBindables.push_back(Bind::IndexBuffer::Resolve(graphics, meshTag, indices));

	auto pvs = Bind::VertexShader::Resolve(graphics, "PhongVS.cso");
	auto pvsbc = pvs->GetByteCode();
	psBindables.push_back(std::move(pvs));

	psBindables.push_back(Bind::InputLayout::Resolve(graphics, vBuffer.GetLayout(), pvsbc));

	if (hasSpecularMap)
	{
		psBindables.push_back(Bind::PixelShader::Resolve(graphics, "PhongPSSpecMap.cso"));
	}
	else
	{
		psBindables.push_back(Bind::PixelShader::Resolve(graphics, "PhongPS.cso"));

		struct PSMaterialConstant
		{
			float specularIntensity = 0.8f;
			float specularPower;
			float padding[2];
		} pmc;
		pmc.specularPower = shininess;

		/* This is clearly an issue because all meshes will share the same material constant, but might have
		 * different NS (specular power) specified for each in the material properties... bad conflict!	 */
		
		psBindables.push_back(Bind::PixelConstantBuffer<PSMaterialConstant>::Resolve(graphics, pmc, 1u));
	}
	
	return std::make_unique<Mesh>(graphics, std::move(psBindables));
}

std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node) noexcept
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

	auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(psCurrentMeshes), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
	}

	return pNode;
}