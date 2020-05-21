#include "App.h"
#include "Box.h"
#include "Cylinder.h"
#include "Pyramid.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "Math.h"
#include "SkinnedBox.h"
#include "Cube.h"
#include "imgui/imgui.h"
#include "AssimpTest.h"

#include <memory>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

GDIPlusManager gdipm;

App::App()
	:
	window(1600, 1200, "CENgine"),
	light(window.Gfx())
{
	class Factory
	{
	public:
		Factory(Graphics& graphics)
			:
			graphics(graphics)
		{}

		std::unique_ptr<Drawable> operator()()
		{
			const DirectX::XMFLOAT3 mat = { cDist(rng), cDist(rng), cDist(rng) };

			switch (sDist(rng))
			{
			case 0:
				return std::make_unique<Box>(graphics, rng, aDist, dDist, oDist, rDist, bDist, mat);
			case 1:
				return std::make_unique<Cylinder>(graphics, rng, aDist, dDist, oDist, rDist, bDist, tDist);
			case 2:
				return std::make_unique<Pyramid>(graphics, rng, aDist, dDist, oDist, rDist, tDist);
			case 3:
				return std::make_unique<SkinnedBox>(graphics, rng, aDist, dDist, oDist, rDist);
			case 4:
				return std::make_unique<AssimpTest>(graphics, rng, aDist, dDist, oDist, rDist, mat, 1.5f);
			default:
				assert(false && "impossible drawable option in factory");
				return {};
			}
		}

	private:

		Graphics& graphics;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_int_distribution<int> sDist{ 0, 4 };
		std::uniform_real_distribution<float> aDist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> dDist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> oDist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rDist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bDist{ 0.4f,3.0f };
		std::uniform_real_distribution<float> cDist{ 0.0f,1.0f };
		std::uniform_int_distribution<int> tDist{ 3,30 };
	};

	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, Factory{ window.Gfx() });

	// Init box pointers for editing instance parameters
	for (auto& pd : drawables)
	{
		if (auto pb = dynamic_cast<Box*>(pd.get()))
		{
			boxes.push_back(pb);
		}
	}

	window.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
}

App::~App()
{

}

int App::Start()
{
	while (true)
	{
		// Process all messages pending but doesn't block new messages
		if (const auto ecode = Window::ProcessMessages())
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
	for (auto& d : drawables)
	{
		d->Update(window.keyboard.IsKeyPressed(VK_SPACE) ? 0.0f : dt);
		d->Draw(window.Gfx());
	}
	light.Draw(window.Gfx());

	// ImGui windows
	SpawnSimulationWindow();
	camera.SpawnControlWindow();
	light.SpawnControlWindow();
	SpawnBoxControlManagerWindow();
	SpawnBoxWindows();

	// Present the frame
	window.Gfx().EndFrame();
}

void App::SpawnSimulationWindow() noexcept
{
	if (ImGui::Begin("Simulation Speed"))
	{
		ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 6.0f, "%.4f", 3.2f);
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Status: %s", window.keyboard.IsKeyPressed(VK_SPACE) ? "PAUSED" : "RUNNING (hold spacebar to pause)");
	}
	ImGui::End();
}

void App::SpawnBoxControlManagerWindow() noexcept
{
	if (ImGui::Begin("Boxes"))
	{
		using namespace std::string_literals;
		const auto preview = comboBoxIndex ? std::to_string(*comboBoxIndex) : "Choose a box..."s;
		if (ImGui::BeginCombo("Box Number", preview.c_str()))
		{
			for (int i = 0; i < boxes.size(); i++)
			{
				const bool selected = *comboBoxIndex == i;
				if (ImGui::Selectable(std::to_string(i).c_str(), selected))
				{
					comboBoxIndex = i;
				}
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Spawn Control Window") && comboBoxIndex)
		{
			boxControlIds.insert(*comboBoxIndex);
			comboBoxIndex.reset();
		}
	}
	ImGui::End();
}

void App::SpawnBoxWindows() noexcept
{
	for (auto i = boxControlIds.begin(); i != boxControlIds.end();)
	{
		if (!boxes[*i]->SpawnControlWindow(*i, window.Gfx()))
		{
			i = boxControlIds.erase(i);
		}
		else
		{
			i++;
		}
	}
}