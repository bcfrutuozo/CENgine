#pragma once

#include "Drawable.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include "Conditional_noexcept.h"
#include "imgui/imgui.h"
#include "DynamicConstant.h"
#include "ConstantBuffer.h"

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

	Node(int id, const std::string& name, std::vector<Mesh*> psMeshes, const DirectX::XMMATRIX& transformMatrix) NOXND;
	void Draw(Graphics& graphics, DirectX::FXMMATRIX accumulatedTransform) const NOXND;
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	void ShowTree(Node*& pSelectedNode) const noexcept;
	const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;
	int GetId() const noexcept;
	const DRR::Buffer* GetMaterialConstants() const NOXND;
	void SetMaterialConstants(const DRR::Buffer&) NOXND;

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