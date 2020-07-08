#include "Mesh.h"
#include "imgui/imgui.h"
#include "Surface.h"
#include "Matrix.h"
#include "DynamicConstant.h"
#include "DynamicConstantBuffer.h"
#include "LayoutCodex.h"

#include <sstream>
#include <unordered_map>
#include <filesystem>

ModelException::ModelException(const int line, const char* file, std::string note) noexcept
	:
	CENgineException(line, file),
	note(std::move(note))
{ }

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

	for(auto& pb : psBinds)
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

	for(const auto pm : psMeshes)
	{
		pm->Draw(graphics, built);
	}

	for(const auto& pc : psChildren)
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
	if(ImGui::IsItemClicked())
	{
		pSelectedNode = const_cast<Node*>(this);
	}

	// Recursive rendering of open node's children
	if(expanded)
	{
		for(const auto& pChild : psChildren)
		{
			pChild->ShowTree(pSelectedNode);
		}
		ImGui::TreePop();
	}
}

const DirectX::XMFLOAT4X4& Node::GetAppliedTransform() const noexcept
{
	return appliedTransform;
}

int Node::GetId() const noexcept
{
	return id;
}

const DRR::Buffer* Node::GetMaterialConstants() const NOXND
{
	if( psMeshes.size() == 0 )
	{
		return nullptr;
	}
	auto pBindable = psMeshes.front()->QueryBindable<Bind::DynamicCachingPixelConstantBuffer>();
	return &pBindable->GetBuffer();
}

void Node::SetMaterialConstants( const DRR::Buffer& buffer_in ) NOXND
{
	auto pcb = psMeshes.front()->QueryBindable<Bind::DynamicCachingPixelConstantBuffer>();
	assert( pcb != nullptr );
	pcb->SetBuffer( buffer_in );
}

// Model
class ModelWindow // pImpl idiom, only defined in this .cpp
{
public:
	void Show(Graphics& graphics, const char* windowName, const Node& root) noexcept
	{
		// window name defaults to "Model"
		windowName = windowName ? windowName : "Model";
		// need an ints to track node indices and selected node
		int nodeIndexTracker = 0;
		if(ImGui::Begin(windowName))
		{
			ImGui::Columns(2, nullptr, true);
			root.ShowTree(pSelectedNode);

			ImGui::NextColumn();
			if(pSelectedNode != nullptr)
			{
				const auto id = pSelectedNode->GetId();
				auto i = transforms.find(id);

				if(i == transforms.end())
				{
					const auto& applied = pSelectedNode->GetAppliedTransform();
					const auto angles = ExtractEulerAngles(applied);
					const auto translation = ExtractTranslation(applied);

					TransformParameters tp;
					tp.roll = angles.z;
					tp.pitch = angles.x;
					tp.yaw = angles.y;
					tp.x = translation.x;
					tp.y = translation.y;
					tp.z = translation.z;
					auto pMatConst = pSelectedNode->GetMaterialConstants();
					auto buf = pMatConst != nullptr ? std::optional<DRR::Buffer> { *pMatConst} : std::optional<DRR::Buffer>{};
					std::tie(i, std::ignore) = transforms.insert({ id, { tp, false, std::move(buf), false} });
				}
				{
					auto& transform = i->second.transformParams;

					// Dirty check
					auto& dirty = i->second.transformParamsDirty;
					const auto dcheck = [&dirty](bool changed)
					{
						dirty = dirty || changed;
					};

					// Widgetd
					ImGui::Text("Orientation");
					dcheck(ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f));
					dcheck(ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f));
					dcheck(ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f));
					ImGui::Text("Position");
					dcheck(ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f));
					dcheck(ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f));
					dcheck(ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f));

				}

				// Link imgui ctrl to our cached material params
				if( i->second.materialCbuf )
				{
					auto& mat = *i->second.materialCbuf;
					// dirty check
					auto& dirty = i->second.materialCbufDirty;
					const auto dcheck = [&dirty]( bool changed )
					{
						dirty = dirty || changed; 
					};

					// widgets
					ImGui::Text( "Material" );
					if( auto v = mat["normalMapEnabled"]; v.Exists() )
					{
						dcheck( ImGui::Checkbox( "Norm Map",&v ) );
					}
					if( auto v = mat["specularMapEnabled"]; v.Exists() )
					{
						dcheck( ImGui::Checkbox( "Spec Map",&v ) );
					}
					if( auto v = mat["hasGlossMap"]; v.Exists() )
					{
						dcheck( ImGui::Checkbox( "Gloss Map",&v ) );
					}
					if( auto v = mat["materialColor"]; v.Exists() )
					{
						dcheck( ImGui::ColorPicker3( "Diff Color",reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v)) ) );
					}
					if( auto v = mat["specularPower"]; v.Exists() )
					{
						dcheck( ImGui::SliderFloat( "Spec Power",&v,0.0f,100.0f,"%.1f",1.5f ) );
					}
					if( auto v = mat["specularColor"]; v.Exists() )
					{
						dcheck( ImGui::ColorPicker3( "Spec Color",reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT3&>(v)) ) );
					}
					if( auto v = mat["specularMapWeight"]; v.Exists() )
					{
						dcheck( ImGui::SliderFloat( "Spec Weight",&v,0.0f,4.0f ) );
					}
					if( auto v = mat["specularIntensity"]; v.Exists() )
					{
						dcheck( ImGui::SliderFloat( "Spec Intens",&v,0.0f,1.0f ) );
					}
				}
			}
		}
		ImGui::End();
	}

	void ApplyParameters() NOXND
	{
		if(TransformDirty())
		{
			pSelectedNode->SetAppliedTransform(GetTransform());
			ResetTransformDirty();
		}
		if(MaterialDirty())
		{
			pSelectedNode->SetMaterialConstants(GetMaterial());
			ResetMaterialDirty();
		}
	}

