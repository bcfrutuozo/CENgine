#pragma once

#include "Window.h";
#include "Timer.h"
#include "ImGuiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Mesh.h"
#include "ScriptParser.h"

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
	PointLight light;
	float speed_factor = 1.0f;

	Model sponza { window.Gfx(), "Models\\sponza\\sponza.obj", 1.0f / 20.0f };
	Model nano { window.Gfx(), "Models\\nano_textured\\nanosuit.obj" };
	Model gobber { window.Gfx(), "Models\\gobber\\GoblinX.obj", 6.0f };
	Model stripey { window.Gfx(), "Models\\brick_wall\\brick_wall.obj", 2.0f };
};
