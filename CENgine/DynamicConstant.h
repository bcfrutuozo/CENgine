#pragma once

#include "Conditional_noexcept.h"

#include <cassert>
#include <vector>
#include <memory>
#include <optional>
#include <DirectXMath.h>
#include <string>

// Master list of leaf types that generates enum elements and various switches
#define LEAF_ELEMENT_TYPES \
	X(Float) \
	X(Float2) \
	X(Float3) \
	X(Float4) \
	X(Matrix) \
	X(Bool) \
	X(Integer)

// Namespace for dynamic runtime reflection
namespace DRR
{
	enum Type
	{
		#define X(el) el,
		LEAF_ELEMENT_TYPES
		#undef X
		Struct,
		Array,
		Empty,
	};

	// Static map of attributes of each leaf type
	template<Type type>
	struct Map
	{
		static constexpr bool valid = false;
	};

	template<> struct Map<Float>
	{
		using SysType = float;								// Type used on the CPU side
		static constexpr size_t hlslSize = sizeof(SysType); // Size of type on GPU side
		static constexpr const char* code = "F1";			// Code used for generating signature of layout
		static constexpr bool valid = true;					// Metaprogramming flag to check validity of Map <param>
	};

	template<> struct Map<Float2>
	{
		using SysType = DirectX::XMFLOAT2;					// Type used on the CPU side
		static constexpr size_t hlslSize = sizeof(SysType); // Size of type on GPU side
		static constexpr const char* code = "F2";			// Code used for generating signature of layout
		static constexpr bool valid = true;					// Metaprogramming flag to check validity of Map <param>
	};

	template<> struct Map<Float3>
	{
		using SysType = DirectX::XMFLOAT3;					// Type used on the CPU side
		static constexpr size_t hlslSize = sizeof(SysType); // Size of type on GPU side
		static constexpr const char* code = "F3";			// Code used for generating signature of layout
		static constexpr bool valid = true;					// Metaprogramming flag to check validity of Map <param>
	};

	template<> struct Map<Float4>
	{
		using SysType = DirectX::XMFLOAT4;					// Type used on the CPU side
		static constexpr size_t hlslSize = sizeof(SysType); // Size of type on GPU side
		static constexpr const char* code = "F4";			// Code used for generating signature of layout
		static constexpr bool valid = true;					// Metaprogramming flag to check validity of Map <param>
	};

	template<> struct Map<Matrix>
	{
		using SysType = DirectX::XMFLOAT4X4;				// Type used on the CPU side
		static constexpr size_t hlslSize = sizeof(SysType); // Size of type on GPU side
		static constexpr const char* code = "M4";			// Code used for generating signature of layout
		static constexpr bool valid = true;					// Metaprogramming flag to check validity of Map <param>
	};

	template<> struct Map<Bool>
	{
		using SysType = bool;								// Type used on the CPU side
		static constexpr size_t hlslSize = 4u;				// Size of type on GPU side
		static constexpr const char* code = "BL";			// Code used for generating signature of layout
		static constexpr bool valid = true;					// Metaprogramming flag to check validity of Map <param>
	};

	template<> struct Map<Integer>
	{
		using SysType = int;								// Type used on the CPU side
		static constexpr size_t hlslSize = sizeof(SysType);	// Size of type on GPU side
		static constexpr const char* code = "IN";			// Code used for generating signature of layout
		static constexpr bool valid = true;					// Metaprogramming flag to check validity of Map <param>
	};

	// Ensures that every leaf type in master list has an entry in the static attribute map
	#define X(el) static_assert(Map<el>::valid,"Missing map implementation for " #el);
	LEAF_ELEMENT_TYPES
		#undef X


		// Enables reverse lookup from SysType to leaf type
		template<typename T>
	struct ReverseMap
	{
		static constexpr bool valid = false;
	};

	#define X(el) \
	template<> struct ReverseMap<typename Map<el>::SysType> \
	{ \
		static constexpr Type type = el; \
		static constexpr bool valid = true; \
	};
	LEAF_ELEMENT_TYPES
		#undef X

