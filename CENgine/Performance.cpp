#include "Performance.h"
#include "Utilities.h"

#pragma warning(push)
#include "imgui/imgui.h"
#pragma warning(pop)

Performance::Performance(Timer& timer)
	:
	timer(timer),
	lastSampleTime(0.0f),
	m_CPU(Hardware::GetDevice<CPU>()),
	m_Memory(Hardware::GetDevice<Memory>()),
	m_GPUs(Hardware::GetDevices<GPU>()),
	m_Disks(Hardware::GetDevices<Drive>()),
	m_Volumes(Hardware::GetDevices<Volume>())
{

}

void Performance::Initialize()
{
	lastSampleTime = timer.Peek();

	m_CPU->Initialize();
	m_Memory->Initialize();

	for (const auto& gpu : m_GPUs)
	{
		gpu->Initialize();
	}

	for (const auto& disk : m_Disks)
	{
		disk->Initialize();
	}

	for (const auto& volume : m_Volumes)
	{
		volume->Initialize();
	}
}

void Performance::ShowWidget()
{
	if (ImGui::Begin("Performance"))
	{
		ImGui::Separator();
		m_CPU->ShowWidget();
		ImGui::Separator();
		ImGui::Text("Memory");
		m_Memory->ShowWidget();
		ImGui::Separator();
		for (const auto& gpu : m_GPUs)
		{
			gpu->ShowWidget();
		}
		ImGui::Separator();
		ImGui::Text("Drives");
		for (const auto& disk : m_Disks)
		{
			disk->ShowWidget();
		}
		ImGui::Separator();
		for (const auto& volume : m_Volumes)
		{
			volume->ShowWidget();
		}
	}

	ImGui::End();
}

void Performance::GetWorkload()
{
	const auto now = timer.Peek();

	if (now > (lastSampleTime + updatePeriod))
	{
		lastSampleTime = now;

		m_CPU->GetWorkload();
		m_Memory->GetWorkload();

		for (const auto& gpu : m_GPUs)
		{
			gpu->GetWorkload();
		}

		for (const auto& disk : m_Disks)
		{
			disk->GetWorkload();
		}

		for (const auto& volume : m_Volumes)
		{
			volume->GetWorkload();
		}
	}
}

void Performance::ReloadStorageDevices()
{
	m_Disks.clear();
	auto newDisks = Hardware::GetDevices<Drive>();
	m_Disks.reserve(newDisks.size());

	m_Volumes.clear();
	auto newVolumes = Hardware::GetDevices<Volume>();
	m_Volumes.reserve(newVolumes.size());

	m_Disks = std::move(newDisks);
	m_Volumes = std::move(newVolumes);


	for (const auto& disk : m_Disks)
	{
		disk->Initialize();
	}

	for (const auto& volume : m_Volumes)
	{
		volume->Initialize();
	}
}

void Performance::RaisePrivilege()
{

}

void Performance::GetCPUTemperature()
{
	//TODO: IMPLEMENT TEMPERATURE
}