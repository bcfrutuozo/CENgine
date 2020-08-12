#include "CameraIndicator.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"
#include "Sphere.h"
#include "Stencil.h"
#include "Channels.h"

CameraIndicator::CameraIndicator(Graphics& graphics)
{
	using namespace Bind;

	const auto geometryTag = "$cam";
	CENgineexp::VertexLayout layout;
	layout.Append(CENgineexp::VertexLayout::ElementType::Position3D);
	CENgineexp::VertexBuffer vertices{ std::move(layout) };
	{
		const float x = 4.0f / 3.0f * 0.75f;
		const float y = 1.0f * 0.75f;
		const float z = -2.0f;
		const float thalf = x * 0.5f;
		const float tspace = y * 0.2f;

		vertices.EmplaceBack(DirectX::XMFLOAT3{ -x,y,0.0f });
		vertices.EmplaceBack(DirectX::XMFLOAT3{ x,y,0.0f });
		vertices.EmplaceBack(DirectX::XMFLOAT3{ x,-y,0.0f });
		vertices.EmplaceBack(DirectX::XMFLOAT3{ -x,-y,0.0f });
		vertices.EmplaceBack(DirectX::XMFLOAT3{ 0.0f,0.0f,z });
		vertices.EmplaceBack(DirectX::XMFLOAT3{ -thalf,y + tspace,0.0f });
		vertices.EmplaceBack(DirectX::XMFLOAT3{ thalf,y + tspace,0.0f });
		vertices.EmplaceBack(DirectX::XMFLOAT3{ 0.0f,y + tspace + thalf,0.0f });
	}
	std::vector<unsigned short> indices;
	{
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(2);
		indices.push_back(3);
		indices.push_back(3);
		indices.push_back(0);
		indices.push_back(0);
		indices.push_back(4);
		indices.push_back(1);
		indices.push_back(4);
		indices.push_back(2);
		indices.push_back(4);
		indices.push_back(3);
		indices.push_back(4);
		indices.push_back(5);
		indices.push_back(6);
		indices.push_back(6);
		indices.push_back(7);
		indices.push_back(7);
		indices.push_back(5);
	}

	pVertices = VertexBuffer::Resolve(graphics, geometryTag, vertices);
	pIndices = IndexBuffer::Resolve(graphics, geometryTag, indices);
	pTopology = Topology::Resolve(graphics, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	{
		Technique line{ Channel::main };
		Step only("lambertian");

		auto pvs = VertexShader::Resolve(graphics, "Solid_VS.cso");
		only.AddBindable(InputLayout::Resolve(graphics, vertices.GetLayout(), *pvs));
		only.AddBindable(std::move(pvs));
		only.AddBindable(PixelShader::Resolve(graphics, "Solid_PS.cso"));

		struct PSColorConstant
		{
			DirectX::XMFLOAT3 color = { 0.2f,0.2f,0.6f };
			float padding = 0.0f;
		} colorConst;

		only.AddBindable(PixelConstantBuffer<PSColorConstant>::Resolve(graphics, colorConst, 1u));
		only.AddBindable(std::make_shared<TransformCbuf>(graphics));
		only.AddBindable(Rasterizer::Resolve(graphics, false));

		line.AddStep(std::move(only));
		AddTechnique(std::move(line));
	}
}

void CameraIndicator::SetPosition(DirectX::XMFLOAT3 position_in) noexcept
{
	position = position_in;
}

void CameraIndicator::SetRotation(DirectX::XMFLOAT3 rotation_in) noexcept
{
	rotation = rotation_in;
}

DirectX::XMMATRIX CameraIndicator::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&rotation)) *
		DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&position));
}