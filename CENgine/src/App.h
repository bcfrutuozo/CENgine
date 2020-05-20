#pragma once

#include "Window.h";
#include "Timer.h"
#include "ImGuiManager.h"
#include "Camera.h"
#include "PointLight.h"

#include <set>

class App
{
public:
	App();
	~App();

	int Start();
private:
	void Run();
	void SpawnSimulationWindow() noexcept;
	void SpawnBoxControlManagerWindow() noexcept;
	void SpawnBoxWindows() noexcept;
private:
	ImGuiManager imgui;
	Window window;
	Timer timer;
	Camera camera;
	PointLight light;
	float speed_factor = 1.0f;

	std::vector<class Box*> boxes;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 10	;
	std::optional<int> comboBoxIndex;
	std::set<int> boxControlIds;
};
