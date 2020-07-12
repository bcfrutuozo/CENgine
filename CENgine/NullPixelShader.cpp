#include "NullPixelShader.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include "Utilities.h"

namespace Bind
{
	NullPixelShader::NullPixelShader(Graphics& graphics)
	{ }

	void NullPixelShader::Bind(Graphics& graphics) noexcept
	{
		GetContext(graphics)->PSSetShader(nullptr, nullptr, 0u);
	}

	std::shared_ptr<NullPixelShader> NullPixelShader::Resolve(Graphics& graphics)
	{
		return Codex::Resolve<NullPixelShader>(graphics);
	}

	std::string NullPixelShader::GenerateUID()
	{
		return typeid(NullPixelShader).name();
	}

	std::string NullPixelShader::GetUID() const noexcept
	{
		return GenerateUID();
	}
}