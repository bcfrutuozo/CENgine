#pragma once

#include "Graphics.h"
#include "Conditional_noexcept.h"
#include "Technique.h"

#include <DirectXMath.h>
#include <memory>

class TechniqueProbe;
class Material;
class RenderGraph;
struct aiMesh;

namespace Bind
{
	class IndexBuffer;
	class VertexBuffer;
	class Topology;
	class InputLayout;
}

class Drawable
{

public:
	Drawable() = default;
	Drawable(Graphics& graphics, const Material& mat, const aiMesh& mesh, float scale = 1.0f) noexcept;
	Drawable(const Drawable&) = delete;
	void AddTechnique(Technique tech_in) noexcept;
 	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Submit() const noexcept;
	void Bind( Graphics& graphics ) const noexcept;
	void Accept(TechniqueProbe& probe);
	UINT GetIndexCount() const NOXND;
	void LinkTechniques(RenderGraph& renderGraph);
	virtual ~Drawable();

protected:

	std::shared_ptr<Bind::IndexBuffer> pIndices;
	std::shared_ptr<Bind::VertexBuffer> pVertices;
	std::shared_ptr<Bind::Topology> pTopology;
	std::vector<Technique> techniques;
};

