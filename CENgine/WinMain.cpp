#include "Application.h"

int CALLBACK WinMain(
	HINSTANCE currentInstance,
	HINSTANCE previousInstance,
	LPSTR commandLine,
	int commandShow)
{
	try
	{
		return Application{ commandLine }.Start();
	}
	catch(const CENgineException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch(const std::exception& e)
	{
		MessageBox(nullptr, e.what(),"Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch(...)
	{
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	return -1;
}