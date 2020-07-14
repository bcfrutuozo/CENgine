#pragma once

#include "Window.h"
#include "Timer.h"
#include "ImGuiManager.h"
#include "CameraContainer.h"
#include "PointLight.h"
#include "Model.h"
#include "ScriptParser.h"
#include "TestCube.h"
#include "BlurOutlineRenderGraph.h"
#include "Math.h"

class App
{
public:
	App(const std::string& commandLine = "");
	~App();

	int Start();
private:
	void Run(float dt);
	void HandleInput(float dt);
	void ShowImGuiDemoWindow();
private:

	std::string commandLine;
	bool showDemoWindow = false;
	ImGuiManager imgui;
	ScriptParser scriptParser;
	Window window;
	RGP::BlurOutlineRenderGraph renderGraph{ window.Gfx() };
	Timer timer;
	float speed_factor = 1.0f;
	CameraContainer cameras;
	PointLight light;
	TestCube cube{ window.Gfx(),4.0f };
	TestCube cube2{ window.Gfx(),4.0f };
	Model sponza{ window.Gfx(), "Models\\sponza\\sponza.obj", 1.0f / 20.0f };
	Model nano{ window.Gfx(), "Models\\nano_textured\\nanosuit.obj", 2.0f };
	Model gobber{ window.Gfx(), "Models\\gobber\\GoblinX.obj", 4.0f };
	//Model stripey{ window.Gfx(), "Models\\brick_wall\\brick_wall.obj", 2.0f };
	bool savingDepth = false;
};
