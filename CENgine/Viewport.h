#pragma once

#include "Bindable.h"
#include "BindableCodex.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	class Viewport : public Bindable
	{
	public:

		Viewport(Graphics& graphics)
			:
			Viewport(graphics, static_cast<float>(graphics.GetWidth()), static_cast<float>(graphics.GetHeight()))
		{}

		Viewport(Graphics& graphics, float width, float height)
		{
			m_Viewport.Width = width;
			m_Viewport.Height = height;
			m_Viewport.MinDepth = 0.0f;
			m_Viewport.MaxDepth = 1.0f;
			m_Viewport.TopLeftX = 0.0f;
			m_Viewport.TopLeftY = 0.0f;
		}

		void Bind(Graphics& graphics) NOXND override
		{
			INFOMAN_NOHR(graphics);
			GetContext(graphics)->RSSetViewports(1u, &m_Viewport);		
		}

	private:

		D3D11_VIEWPORT m_Viewport{};
	};
}