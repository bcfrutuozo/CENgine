#pragma once

#include "Graphics.h"
#include "Conditional_noexcept.h"
#include "Color.h"

#include <vector>
#include <DirectXMath.h>
#include <type_traits>

namespace CENgineexp
{
	class VertexLayout
	{
	public:

		enum ElementType
		{
			Position2D,
			Position3D,
			Texture2D,
			Normal,
			Float3Color,
			Float4Color,
			BGRAColor,
			Count,
		};

		template<ElementType> struct Map;

		template<> struct Map<Position2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P2";
		};

		template<> struct Map<Position3D>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P3";
		};

		template<> struct Map<Texture2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Texcoord";
			static constexpr const char* code = "T2";
		};

		template<> struct Map<Normal>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Normal";
			static constexpr const char* code = "N";
		};

		template<> struct Map<Float3Color>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C3";
		};

		template<> struct Map<Float4Color>
		{
			using SysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C4";
		};

		template<> struct Map<BGRAColor>
		{
			using SysType = ::BGRAColor;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C8";
		};

		class Element
		{
		public:

			Element(ElementType type, size_t offset);
			size_t GetOffsetAfter() const NOXND;
			size_t GetOffset() const NOXND;
			size_t Size() const NOXND;
			static constexpr size_t SizeOf(ElementType type) NOXND;
			ElementType GetType() const noexcept;
			D3D11_INPUT_ELEMENT_DESC GetDescriptor() const NOXND;
			const char* GetCode() const noexcept;
		private:

			template<ElementType type>
			static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDescriptor(size_t offset) noexcept
			{
				return { Map<type>::semantic, 0, Map<type>::dxgiFormat, 0, static_cast<UINT>(offset), D3D11_INPUT_PER_VERTEX_DATA, 0 };
			}

			ElementType type;
			size_t offset;
		};

		template<ElementType Type>
		const Element& Resolve() const NOXND
		{
			for (auto& e : elements)
			{
				if (e.GetType() == Type)
				{
					return e;
				}
			}

			assert("Could not resolve element type" && false);
			return elements.front();
		}

		const Element& ResolveByIndex(size_t i) const NOXND;
		VertexLayout& Append(ElementType type) NOXND;
		size_t Size() const NOXND;
		size_t GetElementCount() const noexcept;
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const NOXND;
		std::string GetCode() const NOXND;
	private:

		std::vector<Element> elements;
	};

	class Vertex
	{
		friend class VertexBuffer;
	public:

		template<VertexLayout::ElementType Type>
		auto& Attr() const NOXND
		{
			auto pAttribute = pData + layout.Resolve<Type>().GetOffset();
			return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>(pAttribute);
		}

		template<typename T>
		void SetAttributeByIndex(const size_t i, T&& val) NOXND
		{
			const auto& element = layout.ResolveByIndex(i);
			auto pAttribute = pData + element.GetOffset();

			switch (element.GetType())
			{
			case VertexLayout::Position2D:
				SetAttribute<VertexLayout::Position2D>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Position3D:
				SetAttribute<VertexLayout::Position3D>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Texture2D:
				SetAttribute<VertexLayout::Texture2D>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Normal:
				SetAttribute<VertexLayout::Normal>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Float3Color:
				SetAttribute<VertexLayout::Float3Color>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::Float4Color:
				SetAttribute<VertexLayout::Float4Color>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::BGRAColor:
				SetAttribute<VertexLayout::BGRAColor>(pAttribute, std::forward<T>(val));
				break;
			default:
				assert("Bad element type" && false);
			}
		}
	protected:

		Vertex(char* pData, const VertexLayout& layout) NOXND;
	private:

		// Enables parameter pack setting of multiple parameters by element index
		template<typename First, typename... Rest>
		void SetAttributeByIndex(size_t i, First&& first, Rest&&... rest) NOXND
		{
			SetAttributeByIndex(i, std::forward<First>(first));
			SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
		}

		// Helper to reduce code duplication in SetAttributeByIndex
		template<VertexLayout::ElementType DestLayoutType, typename SourceType>
		void SetAttribute(char* pAttribute, SourceType&& val) NOXND
		{
			using Dest = typename VertexLayout::Map<DestLayoutType>::SysType;
			if constexpr (std::is_assignable<Dest, SourceType>::value)
			{
				*reinterpret_cast<Dest*>(pAttribute) = val;
			}
			else
			{
				assert("Parameter attribute type mismatch" && false);
			}
		}

		char* pData;
		const VertexLayout& layout;
	};

	class ConstVertex
	{
	public:

		ConstVertex(const Vertex& vertex) NOXND;

		template<VertexLayout::ElementType Type>
		const auto& Attr() const NOXND
		{
			return const_cast<Vertex&>(vertex).Attr<Type>();
		}
	private:

		Vertex vertex;
	};

	class VertexBuffer
	{
	public:

		VertexBuffer(VertexLayout layout) NOXND;
		const char* GetData() const NOXND;
		const VertexLayout& GetLayout() const noexcept;
		size_t Size() const NOXND;
		size_t SizeBytes() const NOXND;

		template<typename ...Params>
		void EmplaceBack(Params&&... params) NOXND
		{
			assert(sizeof... (params) == layout.GetElementCount() && "Param count doesn't match number of vertex elements");
			buffer.resize(buffer.size() + layout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
		}

		Vertex Back() NOXND;
		Vertex Front() NOXND;
		Vertex operator[](size_t i) NOXND;
		ConstVertex Back() const NOXND;
		ConstVertex Front() const NOXND;
		ConstVertex operator[](size_t i) const NOXND;
	private:

		std::vector<char> buffer;
		VertexLayout layout;
	};
}
