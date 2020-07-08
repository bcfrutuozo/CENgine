#include "App.h"
#include "imgui/imgui.h"
#include "VertexShader.h"
#include "TexturePreprocessor.h"

#include <memory>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <shellapi.h>
#include <dxtex/DirectXTex.h>

App::App(const std::string& commandLine)
	:
	commandLine(commandLine),
	window(1920, 1080, "CENgine"),
	light(window.Gfx())
{
	if(this->commandLine != "" )
	{
		int nArgs;
		const auto pLineW = GetCommandLineW();
		const auto pArgs = CommandLineToArgvW(pLineW, &nArgs);
		if(nArgs >= 4 && std::wstring(pArgs[1]) == L"--tweak-objnorm")
		{
			const std::wstring pathInWide = pArgs[2];
			const std::wstring pathOutWide = pArgs[3];
			TexturePreprocessor::FlipYAllNormalMapsInObject(
				std::string(pathInWide.begin(), pathInWide.end()));
		}
		else if(nArgs >= 3 && std::wstring( pArgs[1] ) == L"--tweak-flipy")
		{
			const std::wstring pathInWide = pArgs[2];
			const std::wstring pathOutWide = pArgs[3];
			TexturePreprocessor::FlipYNormalMap(
				std::string(pathInWide.begin(), pathInWide.end()),
				std::string(pathOutWide.begin(), pathOutWide.end()));
		}
		else if (nArgs >= 4 && std::wstring(pArgs[1]) == L"--tweak-validate")
		{
			const std::wstring minWide = pArgs[2];
			const std::wstring maxWide = pArgs[3];
			const std::wstring pathWide = pArgs[4];
			TexturePreprocessor::ValidateNormalMap(
				std::string(pathWide.begin(), pathWide.end()), std::stof(minWide), std::stof(maxWide)
			);
			throw std::runtime_error("Normal map validated successfully. Just kidding about that whole runtime error thing.");
		}
	}

	stripey.SetRootTransform(DirectX::XMMatrixTranslation(-13.5f, 6.0f, 3.5f));

	window.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 400.0f));
	window.ShowConsole();
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
	nano.Draw(window.Gfx());
	gobber.Draw(window.Gfx());
	stripey.Draw(window.Gfx());
	
	light.Draw(window.Gfx());
	sponza.Draw(window.Gfx());

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
	nano.ShowWindow(window.Gfx(), "Nanosuit");
	gobber.ShowWindow(window.Gfx(), "Gobber");
	sponza.ShowWindow(window.Gfx(), "Sponza");
	stripey.ShowWindow(window.Gfx(), "Stripey");

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
