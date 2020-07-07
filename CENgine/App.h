#pragma once

#include "Window.h";
#include "Timer.h"
#include "ImGuiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Mesh.h"

#include <set>

class App
{
public:
	App();
	~App();

	int Start();
	void ShowImGuiDemoWindow();
private:
	void Run();
private:

	bool showDemoWindow = false;
	ImGuiManager imgui;
	Window window;
	Timer timer;
	Camera camera;
	PointLight light;
	float speed_factor = 1.0f;

	//Model nano { window.Gfx(), "Models\\nano_textured\\nanosuit.obj" };
	Model gobber { window.Gfx(), "Models\\gobber\\GoblinX.obj" };
	
	struct
	{
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	} pos;
};
