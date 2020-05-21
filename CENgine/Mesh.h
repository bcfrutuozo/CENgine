#pragma once

#include "DrawableBase.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include "Conditional_noexcept.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Mesh : public DrawableBase<Mesh>
{
public:

	Mesh(Graphics& graphics, std::vector<std::unique_ptr<Bind::Bindable>> psBinds);
	void Draw(Graphics& graphics, DirectX::FXMMATRIX acculumatedTransform) const NOXND;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:

	mutable DirectX::XMFLOAT4X4 transformMatrix;
};

class Node
{
	friend class Model;
public:

	Node(std::vector<Mesh*> psMeshes, const DirectX::XMMATRIX& transformMatrix) NOXND;
	void Draw(Graphics& graphics, DirectX::FXMMATRIX accumulatedTransform) const NOXND;
private:

	void AddChild(std::unique_ptr<Node> pChild) NOXND;

	std::vector<std::unique_ptr<Node>> psChildren;
	std::vector<Mesh*> psMeshes;
	DirectX::XMFLOAT4X4 transformMatrix;
};

class Model
{
public:

	Model(Graphics& graphics, const std::string filename);
	void Draw(Graphics& graphics, DirectX::FXMMATRIX transform) const;
	static std::unique_ptr<Mesh> ParseMesh(Graphics& graphics, const aiMesh& mesh);
	std::unique_ptr<Node> ParseNode(const aiNode& node);
private:

	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> psMeshes;
};