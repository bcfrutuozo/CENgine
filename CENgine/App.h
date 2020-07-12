#pragma once

#include "Window.h";
#include "Timer.h"
#include "ImGuiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Model.h"
#include "ScriptParser.h"
#include "FrameGenerator.h"
#include "Stencil.h"
#include "Material.h"
#include "TestCube.h"

#include <set>

class App
{
public:
	App(const std::string& commandLine = "");
	~App();

	int Start();
	void ShowImGuiDemoWindow();
private:
	void Run();
private:

	std::string commandLine;
	bool showDemoWindow = false;
	ImGuiManager imgui;
	ScriptParser scriptParser;
	Window window;
	Timer timer;
	Camera camera;
	FrameGenerator frame{ window.Gfx() };
	PointLight light;
	float speed_factor = 1.0f;

	TestCube cube{ window.Gfx(),4.0f };
	TestCube cube2{ window.Gfx(),4.0f };
	Model sponza{ window.Gfx(), "Models\\sponza\\sponza.obj", 1.0f / 20.0f };
	//Model nano { window.Gfx(), "Models\\nano_textured\\nanosuit.obj", 2.0f };
	Model gobber { window.Gfx(), "Models\\gobber\\GoblinX.obj", 1.0f };
	Model stripey { window.Gfx(), "Models\\brick_wall\\brick_wall.obj", 2.0f };
};
