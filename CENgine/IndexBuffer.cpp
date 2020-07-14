#include "IndexBuffer.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind {

	IndexBuffer::IndexBuffer(Graphics& graphics, const std::vector<unsigned short>& indices)
		:
		IndexBuffer(graphics, "?", indices)
	{}
	
	IndexBuffer::IndexBuffer(Graphics& graphics, std::string tag, const std::vector<unsigned short>& indices)
		:
		tag(tag),
		count(static_cast<UINT>(indices.size()))
	{
		INFOMAN(graphics);

		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.ByteWidth = UINT(count * sizeof(unsigned short));
		ibd.StructureByteStride = sizeof(unsigned short);

		D3D11_SUBRESOURCE_DATA isrd = {};
		isrd.pSysMem = indices.data();

		GFX_THROW_INFO(GetDevice(graphics)->CreateBuffer(&ibd, &isrd, &pIndexBuffer));
	}

	void IndexBuffer::Bind(Graphics& graphics) NOXND
	{
		INFOMAN_NOHR(graphics);
		GFX_THROW_INFO_ONLY(GetContext(graphics)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u));
	}

	UINT IndexBuffer::GetCount() const noexcept
	{
		return count;
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Resolve(Graphics& graphics, const std::string& tag, const std::vector<unsigned short>& indices)
	{
		assert(tag != "?");
		return Codex::Resolve<IndexBuffer>(graphics, tag, indices);
	}
	
	std::string IndexBuffer::GenerateUID_(const std::string& tag)
	{
		using namespace std::string_literals;
		return typeid(IndexBuffer).name() + "#"s + tag;
	}

	std::string IndexBuffer::GetUID() const noexcept
	{
		return GenerateUID_(tag);
	}
}