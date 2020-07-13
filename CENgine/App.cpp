#include "App.h"
#include "Math.h"
#include "Surface.h"
#include "imgui/imgui.h"
#include "VertexBuffer.h"
#include "Utilities.h"
#include "Log.h"
#include "Mesh.h"
#include "DynamicConstant.h"
#include "ModelProbe.h"
#include "Node.h"
#include "Matrix.h"
#include "BufferClearPass.h"
#include "LambertianPass.h"
#include "TechniqueProbe.h"

#include <memory>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

App::App(const std::string& commandLine)
	:
	commandLine(commandLine),
	window(1920, 1080, "CENgine"),
	scriptParser(TokenizedQuoted(commandLine)),
	light(window.Gfx())
{
	cube.SetPos({ 4.0f,0.0f,0.0f });
	cube2.SetPos({ 0.0f,4.0f,0.0f });

	{
		{
			auto bcp = std::make_unique<BufferClearPass>( "clear" );
			bcp->SetInputSource( "renderTarget","$.backbuffer" );
			bcp->SetInputSource( "depthStencil","$.masterDepth" );
			renderGraph.AppendPass( std::move( bcp ) );
		}
		{
			auto lp = std::make_unique<LambertianPass>( "lambertian" );
			lp->SetInputSource( "renderTarget","clear.renderTarget" );
			lp->SetInputSource( "depthStencil","clear.depthStencil" );
			renderGraph.AppendPass( std::move( lp ) );
		}
		renderGraph.SetSinkTarget( "backbuffer","lambertian.renderTarget" );
		renderGraph.Finalize();

		cube.LinkTechniques( renderGraph );
		cube2.LinkTechniques( renderGraph );
		light.LinkTechniques( renderGraph );
	}

	window.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 400.0f));
}

App::~App()
{

}

int App::Start()
{
	while(true)
	{
		// Process all messages pending but doesn't block new messages
		if(const auto ecode = Window::ProcessMessages())
		{
			// if return option has value exits the application
			return *ecode;
		}

		// While VSync is not enabled and synchronized with pooling rate to avoid CPU hangs
		Sleep(1);

		Run();
	}
}

