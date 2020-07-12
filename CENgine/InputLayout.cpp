#include "InputLayout.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include "Vertex.h"

namespace Bind
{
	InputLayout::InputLayout(Graphics& graphics,
		CENgineexp::VertexLayout layout_in,
		ID3DBlob* pVertexShaderByteCode)
		:
		layout(std::move(layout_in))
	{
		INFOMAN(graphics);

		const auto d3dLayout = layout.GetD3DLayout();

		GFX_THROW_INFO(GetDevice(graphics)->CreateInputLayout(
			d3dLayout.data(), static_cast<UINT>(d3dLayout.size()),
			pVertexShaderByteCode->GetBufferPointer(),
			pVertexShaderByteCode->GetBufferSize(),
			&pInputLayout));
	}

	const CENgineexp::VertexLayout InputLayout::GetLayout() const noexcept
	{
		return layout;
	}

	void InputLayout::Bind(Graphics& graphics) noexcept
	{
		GetContext(graphics)->IASetInputLayout(pInputLayout.Get());
	}

	std::shared_ptr<InputLayout> InputLayout::Resolve(Graphics& graphics, const CENgineexp::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode)
	{
		return Codex::Resolve<InputLayout>(graphics, layout, pVertexShaderByteCode);
	}

	std::string InputLayout::GenerateUID(const CENgineexp::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode)
	{
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout.GetCode();
	}

	std::string InputLayout::GetUID() const noexcept
	{
		return GenerateUID(layout);
	}
}