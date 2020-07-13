#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "BindableCommon.h"
#include "BindableCodex.h"
#include "Material.h"

#include <assimp/scene.h>

Drawable::Drawable(Graphics& graphics, const Material& mat, const aiMesh& mesh, float scale) noexcept
{
	pVertices = mat.MakeVertexBindable(graphics, mesh, scale);
	pIndices = mat.MakeIndexBindable(graphics, mesh);
	pTopology = Bind::Topology::Resolve(graphics);

	for(auto& t : mat.GetTechniques())
	{
		AddTechnique(std::move(t));
	}
}

void Drawable::AddTechnique(Technique tech_in) noexcept
{
	tech_in.InitializeParentReferences( *this );
	techniques.push_back( std::move( tech_in ) );
}

void Drawable::Submit() const noexcept
{
	for(const auto& tech : techniques)
	{
		tech.Submit(*this);
	}
}

void Drawable::Bind(Graphics& graphics) const noexcept
{
	pTopology->Bind(graphics);
	pIndices->Bind(graphics);
	pVertices->Bind(graphics);
}

void Drawable::Accept(TechniqueProbe& probe)
{
	for(auto& t : techniques)
	{
		t.Accept(probe);
	}
}

UINT Drawable::GetIndexCount() const NOXND
{
	return pIndices->GetCount();
}

void Drawable::LinkTechniques(RenderGraph& renderGraph)
{
	for(auto& tech : techniques)
	{
		tech.Link(renderGraph);
	}
}

Drawable::~Drawable()
{ }