		// LayoutElements instances form a tree that describes the layout of the data buffer
		// supporting nested aggregates of structs and arrays	
		class LayoutElement
	{
	public:

		// Get a string signature for this element (recursive) when called on the root
		// element of a layout tree, generates a uniquely-identifying string for the layout
		std::string GetSignature() const NOXND;

		// Check if element is real
		bool Exists() const noexcept;

		// Calculate array indexing offset
		std::pair<size_t, const LayoutElement*> CalculateIndexingOffset(size_t offset, size_t index) const NOXND;

		// [] only works for Structs; access member (child nodes in tree) by name
		LayoutElement& operator[](const std::string& key) NOXND;
		const LayoutElement& operator[](const std::string& key) const NOXND;

		// T() only works for Arrays; gets the array type layout object
		// Needed to further configure an Array's type
		LayoutElement& T() NOXND;
		const LayoutElement& T() const NOXND;

		// Offset based functions only after finalization!
		size_t GetOffsetBegin() const NOXND;
		size_t GetOffsetEnd() const NOXND;

		// Get size in bytes derived from offsets
		size_t GetSizeInBytes() const NOXND;

		// Only works for Structs; add LayoutElement to struct
		LayoutElement& Add(Type addedType, std::string name) NOXND;

		template<Type typeAdded>
		LayoutElement& Add(std::string key) NOXND
		{
			return Add(typeAdded, std::move(key));
		}

		// Only works for Arrays; set the type and the number of elements
		LayoutElement& Set(Type typeAdded, size_t size) NOXND;

		template<Type typeAdded>
		LayoutElement& Set(size_t size) NOXND
		{
			return Set(typeAdded, size);
		}

		// Returns offset of leaf types for read/write purposes with typecheck in Debug
		template<typename T>
		size_t Resolve() const NOXND
		{
			switch(type)
			{
				#define X(el) case el: assert(typeid(Map<el>::SysType) == typeid(T)); return *offset;
				LEAF_ELEMENT_TYPES
					#undef X
				default:
				assert("Tried to resolve non-leaf element" && false);
				return 0u;
			}
		}
	private:

		// This forms the polymorphic base for extra data that Struct and Array have
		struct ExtraDataBase
		{
			virtual ~ExtraDataBase() = default;
		};

		// Friend relationships are used liberally through the DynamicConstant system
		// instead of seeing the various classes in this system as encapsulated decoupled
		// units, they must be viewed as aspect of one large monolithic system
		// the reason for the friend relationships is generally so that intermediate
		// classes that the client should not create can have their constructors made
		// private, so that Finalize() cannot be called on arbitrary LayoutElements, etc.
		friend class IncompleteLayout;
		friend struct ExtraData;

		// Construct an empty layout element
		LayoutElement() noexcept = default;
		LayoutElement(Type typeIn) NOXND;

		// Sets all offsets for element and subelements, prepending padding when necessary
		// Returns offset directly after this element
		size_t Finalize(size_t offsetIn) NOXND;

		// Implementations for GetSignature for aggregate types
		std::string GetSignatureForStruct() const NOXND;
		std::string GetSignatureForArray() const NOXND;

		// Implementations for Finalize for aggregate types
		size_t FinalizeForStruct(size_t offsetIn);
		size_t FinalizeForArray(size_t offsetIn);

		// Returns singleton instance of empty layout element
		static LayoutElement& GetEmptyElement() noexcept
		{
			static LayoutElement empty{ };
			return empty;
		}

		// Returns the value of offset bumped up to the next 16-byte boundary (if not already on one)
		static size_t AdvanceToBoundary(size_t offset) noexcept;

		// Returns true if a memory block crosses a boundary
		static bool CrossesBoundary(size_t offset, size_t size) noexcept;

		// Advances an offset to next boundary if block crosses a boundary
		static size_t AdvanceIfCrossesBoundary(size_t offset, size_t size) noexcept;

		// Checks string for validity as a struct key
		static bool ValidateSymbolName(const std::string& name) noexcept;

