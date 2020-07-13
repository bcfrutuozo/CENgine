#include "FullscreenPass.h"
#include "BindableCommon.h"

FullscreenPass::FullscreenPass(const std::string name, Graphics& graphics) NOXND
	:
BindingPass(std::move(name))
{
	// Do a static initialization if required
	if(!pIndexBuffer)
	{
		// Setup fullscreen geometry
		CENgineexp::VertexLayout layout;
		layout.Append(CENgineexp::VertexLayout::Position2D);
		CENgineexp::VertexBuffer vBuf{ layout };
		vBuf.EmplaceBack(DirectX::XMFLOAT2{ -1, 1 });
		vBuf.EmplaceBack(DirectX::XMFLOAT2{ 1, 1 });
		vBuf.EmplaceBack(DirectX::XMFLOAT2{ -1, -1 });
		vBuf.EmplaceBack(DirectX::XMFLOAT2{ 1, -1 });
		pVertexBuffer = Bind::VertexBuffer::Resolve(graphics, "$Full", std::move(vBuf));
		std::vector<unsigned short> indices = { 0, 1, 2, 1, 3, 2 };
		pIndexBuffer = Bind::IndexBuffer::Resolve(graphics, "$Full", std::move(indices));

		// Setup fullscreen shaders
		pVertexShader = Bind::VertexShader::Resolve(graphics, "Fullscreen_VS.cso");
		pInputLayout = Bind::InputLayout::Resolve(graphics, layout, pVertexShader->GetByteCode());
	}
}

void FullscreenPass::Execute(Graphics& graphics) const noexcept
{
	BindAll(graphics);
	pVertexBuffer->Bind(graphics);
	pIndexBuffer->Bind(graphics);
	pVertexShader->Bind(graphics);
	pInputLayout->Bind(graphics);

	graphics.DrawIndexed(pIndexBuffer->GetCount());
}

std::shared_ptr<Bind::VertexBuffer> FullscreenPass::pVertexBuffer;
std::shared_ptr<Bind::IndexBuffer> FullscreenPass::pIndexBuffer;
std::shared_ptr<Bind::VertexShader> FullscreenPass::pVertexShader;
std::shared_ptr<Bind::InputLayout> FullscreenPass::pInputLayout;