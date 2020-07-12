#pragma once

#include "Graphics.h"
#include "Conditional_noexcept.h"
#include "Color.h"

#include <vector>
#include <type_traits>
#include <assimp/scene.h>
#include <utility>

#define ELEMENT_AI_EXTRACTOR(member) static SysType Extract(const aiMesh& mesh, size_t i) noexcept { return *reinterpret_cast<const SysType*>(&mesh.member[i]); }

#define LAYOUT_ELEMENT_TYPES \
	X(Position2D) \
	X(Position3D) \
	X(Texture2D) \
	X(Normal) \
	X(Tangent) \
	X(Bitangent) \
	X(Float3Color) \
	X(Float4Color) \
	X(BGRAColor) \
	X(Count)

namespace CENgineexp
{
	class VertexLayout
	{
	public:

		enum ElementType
		{
			#define X(el) el,
			LAYOUT_ELEMENT_TYPES
			#undef X
		};

		template<ElementType> struct Map;

		template<> struct Map<Position2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P2";
			ELEMENT_AI_EXTRACTOR(mVertices)
		};

		template<> struct Map<Position3D>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P3";
			ELEMENT_AI_EXTRACTOR(mVertices)
		};

		template<> struct Map<Texture2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "Texcoord";
			static constexpr const char* code = "T2";
			ELEMENT_AI_EXTRACTOR(mTextureCoords[0])
		};

		template<> struct Map<Normal>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Normal";
			static constexpr const char* code = "N";
			ELEMENT_AI_EXTRACTOR(mNormals)
		};

		template<> struct Map<Tangent>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Tangent";
			static constexpr const char* code = "Nt";
			ELEMENT_AI_EXTRACTOR(mTangents)
		};

		template<> struct Map<Bitangent>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Bitangent";
			static constexpr const char* code = "Nb";
			ELEMENT_AI_EXTRACTOR(mBitangents)
		};

		template<> struct Map<Float3Color>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C3";
			ELEMENT_AI_EXTRACTOR(mColors[0])
		};

		template<> struct Map<Float4Color>
		{
			using SysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C4";
			ELEMENT_AI_EXTRACTOR(mColors[0])
		};

		template<> struct Map<BGRAColor>
		{
			using SysType = ::BGRAColor;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "C8";
			ELEMENT_AI_EXTRACTOR(mColors[0])
		};

		template<> struct Map<Count>
		{
			using SysType = long double;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;
			static constexpr const char* semantic = "!INVALID!";
			static constexpr const char* code = "!INV!";
			ELEMENT_AI_EXTRACTOR(mFaces)
		};

		// This template serve as a bridge between strong typed classes to dynamic runtime generated classes
		template<template<VertexLayout::ElementType> class F, typename... Args>
		static constexpr auto Bridge(VertexLayout::ElementType type, Args&&... args) NOXND
		{
			switch(type)
			{
				#define X(el) case VertexLayout::el: return F<VertexLayout::el>::Exec(std::forward<Args>(args)...);
				LAYOUT_ELEMENT_TYPES
					#undef X
			}
			assert("Invalid element type" && false);
			return F<VertexLayout::Count>::Exec(std::forward<Args>(args)...);
		}

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

			ElementType type;
			size_t offset;
		};

		template<ElementType Type>
		const Element& Resolve() const NOXND
		{
			for(auto& e : elements)
			{
				if(e.GetType() == Type)
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
		bool Has(ElementType type) const noexcept;
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
			VertexLayout::Bridge<AttributeSetting>(element.GetType(), this, pAttribute, std::forward<T>(val));
		}
	protected:

		Vertex(char* pData, const VertexLayout& layout) NOXND;
	private:

		// Necessary for Bridge to SetAttribute
		template<VertexLayout::ElementType type>
		struct AttributeSetting
		{
			template<typename T>
			static constexpr auto Exec(Vertex* pVertex, char* pAttribute, T&& val) NOXND
			{
				return pVertex->SetAttribute<type>(pAttribute, std::forward<T>(val));
			}
		};

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
			if constexpr(std::is_assignable<Dest, SourceType>::value)
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

		VertexBuffer(VertexLayout layout, size_t size = 0u) NOXND;
		VertexBuffer(VertexLayout layout_in, const aiMesh& mesh);
		const char* GetData() const NOXND;
		const VertexLayout& GetLayout() const noexcept;
		void Resize(size_t newSize) NOXND;
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

#undef ELEMENT_AI_EXTRACTOR
#ifndef SOURCE_FILE
#undef LAYOUT_ELEMENT_TYPES
#endif