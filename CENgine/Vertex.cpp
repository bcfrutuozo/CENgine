#include "Vertex.h"

namespace CENgineexp
{
	// VertexLayout
	const VertexLayout::Element& VertexLayout::ResolveByIndex(size_t i) const NOXND
	{
		return elements[i];
	}

	VertexLayout& VertexLayout::Append(ElementType type) NOXND
	{
		elements.emplace_back(type, Size());
		return *this;
	}

	size_t VertexLayout::Size() const NOXND
	{
		return elements.empty() ? 0u : elements.back().GetOffsetAfter();
	}

	size_t VertexLayout::GetElementCount() const noexcept
	{
		return elements.size();
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> VertexLayout::GetD3DLayout() const NOXND
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
		desc.reserve(GetElementCount());
		for (const auto& e : elements)
		{
			desc.push_back(e.GetDescriptor());
		}
		return desc;
	}

	// VertexLayout::Element
	VertexLayout::Element::Element(ElementType type, size_t offset)
		:
		type(type),
		offset(offset)
	{}

	size_t VertexLayout::Element::GetOffsetAfter() const NOXND
	{
		return offset + Size();
	}

	size_t VertexLayout::Element::GetOffset() const
	{
		return offset;
	}

	size_t VertexLayout::Element::Size() const NOXND
	{
		return SizeOf(type);
	}

	constexpr size_t VertexLayout::Element::SizeOf(ElementType type) NOXND
	{
		switch (type)
		{
		case Position2D:
			return sizeof(Map<Position2D>::SysType);
		case Position3D:
			return sizeof(Map<Position3D>::SysType);
		case Texture2D:
			return sizeof(Map<Texture2D>::SysType);
		case Normal:
			return sizeof(Map<Normal>::SysType);
		case Float3Color:
			return sizeof(Map<Float3Color>::SysType);
		case Float4Color:
			return sizeof(Map<Float4Color>::SysType);
		case BGRAColor:
			return sizeof(Map<BGRAColor>::SysType);
		}

		assert("Invalid element type" && false);
		return 0u;
	}

	VertexLayout::ElementType VertexLayout::Element::GetType() const noexcept
	{
		return type;
	}

	D3D11_INPUT_ELEMENT_DESC VertexLayout::Element::GetDescriptor() const noexcept(!IS_DEBUG)
	{
		switch (type)
		{
		case Position2D:
			return GenerateDescriptor<Position2D>(GetOffset());
		case Position3D:
			return GenerateDescriptor<Position3D>(GetOffset());
		case Texture2D:
			return GenerateDescriptor<Texture2D>(GetOffset());
		case Normal:
			return GenerateDescriptor<Normal>(GetOffset());
		case Float3Color:
			return GenerateDescriptor<Float3Color>(GetOffset());
		case Float4Color:
			return GenerateDescriptor<Float4Color>(GetOffset());
		case BGRAColor:
			return GenerateDescriptor<BGRAColor>(GetOffset());
		}
		assert("Invalid element type" && false);
		return { "Invalid", 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	}

	// Vertex
	Vertex::Vertex(char* pData, const VertexLayout& layout) NOXND
		:
	pData(pData),
		layout(layout)
	{
		assert(pData != nullptr);
	}

	ConstVertex::ConstVertex(const Vertex& vertex) NOXND
		:
	vertex(vertex)
	{}

	// VertexBuffer
	VertexBuffer::VertexBuffer(VertexLayout layout) NOXND
		:
		layout(std::move(layout))
	{}

	const char* VertexBuffer::GetData() const NOXND
	{
		return buffer.data();
	}

	const VertexLayout& VertexBuffer::GetLayout() const noexcept
	{
		return layout;
	}

	size_t VertexBuffer::Size() const NOXND
	{
		return buffer.size() / layout.Size();
	}

	size_t VertexBuffer::SizeBytes() const NOXND
	{
		return buffer.size();
	}

	Vertex VertexBuffer::Back() noexcept(!IS_DEBUG)
	{
		assert(buffer.size() != 0u);
		return Vertex{ buffer.data() + buffer.size() - layout.Size(), layout };
	}

	Vertex VertexBuffer::Front() noexcept(!IS_DEBUG)
	{
		assert(buffer.size() != 0u);
		return Vertex{ buffer.data(), layout };
	}

	Vertex VertexBuffer::operator[](size_t i) NOXND
	{
		assert(i < Size());
		return Vertex{ buffer.data() + layout.Size() * i, layout };
	}

	ConstVertex VertexBuffer::Back() const NOXND
	{
		return const_cast<VertexBuffer*>(this)->Back();
	}

	ConstVertex VertexBuffer::Front() const noexcept(!IS_DEBUG)
	{
		return const_cast<VertexBuffer*>(this)->Front();
	}

	ConstVertex VertexBuffer::operator[](size_t i) const noexcept(!IS_DEBUG)
	{
		return const_cast<VertexBuffer&>(*this)[i];
	}
}