#include "VertexShader.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

#include <typeinfo>

namespace Bind {

	VertexShader::VertexShader(Graphics& graphics, const std::string& path)
		:
		path(path)
	{
		INFOMAN(graphics);

		GFX_THROW_INFO(D3DReadFileToBlob(std::wstring{ path.begin(), path.end() }.c_str(), &pByteCodeBlob));
		GFX_THROW_INFO(GetDevice(graphics)->CreateVertexShader(
			pByteCodeBlob->GetBufferPointer(),
			pByteCodeBlob->GetBufferSize(),
			nullptr,
			&pVertexShader));
	}

	void VertexShader::Bind(Graphics& graphics) noexcept
	{
		GetContext(graphics)->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	}

	ID3DBlob* VertexShader::GetByteCode() const noexcept
	{
		return pByteCodeBlob.Get();
	}

	std::shared_ptr<VertexShader> VertexShader::Resolve(Graphics& graphics, const std::string& path)
	{
		return Codex::Resolve<VertexShader>(graphics, path);
	}

	std::string VertexShader::GenerateUID(const std::string& path)
	{
		using namespace std::string_literals;
		return typeid(VertexShader).name() + "#"s + path;
	}

	std::string VertexShader::GetUID() const noexcept
	{
		return GenerateUID(path);
	}
}