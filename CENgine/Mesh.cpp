#include "Mesh.h"
#include "imgui/imgui.h"
#include "Surface.h"
#include "Matrix.h"
#include "DynamicConstant.h"
#include "DynamicConstantBuffer.h"
#include "LayoutCodex.h"
#include "Stencil.h"

#include <sstream>
#include <unordered_map>
#include <filesystem>

Mesh::Mesh(Graphics& graphics, const Material& material, const aiMesh& mesh, float scale) NOXND
	:
	Drawable(graphics, material, mesh, scale)
{ }

void Mesh::Submit(FrameGenerator& frame, DirectX::FXMMATRIX accumulatedTransform) const NOXND
{
	DirectX::XMStoreFloat4x4(&transformMatrix, accumulatedTransform);
	Drawable::Submit(frame);
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transformMatrix);
}

	