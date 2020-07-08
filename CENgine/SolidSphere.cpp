#include "SolidSphere.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"
#include "Sphere.h"
#include "Stencil.h"

SolidSphere::SolidSphere(Graphics& graphics, float radius)
{
	using namespace Bind;

	auto model = Sphere::Make();
	model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));

	const auto geometryTag = "$sphere." + std::to_string(radius);
	AddBind(VertexBuffer::Resolve(graphics, geometryTag, model.vertices));
	AddBind(IndexBuffer::Resolve(graphics, geometryTag, model.indices));

	auto pvs = VertexShader::Resolve(graphics, "SolidVS.cso");
	auto pvsbc = pvs->GetByteCode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(graphics, "SolidPS.cso"));

	struct PSColorConstant
	{
		DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f, };
		float padding;
	} colorConst;

	AddBind(PixelConstantBuffer<PSColorConstant>::Resolve(graphics, colorConst, 1u));

	AddBind(InputLayout::Resolve(graphics, model.vertices.GetLayout(), pvsbc));
	AddBind(Topology::Resolve(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbuf>(graphics, *this));
	AddBind(Blender::Resolve(graphics, false));
	AddBind(Rasterizer::Resolve(graphics, false));
	AddBind(std::make_shared<Stencil>(graphics, Stencil::Mode::Off));
}

void SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	position = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}
