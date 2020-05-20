#pragma once

#include "CENgineWin.h"
#include "CENgineException.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"

#include <optional>
#include <memory>

class Window
{
public:

	// Exception
	class Exception : public CENgineException
	{
		using CENgineException::CENgineException;
	public:
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
	};

	// HrException
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};

	// NoGraphicsException
	class NoGraphicsException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
	};

private:
	// Singleton manages registration/cleanup of window class
	class WindowClass
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;

	private:
		WindowClass() noexcept;
		~WindowClass() noexcept;
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* windowClassName = "CENgine Direct3D Engine";
		static WindowClass windowClass;
		HINSTANCE handleInstance;
	};

public:
	Window(int width, int height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	void SetTitle(const std::string& title) noexcept;
	static std::optional<int> ProcessMessages();
	Graphics& Gfx() const;
	
	Graphics graphics;
	Keyboard keyboard;
	Mouse mouse;

private:
	static LRESULT CALLBACK HandleMessageSetup(HWND handleWindow, UINT message, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMessageThunk(HWND handleWindow, UINT message, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT CALLBACK HandleMessage(HWND handleWindow, UINT message, WPARAM wParam, LPARAM lParam) noexcept;

	int width;
	int height;
	HWND handleWindow;
	std::unique_ptr<Graphics> pGraphics;
};