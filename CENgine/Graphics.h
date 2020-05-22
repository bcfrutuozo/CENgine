#pragma once

#include "CENgineWin.h"
#include "CENgineException.h"
#include "DxgiInfoManager.h"
#include "Conditional_noexcept.h"

#include <wrl.h>
#include <d3d11.h>
#include <vector>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>

namespace Bind
{
	class Bindable;
}


class Graphics
{
	friend class Bind::Bindable;
	
public:
	class Exception : public CENgineException
	{
		using CENgineException::CENgineException;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMessages = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};

	// Info Exception
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMessages) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};

	// Device Removed Exception
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
public:
	
	Graphics();
	Graphics(HWND handle, int width, int height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();

		void BeginFrame(float red, float green, float blue) const noexcept;
	void EndFrame();
	void DrawIndexed(UINT count) NOXND;
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetCamera(DirectX::FXMMATRIX cmr) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;
	
	void EnableImGui() noexcept;
	void DisableImGui() noexcept;
	bool IsImGuiEnabled() const noexcept;
private:

	bool isImGuiEnabled;
	DirectX::XMMATRIX camera;
	DirectX::XMMATRIX projection;
	
#ifndef NDEBUG
	DXGIInfoManager infoManager;
#endif
	
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
};
