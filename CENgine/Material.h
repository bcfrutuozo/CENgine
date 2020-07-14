#pragma once

#include "Graphics.h"
#include "Technique.h"
#include "Vertex.h"

#include <vector>
#include <filesystem>

struct aiMaterial;
struct aiMesh;

namespace Bind
{
	class VertexBuffer;
	class IndexBuffer;
}

class Material
{
public:

	Material(Graphics& graphics, const aiMaterial& pMaterial, const std::filesystem::path& modelPath) NOXND;
	CENgineexp::VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept;	
	std::vector<unsigned short> ExtractIndices(const aiMesh& mesh) const noexcept;
	std::shared_ptr<Bind::VertexBuffer> MakeVertexBindable(Graphics& graphics, const aiMesh& mesh, float scale = 1.0f) const NOXND;
	std::shared_ptr<Bind::IndexBuffer> MakeIndexBindable(Graphics& graphics, const aiMesh& mesh) const NOXND;
	std::vector<Technique> GetTechniques() const noexcept;
private:

	std::string MakeMeshTag(const aiMesh& mesh) const noexcept;

	CENgineexp::VertexLayout vtxLayout;
	std::vector<Technique> techniques;
	std::string modelPath;
	std::string name;
};