#pragma once

#include "Bindable.h"

namespace Bind
{
	class IndexBuffer : public Bindable
	{
	public:
		IndexBuffer(Graphics& graphics, const std::vector<unsigned short>& indices);
		IndexBuffer(Graphics& graphics, std::string tag, const std::vector<unsigned short>& indices);
		void Bind(Graphics& graphics) noexcept override;
		UINT GetCount() const noexcept;
		static std::shared_ptr<IndexBuffer> Resolve(Graphics& graphics, const std::string& tag, const std::vector<unsigned short>& indices);

		template<typename... Ignore>
		static std::string GenerateUID(const std::string& tag, Ignore&&... ignore)
		{
			return GenerateUID_(tag);
		}

		std::string GetUID() const noexcept override;
		
	protected:

		std::string& tag;
		UINT count;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	private:

		static std::string GenerateUID_(const std::string& tag);
	};
}