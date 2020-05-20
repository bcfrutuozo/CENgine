#include "Cylinder.h"
#include "Prism.h"
#include "BindableBase.h"

Cylinder::Cylinder(Graphics& graphics,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& aDist,
	std::uniform_real_distribution<float>& dDist,
	std::uniform_real_distribution<float>& oDist,
	std::uniform_real_distribution<float>& rDist,
	std::uniform_real_distribution<float>& bDist,
	std::uniform_int_distribution<int>& tDist)
	:
	Object(graphics, rng, aDist, dDist, oDist, rDist)
{
	if (!IsStaticInitialized())
	{
		auto pvs = std::make_unique<VertexShader>(graphics, L"PhongVS.cso");
		auto pvsbc = pvs->GetByteCode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(graphics, L"IndexedPhongPS.cso"));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(graphics, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct PSMaterialConstant
		{
			DirectX::XMFLOAT3A colors[6] = {
				{1.0f,0.0f,0.0f},
				{0.0f,1.0f,0.0f},
				{0.0f,0.0f,1.0f},
				{1.0f,1.0f,0.0f},
				{1.0f,0.0f,1.0f},
				{0.0f,1.0f,1.0f},
			};
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
		} matConst;
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(graphics, matConst, 1u));
	}
	
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 n;
	};

	const auto model = Prism::MakeTesselatedIndependentCapNormals<Vertex>(tDist(rng));
	AddBind(std::make_unique<VertexBuffer>(graphics, model.vertices));
	AddIndexBuffer(std::make_unique<IndexBuffer>(graphics, model.indices));

	AddBind(std::make_unique<TransformCbuf>(graphics, *this));
}