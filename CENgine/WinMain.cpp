#include "App.h"

int CALLBACK WinMain(
	HINSTANCE currentInstance,
	HINSTANCE previousInstance,
	LPSTR commandLine,
	int commandShow)
{
	try
	{
		//std::string x("--commands process.json");
		return App{ commandLine }.Start();
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