#include "Blender.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	Blender::Blender(Graphics& graphics, bool isBlending, std::optional<float> factors_in)
		:
		isBlending(isBlending)
	{
		INFOMAN(graphics);

		if(factors_in)
		{
			factors.emplace();
			factors->fill(*factors_in);
		}

		D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{}};
		auto& brt = blendDesc.RenderTarget[0];

		if (isBlending)
		{
			brt.BlendEnable = TRUE;
			
			if(factors_in)
			{
				brt.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
				brt.DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
			}
			else
			{
				brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
				brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			}
		}

		GFX_THROW_INFO(GetDevice(graphics)->CreateBlendState(&blendDesc, &pBlender));
	}

	void Blender::Bind(Graphics& graphics) noexcept
	{
		const float* data = factors ? factors->data() : nullptr;
		GetContext(graphics)->OMSetBlendState(pBlender.Get(), nullptr, 0xFFFFFFFFu);
	}

	void Blender::SetFactor(const float factor) NOXND
	{
		assert(factors);
		return factors->fill(factor);
	}

	float Blender::GetFactor() const NOXND
	{
		assert(factors);
		return factors->front();
	}

	std::shared_ptr<Blender> Blender::Resolve(Graphics& graphics, bool isBlending, std::optional<float> factor)
	{
		return Codex::Resolve<Blender>(graphics, isBlending, factor);
	}

	std::string Blender::GenerateUID(bool isBlending, std::optional<float> factor)
	{
		using namespace std::string_literals;
		return typeid(Blender).name() + "#"s + (isBlending ? "b"s : "n"s) + (factor ? "#f"s + std::to_string(*factor) : "");
	}

	std::string Blender::GetUID() const noexcept
	{
		return GenerateUID(isBlending, factors ? factors->front() : std::optional<float>{});
	}
}