private:

	DirectX::XMMATRIX GetTransform() const NOXND
	{
		assert(pSelectedNode != nullptr);
		const auto& transform = transforms.at(pSelectedNode->GetId()).transformParams;
		return DirectX::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
			DirectX::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}

	const DRR::Buffer& GetMaterial() const NOXND
	{
		assert(pSelectedNode != nullptr);
		const auto& mat = transforms.at(pSelectedNode->GetId()).materialCbuf;
		assert(mat);
		return *mat;
	}

	bool TransformDirty() const NOXND
	{
		return pSelectedNode && transforms.at(pSelectedNode->GetId()).transformParamsDirty;
	}

	void ResetTransformDirty() NOXND
	{
		transforms.at(pSelectedNode->GetId()).transformParamsDirty = false;
	}

	bool MaterialDirty() const NOXND
	{
		return pSelectedNode && transforms.at(pSelectedNode->GetId()).materialCbufDirty;
	}

	void ResetMaterialDirty() NOXND
	{
		transforms.at(pSelectedNode->GetId()).materialCbufDirty = false;
	}

	bool IsDirty() const NOXND
	{
		return TransformDirty() || MaterialDirty();
	}

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
	struct NodeData
	{
		TransformParameters transformParams;
		bool transformParamsDirty;
		std::optional<DRR::Buffer> materialCbuf;
		bool materialCbufDirty;
	};
	std::unordered_map<int, NodeData> transforms;
};

Model::Model(Graphics& graphics, const std::string& path, const float scale)
	:
	pWindow(std::make_unique<ModelWindow>())
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

	for(size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		psMeshes.push_back(ParseMesh(graphics, *pScene->mMeshes[i], pScene->mMaterials, path, scale));
	}

	int nextId = 0;
	pRoot = ParseNode(nextId, *pScene->mRootNode);
}

Model::~Model() noexcept
{ }

void Model::Draw(Graphics& graphics) const NOXND
{
	pWindow->ApplyParameters();
	pRoot->Draw(graphics, DirectX::XMMatrixIdentity());
}

void Model::ShowWindow(Graphics& graphics, const char* windowName) noexcept
{
	pWindow->Show(graphics, windowName, *pRoot);
}

