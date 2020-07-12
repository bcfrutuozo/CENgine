#include "GraphicsResource.h"

#include <stdexcept>

ID3D11DeviceContext* GraphicsResource::GetContext(Graphics& graphics) noexcept
{
    return graphics.pContext.Get();
}

ID3D11Device* GraphicsResource::GetDevice(Graphics& graphics) noexcept
{
    return graphics.pDevice.Get();
}

DXGIInfoManager& GraphicsResource::GetInfoManager(Graphics& graphics)
{
    #ifndef NDEBUG
    return graphics.infoManager;
    #else
    throw std::logic_error("Error! Cannot access infoManager while in Release config");
    #endif
}
