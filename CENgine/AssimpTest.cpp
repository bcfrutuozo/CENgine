#include "AssimpTest.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

AssimpTest::AssimpTest(Graphics& graphics,
		std::mt19937& rng,
		std::uniform_real_distribution<float>& aDist,
		std::uniform_real_distribution<float>& dDist,
		std::uniform_real_distribution<float>& oDist,
		std::uniform_real_distribution<float>& rDist,
		DirectX::XMFLOAT3 material,
		float scale)
	:
	Object( graphics,rng,aDist,dDist,oDist,rDist )
{
	if( !IsStaticInitialized() )
	{
		using CENgineexp::VertexLayout;
		
		CENgineexp::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(VertexLayout::Position3D)
			.Append(VertexLayout::Normal)));

		Assimp::Importer imp;
		const auto pModel = imp.ReadFile( "Models\\suzanne.obj",
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices
		);
		const auto pMesh = pModel->mMeshes[0];

		for( unsigned int i = 0; i < pMesh->mNumVertices; i++ )
		{
			vbuf.EmplaceBack(
				DirectX::XMFLOAT3{ pMesh->mVertices[i].x * scale,pMesh->mVertices[i].y * scale,pMesh->mVertices[i].z * scale },
				*reinterpret_cast<DirectX::XMFLOAT3*>(&pMesh->mNormals[i]));
		}

		std::vector<unsigned short> indices;
		indices.reserve( pMesh->mNumFaces * 3 );
		for( unsigned int i = 0; i < pMesh->mNumFaces; i++ )
		{
			const auto& face = pMesh->mFaces[i];
			assert( face.mNumIndices == 3 );
			indices.push_back( face.mIndices[0] );
			indices.push_back( face.mIndices[1] );
			indices.push_back( face.mIndices[2] );
		}

		AddStaticBind( std::make_unique<VertexBuffer>( graphics,vbuf ) );

		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( graphics,indices ) );

		auto pvs = std::make_unique<VertexShader>( graphics,L"PhongVS.cso" );
		auto pvsbc = pvs->GetByteCode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<PixelShader>( graphics,L"PhongPS.cso" ) );

		AddStaticBind( std::make_unique<InputLayout>( graphics,vbuf.GetLayout().GetD3DLayout(),pvsbc ) );

		AddStaticBind( std::make_unique<Topology>( graphics,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

		struct PSMaterialConstant
		{
			DirectX::XMFLOAT3 color;
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[3];
		} pmc;
		pmc.color = material;
		AddStaticBind( std::make_unique<PixelConstantBuffer<PSMaterialConstant>>( graphics,pmc,1u ) );
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind( std::make_unique<TransformCbuf>( graphics,*this ) );
}