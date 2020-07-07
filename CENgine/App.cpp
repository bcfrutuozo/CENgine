#include "App.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include "VertexShader.h"

#include <memory>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

GDIPlusManager gdipm;

App::App()
	:
	window(1920, 1080, "CENgine"),
	light(window.Gfx())
{
	window.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 40.0f));
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
	//nano.Draw(window.Gfx());
	gobber.Draw(window.Gfx());
	
	light.Draw(window.Gfx());

	const auto k = window.keyboard.ReadKey();
	
	if(window.keyboard.IsKeyPressed(k.GetCode()))
	{
		switch( k.GetCode() )
		{
		case VK_ESCAPE:
			if( window.IsCursorEnabled() )
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
			camera.Translate({0.0f, 0.0f, dt});
		}
		if(window.keyboard.IsKeyPressed('A'))
		{
			camera.Translate({-dt, 0.0f, 0.0f});
		}
		if(window.keyboard.IsKeyPressed('S'))
		{
			camera.Translate({ 0.0f, 0.0f, -dt});
		}
		if(window.keyboard.IsKeyPressed('D'))
		{
			camera.Translate({dt, 0.0f, 0.0f});
		}
		if(window.keyboard.IsKeyPressed('R'))
		{
			camera.Translate({ 0.0f, dt, 0.0f});
		}
		if(window.keyboard.IsKeyPressed('F'))
		{
			camera.Translate({0.0f, -dt, 0.0f});
		}
	}

	while(const auto delta = window.mouse.ReadRawDelta())
	{
		if(!window.IsCursorEnabled())
		{
			camera.Rotate(static_cast<float>(delta->x), static_cast<float>(delta->y));
		}
	}
	
	// ImGui windows
	camera.SpawnControlWindow();
	light.SpawnControlWindow();
	ShowImGuiDemoWindow();
	//nano.ShowWindow("Nanosuit");
	gobber.ShowWindow(window.Gfx(), "Gobber");

	// Present the frame
	window.Gfx().EndFrame();
}

void App::ShowImGuiDemoWindow()
{
	if( showDemoWindow )
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}
}
