#pragma once

#include "Vertex.h"

#include <vector>
#include <DirectXMath.h>

class IndexedTriangleList
{
public:

	IndexedTriangleList() = default;
	IndexedTriangleList(CENgineexp::VertexBuffer vertices_in, std::vector<unsigned short> indices_in)
		:
		vertices(std::move(vertices_in)),
		indices(std::move(indices_in))
	{
		assert(vertices.Size() > 2);
		assert(indices.size() % 3 == 0);
	}

	void Transform(DirectX::FXMMATRIX matrix)
	{
		using Elements = CENgineexp::VertexLayout::ElementType;

		for(int i = 0; i < vertices.Size(); i++)
		{
			auto& pos = vertices[i].Attr<Elements::Position3D>();
			DirectX::XMStoreFloat3(
				&pos,
				DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pos), matrix));
		}
	}

	CENgineexp::VertexBuffer vertices;
	std::vector<unsigned short> indices;
};