#pragma once

#include "Graphics.h"

class Model;
class Mesh;
class TechniqueProbe;
class ModelProbe;

class Node
{
	friend Model;
public:

	Node(int id, const std::string& name, std::vector<Mesh*> psMeshes, const DirectX::XMMATRIX& transformMatrix) NOXND;
	void Submit(size_t channels, DirectX::FXMMATRIX accumulatedTransform) const NOXND;
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;
	int GetId() const noexcept;

	bool HasChildren() const noexcept
	{
		return psChildren.size() > 0;
	}
	void Accept(ModelProbe& probe);
	void Accept(TechniqueProbe& probe);
	const std::string& GetName() const
	{
		return name;
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