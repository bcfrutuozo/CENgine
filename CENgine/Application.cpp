#include "Application.h"
#include "Math.h"
#include "imgui/imgui.h"
#include "Utilities.h"
#include "Log.h"
#include "Testing.h"
#include "TestModelProbe.h"
#include "Camera.h"
#include "Channels.h"

#include <algorithm>
#include <winioctl.h>
#include <Windows.h>

Application::Application(const std::string& commandLine)
	:
	commandLine(commandLine),
	window(1920, 1080, "CENgine"),
	scriptParser(TokenizedQuoted(commandLine)),
	light(window.Gfx(), { 10.0f,5.0f,0.0f }),
	perf(timer)
{
	perf.Initialize();
	//window.CreateChild(100, 100, "Debugger");
	cameras.AddCamera(std::make_unique<Camera>(window.Gfx(), "A", DirectX::XMFLOAT3{ -13.5f,6.0f,3.5f }, 0.0f, PI / 2.0f));
	cameras.AddCamera(light.ShareCamera());

	cube.SetPos({ 10.0f,5.0f,6.0f });
	cube2.SetPos({ 10.0f,5.0f,14.0f });

	nano.SetRootTransform(
		DirectX::XMMatrixRotationY(PI / 2.f) *
		DirectX::XMMatrixTranslation(27.f, -0.56f, 1.7f)
	);
	gobber.SetRootTransform(
		DirectX::XMMatrixRotationY(-PI / 2.f) *
		DirectX::XMMatrixTranslation(-8.f, 10.f, 0.f)
	);

	cube.LinkTechniques(renderGraph);
	cube2.LinkTechniques(renderGraph);
	light.LinkTechniques(renderGraph);
	sponza.LinkTechniques(renderGraph);
	gobber.LinkTechniques(renderGraph);
	nano.LinkTechniques(renderGraph);
	cameras.LinkTechniques(renderGraph);

	renderGraph.BindShadowCamera(*light.ShareCamera());
}

Application::~Application()
{ }

int Application::Start()
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
		//Sleep(1);
		const auto dt = timer.Mark() * speed_factor;
		HandleInput(dt);
		Run(dt);
	}
}

void Application::HandleInput(float dt)
{
	while(const auto e = window.keyboard.ReadKey())
	{
		if(!e->IsPress())
		{
			switch(e->GetCode())
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
				case VK_RETURN:
				savingDepth = true;
				break;
			}
		}
	}

	if(!window.IsCursorEnabled())
	{
		if(window.keyboard.IsKeyPressed('W'))
		{
			cameras->Translate({ 0.0f, 0.0f, dt });
		}
		if(window.keyboard.IsKeyPressed('A'))
		{
			cameras->Translate({ -dt, 0.0f, 0.0f });
		}
		if(window.keyboard.IsKeyPressed('S'))
		{
			cameras->Translate({ 0.0f, 0.0f, -dt });
		}
		if(window.keyboard.IsKeyPressed('D'))
		{
			cameras->Translate({ dt, 0.0f, 0.0f });
		}
		if(window.keyboard.IsKeyPressed('R'))
		{
			cameras->Translate({ 0.0f, dt, 0.0f });
		}
		if(window.keyboard.IsKeyPressed('F'))
		{
			cameras->Translate({ 0.0f, -dt, 0.0f });
		}
		if(window.keyboard.IsKeyPressed('G'))
		{
			window.SwitchWindowType();
		}
	}


	while(const auto delta = window.mouse.ReadRawDelta())
	{
		if(!window.IsCursorEnabled())
		{
			cameras->Rotate(static_cast<float>(delta->x), static_cast<float>(delta->y));
		}
	}
}

void Application::Run(float dt)
{
	/* Time elapse on window title
	const float t = m_timer.Peek();
	std::ostringstream oss;
	oss << "Timer elapsed: " << std::setprecision(1) << std::fixed << t << "s";
	m_window.SetTitle(oss.str());*/

	window.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	light.Bind(window.Gfx(), cameras->GetMatrix());
	renderGraph.BindMainCamera(cameras.GetActiveCamera());

	// Render geometry
	light.Submit(Channel::main);
	cube.Submit(Channel::main);
	sponza.Submit(Channel::main);
	cube2.Submit(Channel::main);
	gobber.Submit(Channel::main);
	nano.Submit(Channel::main);
	cameras.Submit(Channel::main);

	sponza.Submit(Channel::shadow);
	cube.Submit(Channel::shadow);
	cube2.Submit(Channel::shadow);
	gobber.Submit(Channel::shadow);
	nano.Submit(Channel::shadow);

	perf.GetWorkload();

	renderGraph.Execute(window.Gfx());

	if(savingDepth)
	{
		renderGraph.DumpShadowMap(window.Gfx(), "shadow.png");
		savingDepth = false;
	}

	static MP sponzeProbe{ "Sponza" };
	static MP gobberProbe{ "Gobber" };
	static MP nanoProbe{ "Nano" };

	// imgui windows
	sponzeProbe.SpawnWindow(sponza);
	gobberProbe.SpawnWindow(gobber);
	nanoProbe.SpawnWindow(nano);
	cameras.SpawnWindow(window.Gfx());
	light.SpawnControlWindow();
	renderGraph.RenderWidgets(window.Gfx());
	cube.SpawnControlWindow(window.Gfx(), "Cube 1");
	cube2.SpawnControlWindow(window.Gfx(), "Cube 2");
	perf.ShowWidget();

	// Present the frame
	window.Gfx().EndFrame();
	renderGraph.Reset();
}

void Application::ShowImGuiDemoWindow()
{
	if(showDemoWindow)
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}
}
