#pragma once

#include "Graphics.h"

#include <string>
#include <memory>
#include <filesystem>

class Node;
class Mesh;
class FrameGenerator;
struct aiMesh;
struct aiMaterial;
struct aiNode;

class Model
{
public:

	Model(Graphics& graphics, const std::string& path, const float scale = 1.0f);
	void Submit(FrameGenerator& frame) const NOXND;
	void SetRootTransform(DirectX::FXMMATRIX transformMatrix) noexcept;
	void Accept(class ModelProbe& probe);
	~Model() noexcept;	
private:

	static std::unique_ptr<Mesh> ParseMesh(Graphics& graphics, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node, float scale) noexcept;

	std::unique_ptr<Node> pRoot;
	std::vector<std::unique_ptr<Mesh>> psMeshes;
};