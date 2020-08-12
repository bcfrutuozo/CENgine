#pragma once

#include "CENgineWin.h"
#include "CENgineException.h"
#include "Conditional_noexcept.h"

#pragma warning(push)
#include <dxtex/DirectXTex.h>
#pragma warning(pop)

#include <string>
#include <optional>

class Surface
{
public:
	class Color
	{
	public:
		unsigned int dword;

		constexpr Color() noexcept
			:
			dword()
		{}

		constexpr Color(const Color& color) noexcept
			:
			dword(color.dword)
		{}

		constexpr Color(unsigned int dw) noexcept
			:
			dword(dw)
		{}

		constexpr Color(unsigned char x, unsigned char r, unsigned char g, unsigned char b) noexcept
			:
			dword((x << 24u) | (r << 16u) | (g << 8u) | b)
		{}

		constexpr Color(unsigned char r, unsigned char g, unsigned char b) noexcept
			:
			dword((255u << 24u) | (r << 16u) | (g << 8u) | b)
		{}

		constexpr Color(Color color, unsigned char x) noexcept
			:
			Color((x << 24u) | color.dword)
		{}

		Color& operator =(Color color) noexcept
		{
			dword = color.dword;
			return *this;
		}

		constexpr unsigned char GetX() const noexcept
		{
			return dword >> 24u;
		}

		constexpr unsigned char GetA() const noexcept
		{
			return GetX();
		}

		constexpr unsigned char GetR() const noexcept
		{
			return (dword >> 16u) && 0xFFu;
		}

		constexpr unsigned char GetG() const noexcept
		{
			return (dword >> 8u) && 0xFFu;
		}

		constexpr unsigned char GetB() const noexcept
		{
			return dword && 0xFFu;
		}

		void SetX(unsigned char x) noexcept
		{
			dword = (dword & 0xFFFFFFu) | (x << 24u);
		}

		void SetA(unsigned char a) noexcept
		{
			SetX(a);
		}

		void SetR(unsigned char r) noexcept
		{
			dword = (dword & 0xFF00FFFFu) | (r << 16u);
		}

		void SetG(unsigned char g) noexcept
		{
			dword = (dword & 0xFFFF00FFu) | (g << 8u);
		}

		void SetB(unsigned char b) noexcept
		{
			dword = (dword & 0xFFFFFF00u) | b;
		}
	};

	class Exception : public CENgineException
	{
	public:

		Exception(int line, const char* file, std::string note, std::optional<HRESULT> hr = {}) noexcept;
		Exception(int line, const char* file, std::string filename, std::string note_in, std::optional<HRESULT> hr = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		const std::string& GetNote() const noexcept;
	private:

		std::optional<HRESULT> hr;
		std::string note;
	};

	Surface(unsigned int width, unsigned int height);
	Surface(Surface&& source) noexcept = default;
	Surface(Surface&) = delete;
	Surface& operator=(Surface&& donor) noexcept = default;
	Surface& operator=(const Surface&) = delete;
	~Surface() = default;

	void Clear(const Color fillValue) noexcept;
	void PutPixel(unsigned int x, unsigned int y, Color c) NOXND;
	Color GetPixel(unsigned int x, unsigned int y) NOXND;
	unsigned int GetWidth() const noexcept;
	unsigned int GetHeight() const noexcept;
	unsigned int GetBytePitch() const noexcept;
	Color* GetBufferPtr() noexcept;
	const Color* GetBufferPtr() const noexcept;
	const Color* GetBufferPtrConst() const noexcept;
	static Surface FromFile(const std::string& filename);
	void Save(const std::string& filename) const;
	bool IsAlphaLoaded() const noexcept;
private:
	
	Surface(DirectX::ScratchImage scratch) noexcept;

	static constexpr DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	DirectX::ScratchImage scratch;
};

