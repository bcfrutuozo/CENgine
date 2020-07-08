#pragma once

#include "Drawable.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include "Conditional_noexcept.h"
#include "imgui/imgui.h"

#include <optional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <type_traits>
#include <filesystem>

class ModelException : public CENgineException
{
public:

	ModelException(int line, const char* file, std::string note) noexcept;
	const char* what() const noexcept override;
	const char* GetType() const noexcept override;
	const std::string& GetNote() const noexcept;
private:

	std::string note;
};

class Mesh : public Drawable
{
public:

	Mesh(Graphics& graphics, std::vector<std::shared_ptr<Bind::Bindable>> psBinds);
	void Draw(Graphics& graphics, DirectX::FXMMATRIX acculumatedTransform) const NOXND;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:

	mutable DirectX::XMFLOAT4X4 transformMatrix;
};

class Node
{
	friend class Model;
public:

	struct PSMaterialConstantFullmonte
	{
		BOOL  normalMapEnabled = TRUE;
		BOOL  specularMapEnabled = TRUE;
		BOOL  hasGlossMap = FALSE;
		float specularPower = 3.1f;
		DirectX::XMFLOAT3 specularColor = { 0.75f, 0.75f, 0.75f };
		float specularMapWeight = 0.671f;
	};

	struct PSMaterialConstantNotex
	{
		DirectX::XMFLOAT4 materialColor = { 0.447970f,0.327254f,0.176283f,1.0f };
		DirectX::XMFLOAT4 specularColor = { 0.65f,0.65f,0.65f,1.0f };
		float specularPower = 120.0f;
		float padding[3];
	};

	Node(int id, const std::string& name, std::vector<Mesh*> psMeshes, const DirectX::XMMATRIX& transformMatrix) NOXND;
	void Draw(Graphics& graphics, DirectX::FXMMATRIX accumulatedTransform) const NOXND;
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	void ShowTree(Node*& pSelectedNode) const noexcept;
	const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;
	int GetId() const noexcept;

	template<class T>
	bool Control(Graphics& graphics, T& c)
	{
		if (psMeshes.empty())
		{
			return false;
		}

		if constexpr (std::is_same<T, PSMaterialConstantFullmonte>::value)
		{
			if (auto pcb = psMeshes.front()->QueryBindable<Bind::PixelConstantBuffer<T>>())
			{
				ImGui::Text("Material");

				bool normalMapEnabled = (bool)c.normalMapEnabled;
				ImGui::Checkbox("Norm Map", &normalMapEnabled);
				c.normalMapEnabled = normalMapEnabled ? TRUE : FALSE;

				bool specularMapEnabled = (bool)c.specularMapEnabled;
				ImGui::Checkbox("Spec Map", &specularMapEnabled);
				c.specularMapEnabled = specularMapEnabled ? TRUE : FALSE;

				bool hasGlossMap = (bool)c.hasGlossMap;
				ImGui::Checkbox("Gloss Alpha", &hasGlossMap);
				c.hasGlossMap = hasGlossMap ? TRUE : FALSE;

				ImGui::SliderFloat("Spec Weight", &c.specularMapWeight, 0.0f, 2.0f);

				ImGui::SliderFloat("Spec Pow", &c.specularPower, 0.0f, 1000.0f, "%f", 5.0f);

				ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&c.specularColor));

				pcb->Update(graphics, c);
				return true;
			}
		}
		else if constexpr (std::is_same<T, PSMaterialConstantNotex>::value)
		{
			if (auto pcb = psMeshes.front()->QueryBindable<Bind::PixelConstantBuffer<T>>())
			{
				ImGui::Text("Material");

				ImGui::ColorPicker3( "Spec Color",reinterpret_cast<float*>(&c.specularColor) );

				ImGui::SliderFloat("Spec Pow", &c.specularPower, 0.0f, 1000.0f, "%f", 5.0f);

				ImGui::ColorPicker3("Diff Color", reinterpret_cast<float*>(&c.materialColor));

				pcb->Update(graphics, c);
				return true;
			}
		}
		return false;
	}

private:

	void AddChild(std::unique_ptr<Node> pChild) NOXND;

	int id;
	std::string name;
	std::vector<std::unique_ptr<Node>> psChildren;
	std::vector<Mesh*> psMeshes;
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMFLOAT4X4 appliedTransform;
};

class Model
{
public:

	Model(Graphics& graphics, const std::string& path, const float scale = 1.0f);
	~Model() noexcept;
	void Draw(Graphics& graphics) const NOXND;
	void ShowWindow(Graphics& graphics, const char* windowName = nullptr) noexcept;
	void SetRootTransform(DirectX::FXMMATRIX transformMatrix) noexcept;
private:

	static std::unique_ptr<Mesh> ParseMesh(Graphics& graphics, const aiMesh& mesh, const aiMaterial* const* psMaterials, const std::filesystem::path& path, const float scale);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node) noexcept;

	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> psMeshes;

	std::unique_ptr<class ModelWindow> pWindow;
};