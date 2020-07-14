#include "Rasterizer.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{

	Rasterizer::Rasterizer(Graphics& graphics, const bool isTwoSided)
		:
		isTwoSided(isTwoSided)
	{
		INFOMAN(graphics);

		D3D11_RASTERIZER_DESC rDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rDesc.CullMode = isTwoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK;

		GFX_THROW_INFO(GetDevice(graphics)->CreateRasterizerState(&rDesc, &pRasterizer));
	}

	void Rasterizer::Bind(Graphics& graphics) NOXND
	{
		INFOMAN_NOHR(graphics);
		GFX_THROW_INFO_ONLY(GetContext(graphics)->RSSetState(pRasterizer.Get()));
	}

	std::shared_ptr<Rasterizer> Rasterizer::Resolve(Graphics& graphics, const bool isTwoSided)
	{
		return Codex::Resolve<Rasterizer>(graphics, isTwoSided);
	}

	std::string Rasterizer::GenerateUID(const bool isTwoSided)
	{
		using namespace std::string_literals;

		return typeid(Rasterizer).name() + "#"s + (isTwoSided ? "2s" : "1s");
	}

	std::string Rasterizer::GetUID() const noexcept
	{
		return GenerateUID(isTwoSided);
	}
}