#include "VertexBuffer.h"
#include "BindableCodex.h"

namespace Bind 
{
	VertexBuffer::VertexBuffer(Graphics& graphics,const CENgineexp::VertexBuffer& vbuf)
		:
		VertexBuffer(graphics, "?", vbuf)
	{}

	VertexBuffer::VertexBuffer(Graphics& graphics,  const std::string& tag, const CENgineexp::VertexBuffer& vbuf)
		:
		tag(tag),
		stride(static_cast<UINT>(vbuf.GetLayout().Size())),
		layout(vbuf.GetLayout())
	{
		INFOMAN(graphics);

		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT( vbuf.SizeBytes() );
		bd.StructureByteStride = stride;
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vbuf.GetData();
		GFX_THROW_INFO( GetDevice( graphics )->CreateBuffer( &bd,&sd,&pVertexBuffer ) );
	} 
	
	void VertexBuffer::Bind(Graphics& graphics) NOXND
	{
		const UINT offset = 0u;
		INFOMAN_NOHR(graphics);
		GFX_THROW_INFO_ONLY(GetContext(graphics)->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset));
	}

	const CENgineexp::VertexLayout& VertexBuffer::GetLayout() const noexcept
	{
		return layout;
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Resolve(Graphics& graphics, const std::string& tag, const CENgineexp::VertexBuffer& vbuf)
	{
		assert(tag != "?");
		return Codex::Resolve<VertexBuffer>(graphics, tag, vbuf);
	}

	std::string VertexBuffer::GenerateUID_(const std::string& tag)
	{
		using namespace std::string_literals;
		return typeid(VertexBuffer).name() + "#"s + tag;
	}

	std::string VertexBuffer::GetUID() const noexcept
	{
		return GenerateUID(tag);
	}
}
