#include "Projection.h"
#include "imgui/imgui.h"
#include "Graphics.h"

Projection::Projection(Graphics& graphics, float width, float height, float nearZ, float farZ)
	:
	width(width),
	height(height),
	nearZ(nearZ),
	farZ(farZ),
	frustum(graphics, width, height, nearZ, farZ),
	homeWidth(width),
	homeHeight(height),
	homeNearZ(nearZ),
	homeFarZ(farZ)
{ }

DirectX::XMMATRIX Projection::GetMatrix() const
{
	return DirectX::XMMatrixPerspectiveLH(width, height, nearZ, farZ);
}

void Projection::RenderWidgets(Graphics& graphics)
{
	bool dirty = false;
	const auto dcheck = [&dirty](bool d) { dirty = dirty || d; };

	ImGui::Text("Projection");
	dcheck(ImGui::SliderFloat("Width", &width, 0.01f, 4.0f, "%.2f", 1.5f));
	dcheck(ImGui::SliderFloat("Height", &height, 0.01f, 4.0f, "%.2f", 1.5f));
	dcheck(ImGui::SliderFloat("Near Z", &nearZ, 0.01f, farZ - 0.01f, "%.2f", 4.0f));
	dcheck(ImGui::SliderFloat("Far Z", &farZ, nearZ + 0.01f, 400.0f, "%.2f", 4.0f));

	if(dirty)
	{
		frustum.SetVertices(graphics, width, height, nearZ, farZ);
	}
}

void Projection::SetPosition(DirectX::XMFLOAT3 position)
{
	frustum.SetPosition(position);
}

void Projection::SetRotation(DirectX::XMFLOAT3 rotation)
{
	frustum.SetRotation(rotation);
}

void Projection::Submit(size_t channel) const
{
	frustum.Submit(channel);
}

void Projection::LinkTechniques(RGP::RenderGraph& renderGraph)
{
	frustum.LinkTechniques(renderGraph);
}

void Projection::Reset(Graphics& graphics)
{
	width = homeWidth;
	height = homeHeight;
	nearZ = homeNearZ;
	farZ = homeFarZ;
	frustum.SetVertices(graphics, width, height, nearZ, farZ);
}