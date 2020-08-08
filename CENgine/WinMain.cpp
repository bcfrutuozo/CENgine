#include "Application.h"

int CALLBACK WinMain(
	_In_ HINSTANCE currentInstance,
	_In_opt_ HINSTANCE previousInstance,
	_In_ LPSTR commandLine,
	_In_ int commandShow)
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