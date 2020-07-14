#include "Node.h"
#include "Mesh.h"
#include "ModelProbe.h"
#include "imgui/imgui.h"

Node::Node(int id, const std::string& name, std::vector<Mesh*> psMeshes, const DirectX::XMMATRIX& transformMatrix) NOXND
	:
id(id),
psMeshes(std::move(psMeshes)),
name(name)
{
	DirectX::XMStoreFloat4x4(&transform, transformMatrix);
	DirectX::XMStoreFloat4x4(&appliedTransform, DirectX::XMMatrixIdentity());
}

void Node::Submit(size_t channels, DirectX::FXMMATRIX accumulatedTransform) const NOXND
{
	const auto built = DirectX::XMLoadFloat4x4(&appliedTransform) *
		DirectX::XMLoadFloat4x4(&transform) *
		accumulatedTransform;

	for(const auto pMesh : psMeshes)
	{
		pMesh->Submit(channels, built);
	}
	for(const auto& pChild : psChildren)
	{
		pChild->Submit(channels, built);
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

const DirectX::XMFLOAT4X4& Node::GetAppliedTransform() const noexcept
{
	return appliedTransform;
}

int Node::GetId() const noexcept
{
	return id;
}

void Node::Accept(class ModelProbe& probe)
{
	if(probe.PushNode(*this))
	{
		for(auto& cp : psChildren)
		{
			cp->Accept(probe);
		}
		probe.PopNode(*this);
	}
}

void Node::Accept(class TechniqueProbe& probe)
{
	for(auto& mp : psMeshes)
	{
		mp->Accept(probe);
	}
}