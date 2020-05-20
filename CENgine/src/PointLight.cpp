#include "PointLight.h"
#include "imgui/imgui.h"

PointLight::PointLight(Graphics& graphics, float radius)
	:
	mesh(graphics, radius),
	constantBuffer(graphics)
{
	Reset();
}

void PointLight::SpawnControlWindow() noexcept
{
	if(ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &cbData.pos.x, -60.f, 60.0f, "%.f%");
		ImGui::SliderFloat("Y", &cbData.pos.y, -60.f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &cbData.pos.z, -60.0f, 60.0f, "%.1f");

		ImGui::Text( "Intensity/Color" );
		ImGui::SliderFloat( "Intensity",&cbData.diffuseIntensity,0.01f,2.0f,"%.2f",2 );
		ImGui::ColorEdit3( "Diffuse Color",&cbData.diffuseColor.x );
		ImGui::ColorEdit3( "Ambient",&cbData.ambient.x );

		ImGui::Text( "Falloff" );
		ImGui::SliderFloat( "Constant",&cbData.attConst,0.05f,10.0f,"%.2f",4 );
		ImGui::SliderFloat( "Linear",&cbData.attLin,0.0001f,4.0f,"%.4f",8 );
		ImGui::SliderFloat( "Quadratic",&cbData.attQuad,0.0000001f,10.0f,"%.7f",10 );

		if(ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	cbData = {
		{ 0.0f,0.0f,0.0f },
		{ 0.05f,0.05f,0.05f },
		{ 1.0f,1.0f,1.0f },
		1.0f,
		1.0f,
		0.045f,
		0.0075f,
	};
}

void PointLight::Draw(Graphics& graphics) const noexcept(!true)
{
	mesh.SetPos(cbData.pos);
	mesh.Draw(graphics);
}

void PointLight::Bind(Graphics& graphics, DirectX::FXMMATRIX view) const noexcept
{
	auto dataCopy = cbData;
	const auto& pos = DirectX::XMLoadFloat3(&cbData.pos);
	DirectX::XMStoreFloat3(&dataCopy.pos, DirectX::XMVector3Transform(pos, view));
	constantBuffer.Update(graphics, dataCopy);
	constantBuffer.Bind(graphics);
}