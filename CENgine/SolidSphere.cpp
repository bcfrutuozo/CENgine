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
	pVertices = VertexBuffer::Resolve(graphics, geometryTag, model.vertices);
	pIndices = IndexBuffer::Resolve(graphics, geometryTag, model.indices);
	pTopology = Topology::Resolve(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique solid;
		Step only("lambertian");
		auto pvs = VertexShader::Resolve(graphics, "Solid_VS.cso");
		auto pvsbc = pvs->GetByteCode();
		only.AddBindable(std::move(pvs));

		only.AddBindable(PixelShader::Resolve(graphics, "Solid_PS.cso"));

		struct PSColorConstant
		{
			DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f, };
			float padding;
		} colorConst;

		only.AddBindable(PixelConstantBuffer<PSColorConstant>::Resolve(graphics, colorConst, 1u));
		only.AddBindable(InputLayout::Resolve(graphics, model.vertices.GetLayout(), pvsbc));
		only.AddBindable(std::make_shared<TransformCbuf>(graphics));
		only.AddBindable(Blender::Resolve(graphics, false));
		only.AddBindable(Rasterizer::Resolve(graphics, false));
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
