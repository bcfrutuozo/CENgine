	#include "Bindable.h"
#include <stdexcept>

namespace Bind {
	
	ID3D11DeviceContext* Bindable::GetContext(Graphics& graphics) noexcept
	{
		return graphics.pContext.Get();
	}

	ID3D11Device* Bindable::GetDevice(Graphics& graphics) noexcept
	{
		return graphics.pDevice.Get();
	}

	DXGIInfoManager& Bindable::GetInfoManager(Graphics& graphics) NOXND
	{
#ifndef NDEBUG
		return graphics.infoManager;
#else
		throw std::logic_error("Cannot access graphics info manager in Release config.");
#endif	
	}
}