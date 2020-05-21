#include "SolidSphere.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Sphere.h"

SolidSphere::SolidSphere(Graphics& graphics, float radius)
{
	using namespace Bind;
	
	if(!IsStaticInitialized())
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
		};

		auto model = Sphere::Make<Vertex>();
		model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));
		AddBind(std::make_unique<VertexBuffer>(graphics, model.vertices));
		AddIndexBuffer(std::make_unique<IndexBuffer>(graphics, model.indices));

		auto pvs = std::make_unique<VertexShader>(graphics, L"SolidVS.cso");
		auto pvsbc = pvs->GetByteCode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(graphics, L"SolidPS.cso"));

		struct PSColorConstant
		{
			DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f,};
			float padding;
		} colorConst;
		
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSColorConstant>>(graphics, colorConst));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		AddStaticBind(std::make_unique<InputLayout>(graphics, ied, pvsbc));
		AddStaticBind(std::make_unique<Topology>(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCbuf>(graphics, *this));
}

void SolidSphere::Update(float dt) noexcept
{
	
}

void SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	position = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}
