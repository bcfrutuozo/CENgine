#pragma once

#include "CENgineWin.h"
#include "CENgineException.h"
#include "DxgiInfoManager.h"
#include "Conditional_noexcept.h"
#include "CENgineWRL.h"

#include <d3d11.h>
#include <vector>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>

class DepthStencil;

namespace Bind
{
	class Bindable;
	class RenderTarget;
}

class Graphics
{
	friend class GraphicsResource;
	
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

	enum class Type
	{
		Windowed,
		Fullscreen
	};

	Graphics(HWND handle, int width, int height, Type windowType = Type::Windowed);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	void SetFullscreenState(Type type);
	Type GetFullscreenState() const noexcept;
	void BeginFrame(float red, float green, float blue) const;
	void EndFrame();
	void DrawIndexed(UINT count) NOXND;
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetCamera(DirectX::FXMMATRIX cmr) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;	
	void EnableImGui() noexcept;
	void DisableImGui() noexcept;
	bool IsImGuiEnabled() const noexcept;
	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;
	std::shared_ptr<Bind::RenderTarget> GetTarget();
private:

	UINT width;
	UINT height;
	bool isImGuiEnabled;
	bool hasWindowTypeSwitched = false;
	Type windowType;
	DirectX::XMMATRIX camera;
	DirectX::XMMATRIX projection;
	
#ifndef NDEBUG
	DXGIInfoManager infoManager;
#endif
	
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	std::shared_ptr<Bind::RenderTarget> pTarget;
};
