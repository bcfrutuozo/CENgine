#define FULL_WINTARD

#include "Surface.h"

#include <algorithm>
#include <gdiplus.h>
#include <sstream>

#pragma comment(lib, "gdiplus.lib")

Surface::Surface(const unsigned width, const unsigned height) noexcept
	:
	pBuffer(std::make_unique<Color[]>(width * height)),
	width(width),
	height(height)
{}

Surface& Surface::operator=(Surface&& donor) noexcept
{
	width = donor.width;
	height = donor.height;
	pBuffer = std::move(donor.pBuffer);
	donor.pBuffer = nullptr;
	return *this;
}

Surface::Surface(Surface&& source) noexcept
	:
	pBuffer(std::move(source.pBuffer)),
	width(source.width),
	height(source.height)
{}

Surface::~Surface()
{}

void Surface::Clear(const Color color) noexcept
{
	memset(pBuffer.get(), color.dword, width * height * sizeof(Color));
}

void Surface::PutPixel(const unsigned x, const unsigned y, const Color c) noexcept(!IS_DEBUG)
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);

	pBuffer[y * width + x] = c;
}

Surface::Color Surface::GetPixel(unsigned x, unsigned y) const noexcept(!IS_DEBUG)
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < width);
	assert(y < height);

	return pBuffer[y * width + x];
}

unsigned Surface::GetWidth() const noexcept
{
	return width;
}

unsigned Surface::GetHeight() const noexcept
{
	return height;
}

Surface::Color* Surface::GetBufferPtr() noexcept
{
	return pBuffer.get();
}

const Surface::Color* Surface::GetBufferPtr() const noexcept
{
	return pBuffer.get();
}

const Surface::Color* Surface::GetBufferPtrConst() const noexcept
{
	return pBuffer.get();
}

Surface Surface::FromFile(const std::string& filename)
{
	unsigned int width = 0;
	unsigned int height = 0;
	std::unique_ptr<Color[]> pBuffer;

	{
		// Convert filename to wide string (for GDI+)
		wchar_t fullname[512];
		mbstowcs_s(nullptr, fullname, filename.c_str(), _TRUNCATE);

		Gdiplus::Bitmap bitmap(fullname);
		if(bitmap.GetLastStatus() != Gdiplus::Status::Ok)
		{
			std::stringstream ss;
			ss << "Loading image [" << filename << "]: failed to load.";
			throw Exception( __LINE__, __FILE__, ss.str());
		}

		width = bitmap.GetWidth();
		height = bitmap.GetHeight();
		pBuffer = std::make_unique<Color[]>(width * height);

		for(unsigned int y = 0; y < height; y++)
		{
			for(unsigned int x = 0; x < width; x++)
			{
				Gdiplus::Color c;
				bitmap.GetPixel(x, y, &c);
				pBuffer[y * width + x] = c.GetValue();
			}
		}
	}

	return Surface(width, height, std::move(pBuffer));
}

void Surface::Save(const std::string& filename) const
{
	auto GetEncoderClsid = [&filename](const WCHAR* format, CLSID* pClsid) -> void
	{
		UINT num = 0;	// Number of image encoders
		UINT size = 0;	// Size of the image encoder array in bytes

		Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;

		Gdiplus::GetImageEncodersSize(&num, &size);
		if(size == 0)
		{
			std::stringstream ss;
			ss << "Saving surface [" << filename <<"]: failed to get encoder; size == 0.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}

		pImageCodecInfo = static_cast<Gdiplus::ImageCodecInfo*>(malloc(size));
		if(pImageCodecInfo == nullptr)
		{
			std::stringstream ss;
			ss << "Saving surface to []: failed to get encoder; fail to allocate memory.";
			throw Exception(__LINE__, __FILE__, ss.str());
		}

		GetImageEncoders(num, size, pImageCodecInfo);

		for(UINT j = 0; j < num; ++j)
		{

			if(wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return;
			}
		}

		free(pImageCodecInfo);
		std::stringstream ss;
		ss << "Saving surface to [" << filename << "]: failed to get encoder; failed to find matching encoder.";
		throw Exception(__LINE__, __FILE__, ss.str());
	};

	CLSID bmpID;

	GetEncoderClsid(L"image/bmp", &bmpID);

	// Convert filename to wide string (for GDI+)
	wchar_t path[512];
	mbstowcs_s(nullptr, path, filename.c_str(), _TRUNCATE);

	Gdiplus::Bitmap bitmap(width, height, width * sizeof(Color), PixelFormat32bppARGB, (BYTE*)pBuffer.get());
	if(bitmap.Save(path, &bmpID, nullptr) != Gdiplus::Status::Ok)
	{
		std::stringstream ss;
		ss << "Saving surface to [" << filename << "]: failed to save.";
		throw Exception(__LINE__, __FILE__, ss.str());
	}
}

void Surface::Copy(const Surface& source) noexcept(!IS_DEBUG)
{
	assert(width == source.width);
	assert(height == source.height);
	memcpy(pBuffer.get(), source.pBuffer.get(), width * height * sizeof(Color));
}

Surface::Surface(unsigned width, unsigned height, std::unique_ptr<Color[]> pBufferParameters) noexcept
	:
	width(width),
	height(height),
	pBuffer(std::move(pBufferParameters))
{}


// Surface exceptions
Surface::Exception::Exception(int line, const char* file, std::string note) noexcept
	:
	CENgineException(line, file),
	note(std::move(note))
{}

const char* Surface::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << "CENgineException::what()" << std::endl
		<< "[Note] " << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Surface::Exception::GetType() const noexcept
{
	return "CENgine Graphics Exception";
}

const std::string& Surface::Exception::GetNote() const noexcept
{
	return note;
}
