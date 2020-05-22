#include "Graphics.h"
#include "dxerr.h"
#include "GraphicsThrowMacros.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "Conditional_noexcept.h"

#include <sstream>
#include <d3dcompiler.h>

// Set the linker settings
#pragma comment(lib, "d3d11.lib")
// For Shader loading functions real time compilation
#pragma comment(lib, "D3DCompiler.lib")

Graphics::Graphics()
	:
	pDevice(nullptr),
	pSwapChain(nullptr),
	pContext(nullptr),
	pTarget(nullptr),
	pDepthStencilView(nullptr),
	isImGuiEnabled(true)
{
}

Graphics::Graphics(HWND handle,
	int width,
	int height)
	:
	pDevice(nullptr),
	pSwapChain(nullptr),
	pContext(nullptr),
	pTarget(nullptr),
	isImGuiEnabled(true)
{
	DXGI_SWAP_CHAIN_DESC swapDescriptor = {};

	swapDescriptor.BufferDesc.Width = width;
	swapDescriptor.BufferDesc.Height = height;
	swapDescriptor.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// Layout of the pixels RGBA
	swapDescriptor.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	// Refresh rate
	swapDescriptor.BufferDesc.RefreshRate.Numerator = 0;
	swapDescriptor.BufferDesc.RefreshRate.Denominator = 0;
	// Interlaced	
	swapDescriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	// Anti-aliasing
	swapDescriptor.SampleDesc.Count = 1;
	swapDescriptor.SampleDesc.Quality = 0;
	// Buffer function 
	swapDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// Number of buffers
	swapDescriptor.BufferCount = 1;
	// Window Handle
	swapDescriptor.OutputWindow = handle;
	// Windowed Mode
	swapDescriptor.Windowed = TRUE;
	swapDescriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDescriptor.Flags = 0;

	UINT swapCreateFlags = 0u;

#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// To check results of D3D function calls
	HRESULT hr;

	// Create device and front/back buffers along with the swap chain and the rendering context
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapDescriptor,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pContext
	));

	// Gain access to texture resource in swap chain (back buffer)
	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
	GFX_THROW_INFO(pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));

	// Create depth stencil state (Z-BUFFER)
	D3D11_DEPTH_STENCIL_DESC depthDescriptor = {};
	depthDescriptor.DepthEnable = TRUE;
	depthDescriptor.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDescriptor.DepthFunc = D3D11_COMPARISON_LESS;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState;
	GFX_THROW_INFO(pDevice->CreateDepthStencilState(&depthDescriptor, &pDepthStencilState));

	// Bind depth stencil state
	pContext->OMSetDepthStencilState(pDepthStencilState.Get(), 1u);

	// Create depth stencil texture
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = width; // MUST BE THE SAME VALUE OF THE SWAP CHAIN
	descDepth.Height = height; // MUST BE THE SAME VALUE OF THE SWAP CHAIN
	descDepth.MipLevels = 1u;	// Mip maps
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;	// No Anti-aliasing
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0u;
	descDepth.MiscFlags = 0u;
	GFX_THROW_INFO(pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

	// Create view of depth stencil texture
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	GFX_THROW_INFO(pDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &pDepthStencilView));

	// Bind depth stencil view on OM
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDepthStencilView.Get());

	// Configure viewport
	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	pContext->RSSetViewports( 1u,&viewport );

	// Init ImGui D3D implementation
	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
}

Graphics::~Graphics()
{
	ImGui_ImplDX11_Shutdown();
}

void Graphics::BeginFrame(float red, float green, float blue) const noexcept
{
	// ImGui begin frame
	if(isImGuiEnabled)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
	
	const float color[] = { red,green,blue, 1.0f };
	pContext->ClearRenderTargetView(
		pTarget.Get(), color);
	pContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::EndFrame()
{
	// ImGui end frame
	if(isImGuiEnabled)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	
	HRESULT hr;

#ifndef NDEBUG
	infoManager.Set();
#endif

	if (FAILED(hr = pSwapChain->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			GFX_EXCEPT(hr);
		}
	}
}

void Graphics::DrawIndexed(UINT count) NOXND
{
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed(count, 0u, 0u));
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return projection;
}

void Graphics::SetCamera(DirectX::FXMMATRIX cmr) noexcept
{
	camera = cmr;
}

DirectX::XMMATRIX Graphics::GetCamera() const noexcept
{
	return camera;
}

void Graphics::EnableImGui() noexcept
{
	isImGuiEnabled = true;
}

void Graphics::DisableImGui() noexcept
{
	isImGuiEnabled = false;
}

bool Graphics::IsImGuiEnabled() const noexcept
{
	return isImGuiEnabled;
}


// Graphics exceptions
Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMessages) noexcept
	:
	Exception(line, file),
	hr(hr)
{
	// Join all messages with new lines in a single string
	for (const auto& m : infoMessages)
	{
		info += m;
		info.push_back('\n');
	}

	// Remove final new line if it exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;

	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}

	oss << GetOriginString();
	whatBuffer = oss.str();

	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "CENgine Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buffer[512];
	DXGetErrorDescription(hr, buffer, sizeof(buffer));
	return buffer;
}


std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "CENgine Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}


Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMessages) noexcept
	:
	Exception(line, file)
{
	// Join all messages with new lines in a single string
	for (const auto& m : infoMessages)
	{
		info += m;
		info.push_back('\n');
	}

	// Remove final new line if it exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;

	oss << GetOriginString();
	whatBuffer = oss.str();

	return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "CENgine Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}