void App::Run()
{
	/* Time elapse on window title
	const float t = m_timer.Peek();
	std::ostringstream oss;
	oss << "Timer elapsed: " << std::setprecision(1) << std::fixed << t << "s";
	m_window.SetTitle(oss.str());*/

	const auto dt = timer.Mark() * speed_factor;

	window.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	window.Gfx().SetCamera(camera.GetMatrix());
	light.Bind(window.Gfx(), camera.GetMatrix());

	// Render geometry
	//nano.Submit();
	//gobber.Submit();
	light.Submit();
	cube.Submit();
	//sponza.Submit();
	cube2.Submit();
	//stripey.Submit();

	renderGraph.Execute(window.Gfx());

	const auto k = window.keyboard.ReadKey();

	if(window.keyboard.IsKeyPressed(k.GetCode()))
	{
		switch(k.GetCode())
		{
			case VK_ESCAPE:
			if(window.IsCursorEnabled())
			{
				window.DisableCursor();
				window.mouse.EnableRaw();
			}
			else
			{
				window.EnableCursor();
				window.mouse.DisableRaw();
			}
			break;
			case VK_F1:
			showDemoWindow = true;
			break;
		}
	}

	if(!window.IsCursorEnabled())
	{
		if(window.keyboard.IsKeyPressed('W'))
		{
			camera.Translate({ 0.0f, 0.0f, dt });
		}
		if(window.keyboard.IsKeyPressed('A'))
		{
			camera.Translate({ -dt, 0.0f, 0.0f });
		}
		if(window.keyboard.IsKeyPressed('S'))
		{
			camera.Translate({ 0.0f, 0.0f, -dt });
		}
		if(window.keyboard.IsKeyPressed('D'))
		{
			camera.Translate({ dt, 0.0f, 0.0f });
		}
		if(window.keyboard.IsKeyPressed('R'))
		{
			camera.Translate({ 0.0f, dt, 0.0f });
		}
		if(window.keyboard.IsKeyPressed('F'))
		{
			camera.Translate({ 0.0f, -dt, 0.0f });
		}
	}


	while(const auto delta = window.mouse.ReadRawDelta())
	{
		if(!window.IsCursorEnabled())
		{
			camera.Rotate(static_cast<float>(delta->x), static_cast<float>(delta->y));
		}
	}

	// Mesh techniques window
	class TP : public TechniqueProbe
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
			namespace dx = DirectX;
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
			if(auto v = buf["offset"]; v.Exists())
			{
				dcheck(ImGui::SliderFloat(tag("offset"), &v, 0.0f, 1.0f, "%.3f", 2.5f));
			}
			if(auto v = buf["materialColor"]; v.Exists())
			{
				dcheck(ImGui::ColorPicker3(tag("Color"), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v))));
			}
			if(auto v = buf["specularColor"]; v.Exists())
			{
				dcheck(ImGui::ColorPicker3(tag("Spec. Color"), reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v))));
			}
			if(auto v = buf["specularGloss"]; v.Exists())
			{
				dcheck(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f, "%.1f", 1.5f));
			}
			if(auto v = buf["specularWeight"]; v.Exists())
			{
				dcheck(ImGui::SliderFloat(tag("Spec. Weight"), &v, 0.0f, 2.0f));
			}
			if(auto v = buf["useSpecularMap"]; v.Exists())
			{
				dcheck(ImGui::Checkbox(tag("Spec. Map Enable"), &v));
			}
			if(auto v = buf["useNormalMap"]; v.Exists())
			{
				dcheck(ImGui::Checkbox(tag("Normal Map Enable"), &v));
			}
			if(auto v = buf["normalMapWeight"]; v.Exists())
			{
				dcheck(ImGui::SliderFloat(tag("Normal Map Weight"), &v, 0.0f, 2.0f));
			}
			return dirty;
		}
	};

	class MP : ModelProbe
	{
	public:
		void SpawnWindow(Model& model)
		{
			ImGui::Begin("Model");
			ImGui::Columns(2, nullptr, true);
			model.Accept(*this);

			ImGui::NextColumn();
			if(pSelectedNode != nullptr)
			{
				bool dirty = false;
				const auto dcheck = [&dirty](bool changed) { dirty = dirty || changed; };
				auto& tf = ResolveTransform();
				ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Translation");
				dcheck(ImGui::SliderFloat("X", &tf.x, -60.f, 60.f));
				dcheck(ImGui::SliderFloat("Y", &tf.y, -60.f, 60.f));
				dcheck(ImGui::SliderFloat("Z", &tf.z, -60.f, 60.f));
				ImGui::TextColored({ 0.4f,1.0f,0.6f,1.0f }, "Orientation");
				dcheck(ImGui::SliderAngle("X-rotation", &tf.xRot, -180.0f, 180.0f));
				dcheck(ImGui::SliderAngle("Y-rotation", &tf.yRot, -180.0f, 180.0f));
				dcheck(ImGui::SliderAngle("Z-rotation", &tf.zRot, -180.0f, 180.0f));
				if(dirty)
				{
					pSelectedNode->SetAppliedTransform(
						DirectX::XMMatrixRotationX(tf.xRot) *
						DirectX::XMMatrixRotationY(tf.yRot) *
						DirectX::XMMatrixRotationZ(tf.zRot) *
						DirectX::XMMatrixTranslation(tf.x, tf.y, tf.z)
					);
				}
			}
			if(pSelectedNode != nullptr)
			{
				TP probe;
				pSelectedNode->Accept(probe);
			}
			ImGui::End();
		}
	protected:
		bool PushNode(Node& node) override
		{
			// if there is no selected node, set selectedId to an impossible value
			const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
			// build up flags for current node
			const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
				| ((node.GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
				| (node.HasChildren() ? 0 : ImGuiTreeNodeFlags_Leaf);
			// render this node
			const auto expanded = ImGui::TreeNodeEx(
				(void*)(intptr_t)node.GetId(),
				node_flags, node.GetName().c_str()
			);
			// processing for selecting node
			if(ImGui::IsItemClicked())
			{
				// used to change the highlighted node on selection change
				struct Probe : public TechniqueProbe
				{
					virtual void OnSetTechnique()
					{
						if(pTech->GetName() == "Outline")
						{
							pTech->SetActiveState(highlighted);
						}
					}
					bool highlighted = false;
				} probe;

				// remove highlight on prev-selected node
				if(pSelectedNode != nullptr)
				{
					pSelectedNode->Accept(probe);
				}
				// add highlight to newly-selected node
				probe.highlighted = true;
				node.Accept(probe);

				pSelectedNode = &node;
			}
			// signal if children should also be recursed
			return expanded;
		}
		void PopNode(Node& node) override
		{
			ImGui::TreePop();
		}
	private:
		Node* pSelectedNode = nullptr;
		struct TransformParameters
		{
			float xRot = 0.0f;
			float yRot = 0.0f;
			float zRot = 0.0f;
			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
		};
		std::unordered_map<int, TransformParameters> transformParams;
	private:
		TransformParameters& ResolveTransform() noexcept
		{
			const auto id = pSelectedNode->GetId();
			auto i = transformParams.find(id);
			if(i == transformParams.end())
			{
				return LoadTransform(id);
			}
			return i->second;
		}
		TransformParameters& LoadTransform(int id) noexcept
		{
			const auto& applied = pSelectedNode->GetAppliedTransform();
			const auto angles = ExtractEulerAngles(applied);
			const auto translation = ExtractTranslation(applied);
			TransformParameters tp;
			tp.zRot = angles.z;
			tp.xRot = angles.x;
			tp.yRot = angles.y;
			tp.x = translation.x;
			tp.y = translation.y;
			tp.z = translation.z;
			return transformParams.insert({ id,{ tp } }).first->second;
		}
	};
	static MP modelProbe;

	// imgui windows
	/*modelProbe.SpawnWindow(sponza);
	modelProbe.SpawnWindow(stripey);
	modelProbe.SpawnWindow(gobber);*/
	camera.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowImGuiDemoWindow();
	cube.SpawnControlWindow(window.Gfx(), "Cube 1");
	cube2.SpawnControlWindow(window.Gfx(), "Cube 2");

	// Present the frame
	window.Gfx().EndFrame();
	renderGraph.Reset();
}

void App::ShowImGuiDemoWindow()
{
	if(showDemoWindow)
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}
}