		// Each element stores its own offset. This makes lookup to find its position in the byte buffer
		// fast. Special handling is required for situations where arrays are involved
		std::optional<size_t> offset;
		Type type = DRR::Type::Empty;
		std::unique_ptr<ExtraDataBase> pExtraData;
	};

	// The layout class serves as a shell to hold the root of the LayoutElement tree
	// client does not create LayoutElements directly, create a incomplete layout and
	// then use it to access the elements and add on from there. When building is done,
	// incomplete layout is moved to Codex (usually via Buffer::Make), and the internal
	// layout element tree is "delivered" (finalized and moved out). Codex returns a baked
	// layout, which the buffer can then use to initialize itself. Baked layout can
	// also be used to directly init multiple Buffers. Baked layouts are conceptually
	// immutable. Base Layout class cannot be constructed.
	class Layout
	{
		friend class LayoutCodex;
		friend class Buffer;

	public:

		size_t GetSizeInBytes() const noexcept;
		std::string GetSignature() const NOXND;
	protected:

		Layout(std::shared_ptr<LayoutElement> pRoot) noexcept;
		std::shared_ptr<LayoutElement> pRoot;
	};

	// Incomplete layout represents a layout that has not yet been finalized and
	// registered structure can be edited by adding layout nodes
	class IncompleteLayout : public Layout
	{
		friend class LayoutCodex;
	public:

		IncompleteLayout() noexcept;

		// Key into the root Struct
		LayoutElement& operator[](const std::string& key) NOXND;

		// Add an element to the root Struct
		template<Type type>
		LayoutElement& Add(const std::string& key) NOXND
		{
			return pRoot->Add<type>(key);
		}
	private:

		// Reset this object with an empty struct at its root
		void ClearRoot() noexcept;

		// Finalize the layout and then relinquish (by yielding the root layout element)
		std::shared_ptr<LayoutElement> DeliverRoot() noexcept;
	};

	// Complete layout represents a completed and registered layout shell object
	// layout tree is fixed
	class CompleteLayout : public Layout
	{
		friend class LayoutCodex;
		friend class Buffer;
	public:

		// Key into the root Struct (const to disable mutation of the layout)
		const LayoutElement& operator[](const std::string& key) const NOXND;

		// Get a share on layout tree root
		std::shared_ptr<LayoutElement> ShareRoot() const noexcept;
	private:

		// This constructor used by Codex to return complete layouts
		CompleteLayout(std::shared_ptr<LayoutElement> pRoot) noexcept;

		// Uses to pilfer the layout tree
		std::shared_ptr<LayoutElement> RelinquishRoot() const noexcept;
	};

	// Proxy type that is emitted when keying/indexing into a Buffer
	// implement conversions/assignment that allows manipulation of the
	// raw bytes of the Buffer. This version is const, only supports reading
	// Refs can be further keyed/indexed to traverse the layout structure
	class ConstElementRef
	{
		friend class Buffer;
		friend class ElementRef;
	public:

		// This is a proxy type emmited when you use addressof& on the Ref
		// it allows conversion to pointer type, useful for using Buffer
		// elements with ImGui widget functions etc.
		class Pointer
		{
			friend ConstElementRef;
		public:

			// Conversion for getting read-only pointer to supported SysType
			template<typename T>
			operator const T* () const NOXND
			{
				static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in pointer conversion");
				return &static_cast<const T&>(*ref);
			}
		private:

			Pointer(const ConstElementRef* ref) noexcept;
			const ConstElementRef* ref;
		};

		// Checks if the indexed element actually exists
		// this is possible because if you key into a Struct with a nonexistent key
		// it will still return an Empty LayoutElement that will enable this test
		// but will not enable any other kind of access
		bool Exists() const noexcept;

		// Key into the current element as a Struct
		ConstElementRef operator[](const std::string& key) const NOXND;

		// Index into the current element as an Array
		ConstElementRef operator[](size_t index) const NOXND;

		// Emits a pointer proxy object
		Pointer operator&() const NOXND;

