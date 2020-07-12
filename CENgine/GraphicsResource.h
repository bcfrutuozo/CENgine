#pragma once

#include "Graphics.h"

class Graphics;

class GraphicsResource
{ 
protected:

	static ID3D11DeviceContext* GetContext(Graphics& graphics) noexcept;
	static ID3D11Device* GetDevice(Graphics& graphics) noexcept;
	static DXGIInfoManager& GetInfoManager(Graphics& graphics);
};

