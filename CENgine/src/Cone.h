#pragma once

#include "IndexedTriangleList.h"
#include "Math.h"

#include <DirectXMath.h>

class Cone
{
public:
	template<class V>
	static IndexedTriangleList<V> MakeTesselated(int longDiv)
	{
		assert(longDiv >= 3);

		const auto base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const float longitudeAngle = 2.0f * PI / longDiv;

		// Base vertices
		std::vector<V> vertices;
		for(int iLong = 0; iLong < longDiv; iLong++)
		{
			vertices.emplace_back();
			auto v = DirectX::XMVector3Transform(
				base,
				DirectX::XMMatrixRotationZ(longitudeAngle * iLong));
			DirectX::XMStoreFloat3(&vertices.back().pos, v);
		}

		// Center
		vertices.emplace_back();
		vertices.back().pos = { 0.0f, 0.0f, -1.0f };
		const auto iCenter = static_cast<unsigned short>(vertices.size() - 1);

		// Tip
		vertices.emplace_back();
		vertices.back().pos = { 0.0f, 0.0f, 1.0f };
		const auto iTip = static_cast<unsigned short>(vertices.size() - 1);

		// Base indices
		std::vector<unsigned short> indices;
		for(unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			indices.push_back(iCenter);
			indices.push_back((iLong + 1) % longDiv);
			indices.push_back(iLong);
		}

		// Cone indices
		for(unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			indices.push_back(iLong);
			indices.push_back((iLong + 1) % longDiv);
			indices.push_back(iTip);
		}

		return { std::move(vertices), std::move(indices) };
	}
	
	template<class V>
	static IndexedTriangleList<V> MakeTesselatedIndependentFaces( int longDiv )
	{
		assert( longDiv >= 3 );

		const auto base = DirectX::XMVectorSet( 1.0f,0.0f,-1.0f,0.0f );
		const float longitudeAngle = 2.0f * PI / longDiv;

		std::vector<V> vertices;

		// Cone vertices
		const auto iCone = static_cast<unsigned short>(vertices.size());
		for( int iLong = 0; iLong < longDiv; iLong++ )
		{
			const float thetas[] = {
				longitudeAngle * iLong,
				longitudeAngle * (((iLong + 1) == longDiv) ? 0 : (iLong + 1))
			};
			vertices.emplace_back();
			vertices.back().pos = { 0.0f,0.0f,1.0f };
			for( auto theta : thetas )
			{
				vertices.emplace_back();
				const auto v = DirectX::XMVector3Transform(
					base,
					DirectX::XMMatrixRotationZ( theta )
				);
				DirectX::XMStoreFloat3( &vertices.back().pos,v );
			}
		}
		
		// Base vertices
		const auto iBaseCenter = static_cast<unsigned short>(vertices.size());
		vertices.emplace_back();
		vertices.back().pos = { 0.0f,0.0f,-1.0f };
		const auto iBaseEdge = static_cast<unsigned short>(vertices.size());
		for( int iLong = 0; iLong < longDiv; iLong++ )
		{
			vertices.emplace_back();
			auto v = DirectX::XMVector3Transform( 
				base,
				DirectX::XMMatrixRotationZ( longitudeAngle * iLong )
			);
			DirectX::XMStoreFloat3( &vertices.back().pos,v );
		}

		std::vector<unsigned short> indices;

		// Cone indices
		for( unsigned short i = 0; i < longDiv * 3; i++ )
		{
			indices.push_back( i + iCone );
		}
		
		// Base indices
		for( unsigned short iLong = 0; iLong < longDiv; iLong++ )
		{
			indices.push_back( iBaseCenter );
			indices.push_back( (iLong + 1) % longDiv + iBaseEdge );
			indices.push_back( iLong + iBaseEdge );
		}

		return { std::move( vertices ),std::move( indices ) };
	}

	template<class V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselated<V>(24);
	}
};