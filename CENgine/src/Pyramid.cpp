#include "Pyramid.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Cone.h"

#include <array>


Pyramid::Pyramid( Graphics& graphics,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& aDist,
	std::uniform_real_distribution<float>& dDist,
	std::uniform_real_distribution<float>& oDist,
	std::uniform_real_distribution<float>& rDist,
	std::uniform_int_distribution<int>& tDist)
	:
	Object(graphics, rng, aDist, dDist, oDist, rDist)
{
	if( !IsStaticInitialized() )
	{
		auto pvs = std::make_unique<VertexShader>( graphics,L"BlendedPhongVS.cso" );
		auto pvsbc = pvs->GetByteCode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<PixelShader>( graphics,L"BlendedPhongPS.cso" ) );

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{"Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		
		AddStaticBind( std::make_unique<InputLayout>( graphics,ied,pvsbc ) );

		AddStaticBind( std::make_unique<Topology>( graphics,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

		struct PSMaterialConstant
		{
			float specularIntensity = 0.6;
			float specularPower = 30.0f;
			float padding[2];
		} colorConst;
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(graphics, colorConst, 1u));
	}

	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 n;
		std::array<char, 4> color;
		char padding;
	};

	const auto tesselation = tDist( rng );
	auto model = Cone::MakeTesselatedIndependentFaces<Vertex>( tesselation );
	// Set vertex colors for mesh (tip red blending to blue base)
	for( auto& v : model.vertices )
	{
		v.color = { static_cast<char>(10),static_cast<char>(10),static_cast<char>(255) };
	}
	for( int i = 0; i < tesselation; i++ )
	{
		model.vertices[i * 3].color = { static_cast<char>(255),static_cast<char>(10),static_cast<char>(10) };
	}

	// Squash mesh a bit in the z direction
	model.Transform( DirectX::XMMatrixScaling( 1.0f,1.0f,0.7f ) );
	// Add normals
	model.SetNormalsIndependentFlat();

	AddBind( std::make_unique<VertexBuffer>( graphics,model.vertices ) );
	AddIndexBuffer( std::make_unique<IndexBuffer>( graphics,model.indices ) );

	AddBind( std::make_unique<TransformCbuf>( graphics,*this ) );
}