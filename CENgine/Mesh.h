#pragma once

#include "Graphics.h"
#include "Drawable.h"
#include "Conditional_noexcept.h"

class Material;
class FrameGenerator;
struct aiMesh;

class Mesh : public Drawable
{
public:

	Mesh(Graphics& graphics, const Material& material, const aiMesh& mesh, float scale = 1.0f) NOXND;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Submit(size_t channels, DirectX::FXMMATRIX acculatedTransform) const NOXND;
private:

	mutable DirectX::XMFLOAT4X4 transformMatrix;
};