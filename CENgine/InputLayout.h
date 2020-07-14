#pragma once

#include "Bindable.h"
#include "Vertex.h"

namespace Bind
{
	class VertexShader;

	class InputLayout : public Bindable
	{
	public:
		
		InputLayout(Graphics& graphics, CENgineexp::VertexLayout layout_in, const VertexShader& vertexShader);
		void Bind(Graphics& graphics) NOXND override;
		const CENgineexp::VertexLayout GetLayout() const noexcept;
		static std::shared_ptr<InputLayout> Resolve(Graphics& graphics, const CENgineexp::VertexLayout& layout, const VertexShader& vertexShader);
		static std::string GenerateUID(const CENgineexp::VertexLayout& layout, const VertexShader& vertexShader);
		std::string GetUID() const noexcept override;
	protected:

		std::string vertexShaderUID;
		CENgineexp::VertexLayout layout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}