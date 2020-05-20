#include "ImGuiManager.h"
#include "imgui/imgui.h"

ImGuiManager::ImGuiManager()
{
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGui::StyleColorsLight();
}

ImGuiManager::~ImGuiManager()
{
	ImGui::DestroyContext();
}