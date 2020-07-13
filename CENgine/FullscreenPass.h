#pragma once

#include "BindingPass.h"

namespace Bind
{
	class VertexBuffer;
	class IndexBuffer;
	class VertexShader;
	class InputLayout;
}

class FullscreenPass : public BindingPass
{
public:

	FullscreenPass(const std::string name, Graphics& graphics) NOXND;
	void Execute(Graphics& graphics) const noexcept override;
private:

	static std::shared_ptr<Bind::VertexBuffer> pVertexBuffer;
	static std::shared_ptr<Bind::IndexBuffer> pIndexBuffer;
	static std::shared_ptr<Bind::VertexShader> pVertexShader;
	static std::shared_ptr<Bind::InputLayout> pInputLayout;
};

