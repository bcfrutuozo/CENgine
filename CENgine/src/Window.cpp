#include "Window.h"
#include "WindowsThrowMacros.h"
#include "imgui/imgui_impl_win32.h"
#include "resource.h"

#include <sstream>

// Singleton WindowClass
Window::WindowClass Window::WindowClass::windowClass;

Window::WindowClass::WindowClass() noexcept
	:
	handleInstance(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMessageSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));;
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(windowClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return windowClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return windowClass.handleInstance;
}

// Window
Window::Window(int width, int p_height, const char* p_name)
	:
	width(width),
	height(p_height)
{
	// Calculate window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = p_height + wr.top;
	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}

	// Create window and get its handle
	handleWindow = CreateWindow(
		WindowClass::GetName(), p_name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(),
		this);

	if (handleWindow == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}

	// Show window
	ShowWindow(handleWindow, SW_SHOWDEFAULT);

	// Init ImGui_Win32 implementation
	ImGui_ImplWin32_Init(handleWindow);
	
	// Create graphics object
	pGraphics = std::make_unique<Graphics>(handleWindow);
}

Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(handleWindow);
}

void Window::SetTitle(const std::string& t_Title) noexcept
{
	if(SetWindowText(handleWindow, t_Title.c_str()) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessages()
{
	MSG msg;

	// While queue has messages, remove and dispatch them without blocking on empty queue
	while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// Check for quit message because PeekMessage doesn't signal it via return val
		if(msg.message == WM_QUIT)
		{
			// Return optional wrapping int (arg to PostQuitMessage is in wParam) signals quit
			return static_cast<int>(msg.wParam);
		}

		// Translate will post auxiliary WM_CHAR messages from key messages
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Return empty optional when not exiting application
	return {};
}

Graphics& Window::Gfx() const
{
	if(!pGraphics)
	{
		throw CHWND_NOGFX_EXCEPT();
	}
	
	return *pGraphics;
}

LRESULT WINAPI Window::HandleMessageSetup(HWND handleWindow, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
	// Use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
	if (message == WM_NCCREATE)
	{
		// Extract pointer to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// Set WinAPI-managed user data do store pointer to window class
		SetWindowLongPtr(handleWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// Set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(handleWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMessageThunk));
		// Forward message to window class handler
		return pWnd->HandleMessage(handleWindow, message, wParam, lParam);
	}

	// If we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(handleWindow, message, wParam, lParam);
}

LRESULT WINAPI Window::HandleMessageThunk(HWND handleWindow, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
	// Retrieve pointer to window class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(handleWindow, GWLP_USERDATA));
	// Forward message to window class handler
	return pWnd->HandleMessage(handleWindow, message, wParam, lParam);
}

LRESULT Window::HandleMessage(HWND handleWindow, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
	if(ImGui_ImplWin32_WndProcHandler(handleWindow, message, wParam, lParam))
	{
		return true;
	}

	const auto& imio = ImGui::GetIO();
	
	switch (message)
	{

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

		// Clear key state when window loses focus to prevent input getting "stuck" 
	case WM_KILLFOCUS:
		keyboard.ClearState();
		break;

		/************************ KEYBOARD MESSAGES ************************/
	case WM_KEYDOWN:
		// Syskey commands need to be handled to track ALT key (VK_MENU)
	case WM_SYSKEYDOWN:

		// Stifle the keyboard message if ImGui wants to capture message
		if(imio.WantCaptureKeyboard)
		{
			break;
		}
		
		if (!(lParam & 0x40000000) || keyboard.IsAutorepeatEnabled()) // Filter Autorepeat key events
		{
			keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:

		// Stifle the keyboard message if ImGui wants to capture message
		if(imio.WantCaptureKeyboard)
		{
			break;
		}
		
		keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		// Stifle the keyboard message if ImGui wants to capture message
		if(imio.WantCaptureKeyboard)
		{
			break;
		}
		
		keyboard.OnChar(static_cast<unsigned char>(wParam));
		break;
		/********************** END KEYBOARD MESSAGES **********************/

		/********************** MOUSE MESSAGES **********************/
	case WM_MOUSEMOVE:

		// Stifle the mouse message if ImGui wants to capture message
		if(imio.WantCaptureMouse)
		{
			break;
		}
	{
		POINTS pt = MAKEPOINTS(lParam);

		// Check if in client region -> log move, and log enter + capture mouse (
		mouse.OnMouseMove(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		// Stifle the mouse message if ImGui wants to capture message
		if(imio.WantCaptureMouse)
		{
			break;
		}
			
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		// Stifle the mouse message if ImGui wants to capture message
		if(imio.WantCaptureMouse)
		{
			break;
		}
			
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		// Stifle the mouse message if ImGui wants to capture message
		if(imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP:
	{
		// Stifle the mouse message if ImGui wants to capture message
		if(imio.WantCaptureMouse)
		{
			break;
		}
			
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		// Stifle the mouse message if ImGui wants to capture message
		if(imio.WantCaptureMouse)
		{
			break;
		}
			
		const POINTS pt = MAKEPOINTS(lParam);
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			mouse.OnWheelUp(pt.x, pt.y);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
		{
			mouse.OnWheelDown(pt.x, pt.y);
		}
	}

	/******************** END MOUSE MESSAGES ********************/
	}

	return DefWindowProc(handleWindow, message, wParam, lParam);
}

// Window Exceptions
std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMessageBuffer = nullptr;

	// Windows will allocate memory for error string and make our pointer point to it
	const DWORD nMessageLength = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMessageBuffer),
		0,
		nullptr);

	// String length as 0 indicates a failure
	if(nMessageLength == 0)
	{
		return "Unidentified error code";
	}

	// Copy error string from windows allocated buffer to string
	std::string errorString = pMessageBuffer;

	// Free windows buffer
	LocalFree(pMessageBuffer);

	return errorString;
}


Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	hr(hr)
{}

const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
	return "CENgine Window Exception";
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exception::TranslateErrorCode(hr);
}

const char* Window::NoGraphicsException::GetType() const noexcept
{
	return "CENgine Window Exception [No Graphics]";
}