		// Conversion for reading as a supported SysType
		template<typename T>
		operator const T& () const NOXND
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in conversion");
			return *reinterpret_cast<const T*>(pBytes + offset + pLayout->Resolve<T>());
		}
	private:

		// Refs should only be constructable by other refs or by the buffer
		ConstElementRef(const LayoutElement* pLayout, const char* pBytes, size_t offset) noexcept;

		// This offset is the offset that is built up by indexing into arrays
		// accumulated for every array index in the path of access into the structure
		size_t offset;
		const LayoutElement* pLayout;
		const char* pBytes;
	};

	// Version of ConstElementRef that also allows writing to the bytes of Buffer
	// see above in ConstElementRef for detailed description
	class ElementRef
	{
		friend class Buffer;
	public:

		class Pointer
		{
			friend ElementRef;

		public:

			// Conversion to read/write pointer to supported SysType
			template<typename T>
			operator T* () const NOXND
			{
				static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in pointer conversion");
				return &static_cast<T&>(*ref);
			}
		private:

			Pointer(ElementRef* ref) noexcept;
			ElementRef* ref;
		};

		operator ConstElementRef() const noexcept;
		bool Exists() const noexcept;
		ElementRef operator[](const std::string& key) const NOXND;
		ElementRef operator[](size_t index) const NOXND;

		// Optionally set value if not an empty Ref
		template<typename S>
		bool SetIfExists(const S& val) NOXND
		{
			if(Exists())
			{

				*this = val;
				return true;
			}

			return false;
		}

		Pointer operator&() const NOXND;

		// Conversion for reading/writing as a supported SysType
		template<typename T>
		operator T& () const NOXND
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in conversion");
			return *reinterpret_cast<T*>(pBytes + offset + pLayout->Resolve<T>());
		}

		// Assignment for writing to as a supported SysType
		template<typename T>
		T& operator=(const T& rhs) const NOXND
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::valid, "Unsupported SysType used in assignment");
			return static_cast<T&>(*this) = rhs;
		}
	private:

		// Refs should only be constructable by other refs or by the buffer
		ElementRef(const LayoutElement* pLayout, char* pBytes, size_t offset) noexcept;

		size_t offset;
		const LayoutElement* pLayout;
		char* pBytes;
	};

	// The buffer object is a combination of a raw byte buffer with a LayoutElement
	// tree structure which acts as an view/interpretation/overlay for those bytes
	// operator [] indexes into the root Struct, returning a Ref shell that can be
	// used to further index if struct/array, returning further Ref shells, or used
	// to access the data stored in the buffer if a Leaf element type
	class Buffer
	{
	public:

		// Various resources can be used to construct a buffer
		Buffer(IncompleteLayout&& layout) NOXND;
		Buffer(const CompleteLayout& layout) NOXND;
		Buffer(CompleteLayout&& layout) NOXND;
		Buffer(const Buffer&) noexcept;

		// Have to be careful with this one, where the buffer that has once
		// been pilfered must not be used
		Buffer(Buffer&&) noexcept;

		// How you begin indexing into buffer (root is always a Struct)
		ElementRef operator[](const std::string& key) NOXND;

		// If Buffer is const, you only get to index into the buffer with a
		// read-only proxy
		ConstElementRef operator[](const std::string& key) const NOXND;

		// Get the raw bytes
		const char* GetData() const noexcept;

		// Size of the raw byte buffer
		size_t GetSizeInBytes() const noexcept;
		const LayoutElement& GetRootLayoutElement() const noexcept;

		// Copy bytes from another buffer (layouts must match)
		void CopyFrom(const Buffer&) NOXND;

		// Returns another shared pointer to the layout root
		std::shared_ptr<LayoutElement> ShareLayoutRoot() const noexcept;
	private:

		std::shared_ptr<LayoutElement> pLayoutRoot;
		std::vector<char> bytes;
	};
}

#ifndef DRR_IMPL_SOURCE
#undef LEAF_ELEMENT_TYPES
#endif