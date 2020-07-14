#pragma once

#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"

namespace Bind 
{
	class VertexBuffer : public Bindable
	{
	public:

		VertexBuffer(Graphics& graphics, const CENgineexp::VertexBuffer& vbuf);
		VertexBuffer(Graphics& graphics, const std::string& tag, const CENgineexp::VertexBuffer& vbuf);
		void Bind(Graphics& graphics) NOXND override;
		const CENgineexp::VertexLayout& GetLayout() const noexcept;
		static std::shared_ptr<VertexBuffer> Resolve(Graphics& graphics, const std::string& tag, const CENgineexp::VertexBuffer& vbuf);

		template<typename... Ignore>
		static std::string GenerateUID(const std::string& tag, Ignore&&... ignore)
		{
			return GenerateUID_(tag);
		}

		std::string GetUID() const noexcept override;
	protected:

		std::string tag;
		UINT stride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
		CENgineexp::VertexLayout layout;
	private:

		static std::string GenerateUID_(const std::string& tag);
	};
}