#include "SkinnedBox.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Cube.h"
#include "Surface.h"
#include "Texture.h"
#include "Sampler.h"


SkinnedBox::SkinnedBox( Graphics& graphics,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& aDist,
	std::uniform_real_distribution<float>& dDist,
	std::uniform_real_distribution<float>& oDist,
	std::uniform_real_distribution<float>& rDist )
	:
	Object( graphics,rng,aDist,dDist,oDist,rDist )
{
	if( !IsStaticInitialized() )
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 n;
			DirectX::XMFLOAT2 tc;
		};
		auto model = Cube::MakeIndependentTextured<Vertex>();
		model.SetNormalsIndependentFlat();

		AddStaticBind( std::make_unique<VertexBuffer>( graphics,model.vertices ) );

		AddStaticBind( std::make_unique<Texture>( graphics,Surface::FromFile( "Images\\teste.png" ) ) );

		AddStaticBind( std::make_unique<Sampler>( graphics ) );

		auto pvs = std::make_unique<VertexShader>( graphics,L"TexturedPhongVS.cso" );
		auto pvsbc = pvs->GetByteCode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<PixelShader>( graphics,L"TexturedPhongPS.cso" ) );

		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( graphics,model.indices ) );

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,24,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind( std::make_unique<InputLayout>( graphics,ied,pvsbc ) );

		AddStaticBind( std::make_unique<Topology>( graphics,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

		struct PSMaterialConstant
		{
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[2];
		} colorConst;
		AddStaticBind( std::make_unique<PixelConstantBuffer<PSMaterialConstant>>( graphics,colorConst,1u ) );
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind( std::make_unique<TransformCbuf>( graphics,*this ) );
}