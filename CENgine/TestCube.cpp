#include "TestCube.h"
#include "Cube.h"
#include "BindableCommon.h"
#include "DynamicConstantBuffer.h"
#include "DynamicConstant.h"
#include "TechniqueProbe.h"
#include "Channels.h"
#include "TransformCbufScaling.h"
#include "Channels.h"

#pragma warning(push)
#include "imgui/imgui.h"
#pragma warning(pop)

TestCube::TestCube(Graphics& graphics, float size)
{
	auto model = Cube::MakeIndependentTextured();
	model.Transform(DirectX::XMMatrixScaling(size, size, size));
	model.SetNormalsIndependentFlat();
	const auto geometryTag = "$cube." + std::to_string(size);
	pVertices = Bind::VertexBuffer::Resolve(graphics, geometryTag, model.vertices);
	pIndices = Bind::IndexBuffer::Resolve(graphics, geometryTag, model.indices);
	pTopology = Bind::Topology::Resolve(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	auto tcb = std::make_shared<Bind::TransformCbuf>(graphics);

	{
		Technique shade("Shade", Channel::main);
		{
			Step only("lambertian");

			only.AddBindable(Bind::Texture::Resolve(graphics, "Images\\brickwall.jpg"));
			only.AddBindable(Bind::Sampler::Resolve(graphics));

			auto pvs = Bind::VertexShader::Resolve(graphics, "ShadowTest_VS.cso");
			only.AddBindable(Bind::InputLayout::Resolve(graphics, model.vertices.GetLayout(), *pvs));
			only.AddBindable(std::move(pvs));

			only.AddBindable(Bind::PixelShader::Resolve(graphics, "ShadowTest_PS.cso"));

			DRR::IncompleteLayout lay;
			lay.Add<DRR::Type::Float3>("specularColor");
			lay.Add<DRR::Type::Float>("specularWeight");
			lay.Add<DRR::Type::Float>("specularGloss");
			auto buf = DRR::Buffer(std::move(lay));
			buf["specularColor"] = DirectX::XMFLOAT3{ 1.0f,1.0f,1.0f };
			buf["specularWeight"] = 0.1f;
			buf["specularGloss"] = 20.0f;
			only.AddBindable(std::make_shared<Bind::DynamicCachingPixelConstantBuffer>(graphics, buf, 1u));

			only.AddBindable(Bind::Rasterizer::Resolve(graphics, false));
			only.AddBindable(tcb);

			shade.AddStep(std::move(only));
		}
		AddTechnique(std::move(shade));
	}

	{
		Technique outline("Outline", Channel::main);
		{
			Step mask("outlineMask");

			// TODO: better sub-layout generation tech for future consideration maybe
			mask.AddBindable(Bind::InputLayout::Resolve(graphics, model.vertices.GetLayout(), *Bind::VertexShader::Resolve(graphics, "Solid_VS.cso")));

			mask.AddBindable(std::move(tcb));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			outline.AddStep(std::move(mask));
		}
		{
			Step draw("outlineDraw");

			DRR::IncompleteLayout lay;
			lay.Add<DRR::Type::Float4>("color");
			auto buf = DRR::Buffer(std::move(lay));
			buf["color"] = DirectX::XMFLOAT4{ 1.0f,0.4f,0.4f,1.0f };
			draw.AddBindable(std::make_shared<Bind::DynamicCachingPixelConstantBuffer>(graphics, buf, 1u));

			// TODO: better sub-layout generation tech for future consideration maybe
			draw.AddBindable(Bind::InputLayout::Resolve(graphics, model.vertices.GetLayout(), *Bind::VertexShader::Resolve(graphics, "Solid_VS.cso")));

			draw.AddBindable(std::make_shared<Bind::TransformCbuf>(graphics));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			outline.AddStep(std::move(draw));
		}
		AddTechnique(std::move(outline));
	}

	// Shadow map technique
	{
		Technique map{ "ShadowMap",Channel::shadow,true };
		{
			Step draw("shadowMap");

			// TODO: better sub-layout generation tech for future consideration maybe
			draw.AddBindable(Bind::InputLayout::Resolve(graphics, model.vertices.GetLayout(), *Bind::VertexShader::Resolve(graphics, "Solid_VS.cso")));

			draw.AddBindable(std::make_shared<Bind::TransformCbuf>(graphics));

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			map.AddStep(std::move(draw));
		}
		AddTechnique(std::move(map));
	}
}

void TestCube::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

void TestCube::SetRotation(float roll, float pitch, float yaw) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX TestCube::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}

void TestCube::SpawnControlWindow(Graphics& gfx, const char* name) noexcept
{
	if(ImGui::Begin(name))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);

		class Probe : public TechniqueProbe
		{
		public:
			void OnSetTechnique() override
			{
				using namespace std::string_literals;
				ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, pTech->GetName().c_str());
				bool active = pTech->IsActive();
				ImGui::Checkbox(("Tech Active##"s + std::to_string(techIdx)).c_str(), &active);
				pTech->SetActiveState(active);
			}
			bool OnVisitBuffer(DRR::Buffer& buf) override
			{
				float dirty = false;
				const auto dcheck = [&dirty](bool changed) { dirty = dirty || changed; };
				auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(bufIdx)]
				(const char* label) mutable
				{
					tagScratch = label + tagString;
					return tagScratch.c_str();
				};

				if(auto v = buf["scale"]; v.Exists())
				{
					dcheck(ImGui::SliderFloat(tag("Scale"), &v, 1.0f, 2.0f, "%.3f", 3.5f));
				}
				if(auto v = buf["color"]; v.Exists())
				{
					dcheck(ImGui::ColorPicker4(tag("Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT4&>(v))));
				}
				if(auto v = buf["specularIntensity"]; v.Exists())
				{
					dcheck(ImGui::SliderFloat(tag("Spec. Intens."), &v, 0.0f, 1.0f));
				}
				if(auto v = buf["specularPower"]; v.Exists())
				{
					dcheck(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f, "%.1f", 1.5f));
				}
				return dirty;
			}
		} probe;

		Accept(probe);
	}
	ImGui::End();
}