void Model::SetRootTransform(DirectX::FXMMATRIX transformMatrix) noexcept
{
	pRoot->SetAppliedTransform(transformMatrix);
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& graphics, const aiMesh& mesh, const aiMaterial* const* psMaterials, const std::filesystem::path& path, const float scale)
{
	using CENgineexp::VertexLayout;
	using namespace std::string_literals;

	std::vector<std::shared_ptr<Bind::Bindable>> psBindables;

	const auto rootPath = path.parent_path().string() + "\\";

	bool hasSpecularMap = false;
	bool hasAlphaGloss = false;
	bool hasAlphaDiffuse = false;
	bool hasNormalMap = false;
	bool hasDiffuseMap = false;
	float shininess = 2.0f;
	DirectX::XMFLOAT4 specularColor = { 0.18f,0.18f,0.18f,1.0f };
	DirectX::XMFLOAT4 diffuseColor = { 0.45f,0.45f,0.85f,1.0f };

	if(mesh.mMaterialIndex >= 0)
	{
		auto& material = *psMaterials[mesh.mMaterialIndex];
		aiString texFilename;

		if(material.GetTexture(aiTextureType_DIFFUSE, 0, &texFilename) == aiReturn_SUCCESS)
		{
			auto tex = Bind::Texture::Resolve(graphics, rootPath + texFilename.C_Str());
			hasAlphaDiffuse = tex->HasAlpha();
			psBindables.push_back(std::move(tex));
			hasDiffuseMap = true;
		}
		else
		{
			material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(diffuseColor));
		}

		if(material.GetTexture(aiTextureType_SPECULAR, 0, &texFilename) == aiReturn_SUCCESS)
		{
			auto tex = Bind::Texture::Resolve(graphics, rootPath + texFilename.C_Str(), 1);
			hasAlphaGloss = tex->HasAlpha();
			psBindables.push_back(std::move(tex));
			hasSpecularMap = true;
		}
		else
		{
			material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(specularColor));
		}

		if(!hasAlphaGloss)
		{
			material.Get(AI_MATKEY_SHININESS, shininess);
		}

		if(material.GetTexture(aiTextureType_NORMALS, 0, &texFilename) == aiReturn_SUCCESS)
		{
			auto tex = Bind::Texture::Resolve(graphics, rootPath + texFilename.C_Str(), 2);
			hasAlphaGloss = tex->HasAlpha();
			psBindables.push_back(std::move(tex));
			hasNormalMap = true;
		}

		if(hasDiffuseMap || hasSpecularMap || hasNormalMap)
		{
			psBindables.push_back(Bind::Sampler::Resolve(graphics));
		}
	}

	const auto meshTag = path.string() + "%" + mesh.mName.C_Str();

	if(hasDiffuseMap && hasNormalMap && hasSpecularMap)
	{
		CENgineexp::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
			.Append(VertexLayout::Tangent)
			.Append(VertexLayout::Bitangent)
			.Append(VertexLayout::Texture2D)));

		for(unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i]));
		}

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


		psBindables.push_back(Bind::VertexBuffer::Resolve(graphics, meshTag, vbuf));
		psBindables.push_back(Bind::IndexBuffer::Resolve(graphics, meshTag, indices));

		auto pvs = Bind::VertexShader::Resolve(graphics, "PhongVSNormalMap.cso");
		auto pvsbc = pvs->GetByteCode();
		psBindables.push_back(std::move(pvs));

		psBindables.push_back(Bind::PixelShader::Resolve(graphics, hasAlphaDiffuse ? "PhongPSSpecNormalMask.cso" : "PhongPSSpecNormalMap.cso"));
		psBindables.push_back(Bind::InputLayout::Resolve(graphics, vbuf.GetLayout(), pvsbc));

		DRR::IncompleteLayout lay;
		lay.Add<DRR::Bool>( "normalMapEnabled" );
		lay.Add<DRR::Bool>( "specularMapEnabled" );
		lay.Add<DRR::Bool>( "hasGlossMap" );
		lay.Add<DRR::Float>( "specularPower" );
		lay.Add<DRR::Float3>( "specularColor" );
		lay.Add<DRR::Float>( "specularMapWeight" );

		auto buf = DRR::Buffer( std::move( lay ) );
		buf["normalMapEnabled"] = true;
		buf["specularMapEnabled"] = true;
		buf["hasGlossMap"] = hasAlphaGloss;
		buf["specularPower"] = shininess;
		buf["specularColor"] = DirectX::XMFLOAT3{ 0.75f,0.75f,0.75f };
		buf["specularMapWeight"] = 0.671f;

		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		psBindables.push_back(std::make_shared<Bind::DynamicCachingPixelConstantBuffer>( graphics,buf,1u ));

	}
	else if(hasDiffuseMap && hasNormalMap)
	{
		CENgineexp::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
			.Append(VertexLayout::Tangent)
			.Append(VertexLayout::Bitangent)
			.Append(VertexLayout::Texture2D)
		));

		for(unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

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

		psBindables.push_back(Bind::VertexBuffer::Resolve(graphics, meshTag, vbuf));

		psBindables.push_back(Bind::IndexBuffer::Resolve(graphics, meshTag, indices));

		auto pvs = Bind::VertexShader::Resolve(graphics, "PhongVSNormalMap.cso");
		auto pvsbc = pvs->GetByteCode();
		psBindables.push_back(std::move(pvs));

		psBindables.push_back(Bind::PixelShader::Resolve(graphics, "PhongPSNormalMap.cso"));
		psBindables.push_back(Bind::InputLayout::Resolve(graphics, vbuf.GetLayout(), pvsbc));

		DRR::IncompleteLayout layout;
		layout.Add<DRR::Float>( "specularIntensity" );
		layout.Add<DRR::Float>( "specularPower" );
		layout.Add<DRR::Bool>( "normalMapEnabled" );

		auto cbuf = DRR::Buffer( std::move( layout ) );
		cbuf["specularIntensity"] = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
		cbuf["specularPower"] = shininess;
		cbuf["normalMapEnabled"] = true;

		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		psBindables.push_back(std::make_shared<Bind::DynamicCachingPixelConstantBuffer>( graphics,cbuf,1u ));
	}
	else if(hasDiffuseMap && !hasNormalMap && hasSpecularMap)
	{
		CENgineexp::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
			.Append(VertexLayout::Texture2D)
		));

		for(unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

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

		psBindables.push_back(Bind::VertexBuffer::Resolve(graphics, meshTag, vbuf));

		psBindables.push_back(Bind::IndexBuffer::Resolve(graphics, meshTag, indices));

		auto pvs = Bind::VertexShader::Resolve(graphics, "PhongVS.cso");
		auto pvsbc = pvs->GetByteCode();
		psBindables.push_back(std::move(pvs));

		psBindables.push_back(Bind::PixelShader::Resolve(graphics, "PhongPSSpec.cso"));

		psBindables.push_back(Bind::InputLayout::Resolve(graphics, vbuf.GetLayout(), pvsbc));

		DRR::IncompleteLayout lay;
		lay.Add<DRR::Float>( "specularPower" );
		lay.Add<DRR::Bool>( "hasGloss" );
		lay.Add<DRR::Float>( "specularMapWeight" );

		auto buf = DRR::Buffer( std::move( lay ) );
		buf["specularPower"] = shininess;
		buf["hasGloss"] = hasAlphaGloss;
		buf["specularMapWeight"] = 1.0f;

		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		psBindables.push_back(std::make_unique<Bind::DynamicCachingPixelConstantBuffer>( graphics,buf,1u ));
	}
	else if(hasDiffuseMap)
	{
		CENgineexp::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
			.Append(VertexLayout::Texture2D)
		));

		for(unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

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

		psBindables.push_back(Bind::VertexBuffer::Resolve(graphics, meshTag, vbuf));

		psBindables.push_back(Bind::IndexBuffer::Resolve(graphics, meshTag, indices));

		auto pvs = Bind::VertexShader::Resolve(graphics, "PhongVS.cso");
		auto pvsbc = pvs->GetByteCode();
		psBindables.push_back(std::move(pvs));

		psBindables.push_back(Bind::PixelShader::Resolve(graphics, "PhongPS.cso"));

		psBindables.push_back(Bind::InputLayout::Resolve(graphics, vbuf.GetLayout(), pvsbc));

		DRR::IncompleteLayout lay;
		lay.Add<DRR::Float>( "specularIntensity" );
		lay.Add<DRR::Float>( "specularPower" );
		lay.Add<DRR::Float>( "specularMapWeight" );

		auto buf = DRR::Buffer( std::move( lay ) );
		buf["specularIntensity"] = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
		buf["specularPower"] = shininess;
		buf["specularMapWeight"] = 1.0f;

		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		psBindables.push_back(std::make_unique<Bind::DynamicCachingPixelConstantBuffer>( graphics,buf,1u ));
	}
	else if(!hasDiffuseMap && !hasNormalMap && !hasSpecularMap)
	{
		CENgineexp::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)
		));

		for(unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i])
			);
		}

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

		psBindables.push_back(Bind::VertexBuffer::Resolve(graphics, meshTag, vbuf));

		psBindables.push_back(Bind::IndexBuffer::Resolve(graphics, meshTag, indices));

		auto pvs = Bind::VertexShader::Resolve(graphics, "PhongVSNotex.cso");
		auto pvsbc = pvs->GetByteCode();
		psBindables.push_back(std::move(pvs));

		psBindables.push_back(Bind::PixelShader::Resolve(graphics, "PhongPSNotex.cso"));

		psBindables.push_back(Bind::InputLayout::Resolve(graphics, vbuf.GetLayout(), pvsbc));

		DRR::IncompleteLayout lay;
		lay.Add<DRR::Float4>( "materialColor" );
		lay.Add<DRR::Float4>( "specularColor" );
		lay.Add<DRR::Float>( "specularPower" );

		auto buf = DRR::Buffer( std::move( lay ) );
		buf["specularPower"] = shininess;
		buf["specularColor"] = specularColor;
		buf["materialColor"] = diffuseColor;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		psBindables.push_back(std::make_unique<Bind::DynamicCachingPixelConstantBuffer>( graphics,buf,1u ));
	}
	else
	{
		throw std::runtime_error("terrible combination of textures in material");
	}

	// Anything with alpha diffuse is two-sided IN SPONZA, need a better way
	// of signalling two-sidedness to be more general in the future
	psBindables.push_back(Bind::Rasterizer::Resolve(graphics, hasAlphaDiffuse));
	psBindables.push_back(Bind::Blender::Resolve(graphics, false));

	return std::make_unique<Mesh>(graphics, std::move(psBindables));
}

std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node) noexcept
{
	const auto transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
	));

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
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
	}

	return pNode;
}