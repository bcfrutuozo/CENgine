#pragma once

#include "Bindable.h"
#include "BindableCodex.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	class Stencil : public Bindable
	{
	public:

		enum class Mode
		{
			Off,
			Write,
			Mask,
			DepthOff,
			DepthReversed
		};

		Stencil(Graphics& graphics, Mode mode)
			:
			mode(mode)
		{
			D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT { } };

			if(mode == Mode::Write)
			{
				dsDesc.DepthEnable = FALSE;
				dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
				dsDesc.StencilEnable = TRUE;
				dsDesc.StencilWriteMask = 0xFF;
				dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			}
			else if(mode == Mode::Mask)
			{
				dsDesc.DepthEnable = FALSE;
				dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
				dsDesc.StencilEnable = TRUE;
				dsDesc.StencilReadMask = 0xFF;
				dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
				dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			}
			else if(mode == Mode::DepthOff)
			{
				dsDesc.DepthEnable = FALSE;
				dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			}
			else if(mode == Mode::DepthReversed)
			{
				dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
			}

			GetDevice(graphics)->CreateDepthStencilState(&dsDesc, &pStencil);
		}

		void Bind(Graphics& graphics) NOXND override
		{
			INFOMAN_NOHR(graphics);
			GFX_THROW_INFO_ONLY(GetContext(graphics)->OMSetDepthStencilState(pStencil.Get(), 0xFF));
		}

		static std::shared_ptr<Stencil> Resolve(Graphics& graphics, Mode mode)
		{
			return Codex::Resolve<Stencil>(graphics, mode);
		}

		static std::string GenerateUID(Mode mode)
		{
			using namespace std::string_literals;

			const auto modeName = [&mode]()
			{
				switch(mode)
				{
					case Mode::Off:
					return "off"s;
					case Mode::Write:
					return "write"s;
					case Mode::Mask:
					return "mask"s;
					case Mode::DepthOff:
					return "depth-off"s;
					case Mode::DepthReversed:
					return "depth-reversed"s;

				}
				return "ERROR"s;
			};

			return typeid(Stencil).name() + "#"s + modeName();
		}

		std::string GetUID() const noexcept override
		{
			return GenerateUID(mode);
		}
	private:

		Mode mode;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pStencil;
	};
}