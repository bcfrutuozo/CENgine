#include "PointLight.h"
#include "imgui/imgui.h"
#include "Camera.h"

PointLight::PointLight(Graphics& graphics, float radius)
	:
	mesh(graphics, radius),
	constantBuffer(graphics)
{
	Reset();
	pCamera = std::make_shared<Camera>(graphics, "Light", cbData.pos, 0.0f, 0.0f, true);
}

void PointLight::SpawnControlWindow() noexcept
{
	if(ImGui::Begin("Light"))
	{
		bool dirtyPos = false;
		const auto dcheck = [&dirtyPos](bool dirty) { dirtyPos = dirtyPos || dirty; };

		ImGui::Text("Position");
		dcheck(ImGui::SliderFloat("X", &cbData.pos.x, -60.f, 60.0f, "%.f%"));
		dcheck(ImGui::SliderFloat("Y", &cbData.pos.y, -60.f, 60.0f, "%.1f"));
		dcheck(ImGui::SliderFloat("Z", &cbData.pos.z, -60.0f, 60.0f, "%.1f"));

		if(dirtyPos)
		{
			pCamera->SetPosition(cbData.pos);
		}

		ImGui::Text("Intensity/Color");
		ImGui::SliderFloat("Intensity", &cbData.diffuseIntensity, 0.01f, 2.0f, "%.2f", 2);
		ImGui::ColorEdit3("Diffuse Color", &cbData.diffuseColor.x);
		ImGui::ColorEdit3("Ambient", &cbData.ambient.x);

		ImGui::Text("Falloff");
		ImGui::SliderFloat("Constant", &cbData.attConst, 0.05f, 10.0f, "%.2f", 4);
		ImGui::SliderFloat("Linear", &cbData.attLin, 0.0001f, 4.0f, "%.4f", 8);
		ImGui::SliderFloat("Quadratic", &cbData.attQuad, 0.0000001f, 10.0f, "%.7f", 10);

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
		{ 10.0f,9.0f,2.5f },
		{ 0.05f,0.05f,0.05f },
		{ 1.0f,1.0f,1.0f },
		1.0f,
		1.0f,
		0.045f,
		0.0075f,
	};
}

void PointLight::Submit(size_t channels) const NOXND
{
	mesh.SetPos(cbData.pos);
	mesh.Submit(channels);
}

void PointLight::Bind(Graphics& graphics, DirectX::FXMMATRIX view) const noexcept
{
	auto dataCopy = cbData;
	const auto& pos = DirectX::XMLoadFloat3(&cbData.pos);
	DirectX::XMStoreFloat3(&dataCopy.pos, DirectX::XMVector3Transform(pos, view));
	constantBuffer.Update(graphics, dataCopy);
	constantBuffer.Bind(graphics);
}

void PointLight::LinkTechniques(RGP::RenderGraph& renderGraph)
{
	mesh.LinkTechniques(renderGraph);
}

std::shared_ptr<Camera> PointLight::ShareCamera() const noexcept
{
	return pCamera;
}