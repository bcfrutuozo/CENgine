#include "App.h"
#include "GDIPlusManager.h"
#include "Cube.h"
#include "imgui/imgui.h"

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
	const auto transform = DirectX::XMMatrixRotationRollPitchYaw(pos.roll, pos.pitch, pos.yaw) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
	nano.Draw(window.Gfx(), transform);
	
	light.Draw(window.Gfx());

	// ImGui windows
	camera.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowModelWindow();

	// Present the frame
	window.Gfx().EndFrame();
}

void App::ShowModelWindow()
{
	if( ImGui::Begin( "Model" ) )
	{
		using namespace std::string_literals;

		ImGui::Text( "Orientation" );
		ImGui::SliderAngle( "Roll",&pos.roll,-180.0f,180.0f );
		ImGui::SliderAngle( "Pitch",&pos.pitch,-180.0f,180.0f );
		ImGui::SliderAngle( "Yaw",&pos.yaw,-180.0f,180.0f );

		ImGui::Text( "Position" );
		ImGui::SliderFloat( "X",&pos.x,-20.0f,20.0f );
		ImGui::SliderFloat( "Y",&pos.y,-20.0f,20.0f );
		ImGui::SliderFloat( "Z",&pos.z,-20.0f,20.0f );
	}
	ImGui::End();
}