#include "SolidSphere.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"
#include "Sphere.h"
#include "Stencil.h"
#include "Channels.h"

SolidSphere::SolidSphere(Graphics& graphics, float radius)
{
	auto model = Sphere::Make();
	model.Transform(DirectX::XMMatrixScaling(radius, radius, radius));

	const auto geometryTag = "$sphere." + std::to_string(radius);
	pVertices = Bind::VertexBuffer::Resolve(graphics, geometryTag, model.vertices);
	pIndices = Bind::IndexBuffer::Resolve(graphics, geometryTag, model.indices);
	pTopology = Bind::Topology::Resolve(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique solid{ Channel::main };
		Step only("lambertian");
		auto pvs = Bind::VertexShader::Resolve(graphics, "Solid_VS.cso");
		only.AddBindable(Bind::InputLayout::Resolve(graphics, model.vertices.GetLayout(), *pvs));
		only.AddBindable(std::move(pvs));

		only.AddBindable(Bind::PixelShader::Resolve(graphics, "Solid_PS.cso"));

		struct PSColorConstant
		{
			DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f, };
			float padding;
		} colorConst;

		only.AddBindable(Bind::PixelConstantBuffer<PSColorConstant>::Resolve(graphics, colorConst, 1u));
		only.AddBindable(std::make_shared<Bind::TransformCbuf>(graphics));
		only.AddBindable(Bind::Rasterizer::Resolve(graphics, false));
		solid.AddStep(std::move(only));
		AddTechnique(std::move(solid));
	}
}

void SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	position = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}
