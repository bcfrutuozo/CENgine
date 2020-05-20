#include "App.h"

int CALLBACK WinMain(
	HINSTANCE p_currentInstance,
	HINSTANCE p_previousInstance,
	LPSTR p_commandLine,
	int p_commandShow)
{
	try
	{
		return App{}.Start();
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