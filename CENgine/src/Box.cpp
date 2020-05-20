#include "Box.h"
#include "BindableBase.h"
#include "Cube.h"
#include "imgui/imgui.h"

Box::Box(Graphics& graphics,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& aDist,
	std::uniform_real_distribution<float>& dDist,
	std::uniform_real_distribution<float>& oDist,
	std::uniform_real_distribution<float>& rDist,
	std::uniform_real_distribution<float>& bDist,
	DirectX::XMFLOAT3 material)
	:
	Object(graphics, rng, aDist, dDist, oDist, rDist)
{
	if (!IsStaticInitialized()) 
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 n;
		};

		auto model = Cube::MakeIndependent<Vertex>();
		model.SetNormalsIndependentFlat();
		
		AddStaticBind(std::make_unique<VertexBuffer>(graphics, model.vertices));

		auto pVertexShader = std::make_unique<VertexShader>(graphics, L"PhongVS.cso");
		auto pVertexShaderByteCode = pVertexShader->GetByteCode();
		AddStaticBind(std::move(pVertexShader));

		AddStaticBind(std::make_unique<PixelShader>(graphics, L"PhongPS.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(graphics, model.indices));
		
		const std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescriptor =
		{
			{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}		
		};

		AddStaticBind(std::make_unique<InputLayout>(graphics, inputElementDescriptor, pVertexShaderByteCode));
		AddStaticBind(std::make_unique<Topology>(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCbuf>(graphics, *this));

	materialConstants.color = material;
	AddBind(std::make_unique<MaterialCbuf>(graphics, materialConstants, 1u));
	
	// Model deformation transform (per instance, not stored as bind)
	DirectX::XMStoreFloat3x3(&mt, DirectX::XMMatrixScaling(1.0f, 1.0f, bDist(rng)));
}

bool Box::SpawnControlWindow(int id, Graphics& graphics) noexcept
{
	using namespace std::string_literals;
	
	bool dirty = false;
	bool open = true;
	
	if(ImGui::Begin(("Box "s + std::to_string(id)).c_str(), &open))
	{
		ImGui::Text( "Material Properties" );
		const auto cd = ImGui::ColorEdit3( "Material Color",&materialConstants.color.x );
		const auto sid = ImGui::SliderFloat( "Specular Intensity",&materialConstants.specularIntensity,0.05f,4.0f,"%.2f",2 );
		const auto spd = ImGui::SliderFloat( "Specular Power",&materialConstants.specularPower,1.0f,200.0f,"%.2f",2 );
		dirty = cd || sid || spd;


		ImGui::Text( "Position" );
		ImGui::SliderFloat( "R",&r,0.0f,80.0f,"%.1f" );
		ImGui::SliderAngle( "Theta",&theta,-180.0f,180.0f );
		ImGui::SliderAngle( "Phi",&phi,-180.0f,180.0f );
		ImGui::Text( "Orientation" );
		ImGui::SliderAngle( "Roll",&roll,-180.0f,180.0f );
		ImGui::SliderAngle( "Pitch",&pitch,-180.0f,180.0f );
		ImGui::SliderAngle( "Yaw",&yaw,-180.0f,180.0f );
	}
	ImGui::End();

	if(dirty)
	{
		SyncMaterial(graphics);
	}

	return open;
}

void Box::SyncMaterial(Graphics& graphics) noexcept(!IS_DEBUG)
{
	auto pConstPS = QueryBindable<MaterialCbuf>();
	assert(pConstPS != nullptr);
	pConstPS->Update(graphics, materialConstants);
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat3x3(&mt) * Object::GetTransformXM();
}