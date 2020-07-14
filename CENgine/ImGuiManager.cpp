#include "ImGuiManager.h"
#include "imgui/imgui.h"

#include <filesystem>

ImGuiManager::ImGuiManager()
{
	if(!std::filesystem::exists("imgui.ini") && std::filesystem::exists("imgui_default.ini"))
	{
		std::filesystem::copy_file("imgui_default.ini", "imgui.ini");
	}

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGui::StyleColorsLight();
}

ImGuiManager::~ImGuiManager()
{
	ImGui::DestroyContext();
}