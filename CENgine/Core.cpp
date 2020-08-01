#include "Core.h"
#include "imgui/imgui.h"

#include <PdhMsg.h>

Core::Core(Device device)
{
	m_Device = device;
}

void Core::Initialize()
{
	PDH_STATUS  pdhStatus = ERROR_SUCCESS;
	LPTSTR      szCounterListBuffer = NULL;
	DWORD       dwCounterListSize = 0;
	LPTSTR      szInstanceListBuffer = NULL;
	DWORD       dwInstanceListSize = 0;
	LPTSTR      szThisInstance = NULL;

	//hEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("CoreStatusEnumerator"));

	// Determine the required buffer size for the data. 
	pdhStatus = PdhEnumObjectItems(
		NULL,                   // real time source
		NULL,                   // local machine
		TEXT("Processor"),        // object to enumerate
		szCounterListBuffer,    // pass NULL and 0
		&dwCounterListSize,     // to get length required
		szInstanceListBuffer,   // buffer size 
		&dwInstanceListSize,    // 
		PERF_DETAIL_WIZARD,     // counter detail level
		0);

	if(pdhStatus == PDH_MORE_DATA)
	{
		// Allocate the buffers and try the call again.
		szCounterListBuffer = (LPTSTR)malloc((dwCounterListSize * sizeof(TCHAR)));
		szInstanceListBuffer = (LPTSTR)malloc((dwInstanceListSize * sizeof(TCHAR)));

		if((szCounterListBuffer != NULL) && (szInstanceListBuffer != NULL))
		{
			pdhStatus = PdhEnumObjectItems(
				NULL,                 // real time source
				NULL,                 // local machine
				TEXT("Processor"),      // object to enumerate
				szCounterListBuffer,  // buffer to receive counter list
				&dwCounterListSize,
				szInstanceListBuffer, // buffer to receive instance list 
				&dwInstanceListSize,
				PERF_DETAIL_WIZARD,   // counter detail level
				0);

			pdhStatus = PdhOpenQuery(NULL, 0, &hQuery);

			if(pdhStatus == ERROR_SUCCESS)
			{
				TCHAR szCounterPath[128];
				
				sprintf_s(szCounterPath, "\\Processor(%d)\\%% Processor Time", m_Device.Index);
				pdhStatus = PdhAddEnglishCounter(hQuery, szCounterPath, 0, &hProcessorCounters);
				
				sprintf_s(szCounterPath, "\\Processor(%d)\\%% Idle Time", m_Device.Index);
				pdhStatus = PdhAddEnglishCounter(hQuery, szCounterPath, 0, &hIdleCounters);
				
				sprintf_s(szCounterPath, "\\Processor(%d)\\%% Privileged Time", m_Device.Index);
				pdhStatus = PdhAddEnglishCounter(hQuery, szCounterPath, 0, &hPrivelegedCounters);
			}
		}
	}

	free(szCounterListBuffer);
	free(szInstanceListBuffer);
}

Core::~Core()
{
	PdhCloseQuery(hQuery);
}

void Core::ShowWidget()
{
	ImGui::Text("Core %d: %.02f%%	 %.02f%%	%.02f%%", m_Device.Index, m_Idle, m_Kernel, m_Processor);
}

void Core::GetWorkload()
{
	PDH_FMT_COUNTERVALUE cvIdle, cvPriveleged, cvProcessor;
	DWORD dwCounterType = 0;

	if(PdhCollectQueryData(hQuery) == ERROR_SUCCESS)
	{
		if(PdhGetFormattedCounterValue(hIdleCounters, PDH_FMT_DOUBLE, &dwCounterType, &cvIdle) == ERROR_SUCCESS)
		{
			m_Idle = cvIdle.doubleValue;
		};

		if(PdhGetFormattedCounterValue(hPrivelegedCounters, PDH_FMT_DOUBLE, &dwCounterType, &cvPriveleged) == ERROR_SUCCESS)
		{
			m_Kernel = cvPriveleged.doubleValue;
		}

		if(PdhGetFormattedCounterValue(hProcessorCounters, PDH_FMT_DOUBLE, &dwCounterType, &cvProcessor) == ERROR_SUCCESS)
		{
			m_Processor = cvProcessor.doubleValue;
		}
	}
}
