#pragma once

#include "Hardware.h"
#include "Timer.h"

class Performance
{
public:

	Performance(Timer& timer);
	~Performance() = default;
	void Initialize();
	void ShowWidget();
	void GetWorkload();
	void ReloadStorageDevices();
private:

	static void RaisePrivilege();
	void GetCPUTemperature();

	static constexpr float updatePeriod = 0.5f;

	// Timer to avoid performace retrieval on every frame
	Timer timer;
	float lastSampleTime;

	std::unique_ptr<CPU> m_CPU;
	std::unique_ptr<Memory> m_Memory;
	std::vector<std::unique_ptr<Drive>> m_Disks;
	std::vector<std::unique_ptr<GPU>> m_GPUs;
	std::vector<std::unique_ptr<Volume>> m_Volumes;
};