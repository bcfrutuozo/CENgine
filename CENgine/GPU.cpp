#include "GPU.h"

#pragma warning(push)
#include "imgui/imgui.h"
#pragma warning(pop)

GPU::GPU(Device device)
	:
	Peripheral(device)
{}

GPU::~GPU()
{ }

void GPU::ShowWidget()
{
	ImGui::Text("%s", m_Name.c_str());
	ImGui::Text("Driver info: %s", m_DriverInfo.c_str());
	ImGui::Text("Fan Speed: %d%%", m_FanSpeed);
	ImGui::Text("Temperature: %dC", m_Temperature);
	ImGui::Text("Power Usage: %dmW", m_PowerUsage);
	ImGui::Text("GPU Utilization: %d%%", m_GPUUtilization);
	ImGui::Text("Memory Utilization: %d%%", m_MemoryUtilization);
}
