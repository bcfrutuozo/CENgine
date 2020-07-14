#include "FullscreenPass.h"
#include "BindableCommon.h"

namespace RGP
{
	FullscreenPass::FullscreenPass(const std::string name, Graphics& graphics) NOXND
		:
	BindingPass(std::move(name))
	{
		// Setup fullscreen geometry
		CENgineexp::VertexLayout layout;
		layout.Append(CENgineexp::VertexLayout::Position2D);
		CENgineexp::VertexBuffer vBuf{ layout };
		vBuf.EmplaceBack(DirectX::XMFLOAT2{ -1, 1 });
		vBuf.EmplaceBack(DirectX::XMFLOAT2{ 1, 1 });
		vBuf.EmplaceBack(DirectX::XMFLOAT2{ -1, -1 });
		vBuf.EmplaceBack(DirectX::XMFLOAT2{ 1, -1 });
		AddBind(Bind::VertexBuffer::Resolve(graphics, "$Full", std::move(vBuf)));
		std::vector<unsigned short> indices = { 0, 1, 2, 1, 3, 2 };
		AddBind(Bind::IndexBuffer::Resolve(graphics, "$Full", std::move(indices)));

		// Setup other common fullscreen bindables
		auto vs = Bind::VertexShader::Resolve(graphics, "Fullscreen_VS.cso");
		AddBind(Bind::InputLayout::Resolve(graphics, layout, *vs));
		AddBind(std::move(vs));
		AddBind(Bind::Topology::Resolve(graphics));
		AddBind(Bind::Rasterizer::Resolve(graphics, false));
	}

	void FullscreenPass::Execute(Graphics& graphics) const NOXND
	{
		BindAll(graphics);
		graphics.DrawIndexed(6u);
	}
}