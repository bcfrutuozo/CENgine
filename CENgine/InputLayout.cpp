#include "InputLayout.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include "Vertex.h"
#include "VertexShader.h"

namespace Bind
{
	InputLayout::InputLayout(Graphics& graphics,
		CENgineexp::VertexLayout layout_in,
		const VertexShader& vertexShader)
		:
		layout(std::move(layout_in))
	{
		INFOMAN(graphics);

		const auto d3dLayout = layout.GetD3DLayout();
		const auto pByteCode = vertexShader.GetByteCode();

		GFX_THROW_INFO(GetDevice(graphics)->CreateInputLayout(
			d3dLayout.data(), static_cast<UINT>(d3dLayout.size()),
			pByteCode->GetBufferPointer(),
			pByteCode->GetBufferSize(),
			&pInputLayout));
	}

	const CENgineexp::VertexLayout InputLayout::GetLayout() const noexcept
	{
		return layout;
	}

	void InputLayout::Bind(Graphics& graphics) NOXND
	{
		INFOMAN_NOHR(graphics);
		GFX_THROW_INFO_ONLY(GetContext(graphics)->IASetInputLayout(pInputLayout.Get()));
	}

	std::shared_ptr<InputLayout> InputLayout::Resolve(Graphics& graphics, const CENgineexp::VertexLayout& layout, const VertexShader& vertexShader)
	{
		return Codex::Resolve<InputLayout>(graphics, layout, vertexShader);
	}

	std::string InputLayout::GenerateUID(const CENgineexp::VertexLayout& layout, const VertexShader& vertexShader)
	{
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout.GetCode() + "#"s + vertexShader.GetUID();
	}

	std::string InputLayout::GetUID() const noexcept
	{
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout.GetCode() + "#"s + vertexShaderUID;
	}
}