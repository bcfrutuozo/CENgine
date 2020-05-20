#pragma once

#include "IndexedTriangleList.h"
#include "Math.h"

#include <vector>
#include <array>

class Plane
{
public:
	template<class V>
	static IndexedTriangleList<V> MakeTesselated(const int divisionsX, const int divisionsY)
	{
		assert(divisionsX >= 1);
		assert(divisionsY >= 1);

		constexpr float width = 2.0f;
		constexpr float height = 2.0f;

		const int nVerticesX = divisionsX + 1;
		const int nVerticesY = divisionsY + 1;

		std::vector<V> vertices (nVerticesX * nVerticesY);

		{
			const float sideX = width / 2.0f;
			const float sideY = height / 2.0f;
			const float divisionSizeX = width / float(divisionsX);
			const float divisionSizeY = height / float(divisionsY);

			const auto bottomLeft = DirectX::XMVectorSet(-sideX, -sideY, 0.0f, 0.0f);

			for(int y = 0, i =0; y <nVerticesY; y++)
			{
				const float yPos = float(y) * divisionSizeY;
				for(int x = 0; x < nVerticesX; x++, i++)
				{
					const auto vector = DirectX::XMVectorAdd(bottomLeft, DirectX::XMVectorSet(float(x) * divisionSizeX, yPos, 0.0f, 0.0f));
					DirectX::XMStoreFloat3(&vertices[i].pos, vector);
				}
			}
			
		}

		std::vector<unsigned short> indices;
		indices.reserve(Square(divisionsX * divisionsY) * 6);

		{
			const auto vxy2i = [nVerticesX](const size_t x, const size_t y)
			{
				return static_cast<unsigned short>(y * nVerticesX + x);
			};

			for(size_t y = 0; y < divisionsY; y++)
			{
				for(size_t x = 0; x < divisionsX; x++)
				{
					const std::array<unsigned short, 4> indexArray =
					{ vxy2i(x, y), vxy2i(x + 1, y), vxy2i(x, y + 1), vxy2i(x + 1, y + 1) };
					indices.push_back(indexArray[0]);
					indices.push_back(indexArray[2]);
					indices.push_back(indexArray[1]);
					indices.push_back(indexArray[1]);
					indices.push_back(indexArray[2]);
					indices.push_back(indexArray[3]);
				}
			}
		}

		return { std::move(vertices), std::move(indices) };
	}

	template<class V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselated<V>(1, 1);
	}
};