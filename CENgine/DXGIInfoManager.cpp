#include "DxgiInfoManager.h"
#include "Window.h"
#include "Graphics.h"
#include "WindowsThrowMacros.h"
#include "GraphicsThrowMacros.h"

#include <dxgidebug.h>
#include <memory>

#pragma comment(lib, "dxguid.lib")

DXGIInfoManager::DXGIInfoManager()
	:
	next(0u)
{
	// Define function signature of DXGIGetDebugInterface
	typedef HRESULT (WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	// Load the DLL that contains the function DXGIGetDebugInterface
	const auto hModDXGIDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if(hModDXGIDebug == nullptr)
	{
		throw WND_LAST_EXCEPT();
	}

	// Get the address of DXGIGetDebugInterface in DLL
	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(hModDXGIDebug, "DXGIGetDebugInterface")));

	if(DxgiGetDebugInterface == nullptr)
	{
		throw WND_LAST_EXCEPT();
	}

	HRESULT hr;
	GFX_THROW_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &pDXGIInfoQueue));
}

void DXGIInfoManager::Set() noexcept
{
	// Set the index (next) so that the next all to GetMessages()
	// will only get errors generated after this call
	next = pDXGIInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL); 
}

std::vector<std::string> DXGIInfoManager::GetMessages() const
{
	std::vector<std::string> messages;
	const auto end = pDXGIInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

	for(auto i = next; i < end; i++)
	{
		HRESULT hr;
		SIZE_T messageLength = 0;

		// Ge the size of the message in bytes
		GFX_THROW_NOINFO(pDXGIInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));

		// Allocate memory for the message
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

		// Get the message and push its description into the vector
		GFX_THROW_NOINFO(pDXGIInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength));
		messages.emplace_back(pMessage->pDescription);
	}

	return messages;
}
