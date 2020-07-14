#include "VerticalBlurPass.h"
#include "Sink.h"
#include "Source.h"
#include "PixelShader.h"
#include "Blender.h"
#include "Stencil.h"
#include "Sampler.h"

namespace RGP
{
	VerticalBlurPass::VerticalBlurPass(std::string name, Graphics& graphics)
		:
		FullscreenPass(std::move(name), graphics)
	{
		AddBind(Bind::PixelShader::Resolve(graphics, "BlurOutline_PS.cso"));
		AddBind(Bind::Blender::Resolve(graphics, true));
		AddBind(Bind::Stencil::Resolve(graphics, Bind::Stencil::Mode::Mask));
		AddBind(Bind::Sampler::Resolve(graphics, Bind::Sampler::Type::Bilinear, true));

		AddBindSink<Bind::RenderTarget>("scratchIn");
		AddBindSink<Bind::DynamicCachingPixelConstantBuffer>("kernel");
		RegisterSink(DirectBindableSink<Bind::DynamicCachingPixelConstantBuffer>::Make("direction", direction));
		RegisterSink(DirectBufferSink<Bind::RenderTarget>::Make("renderTarget", renderTarget));
		RegisterSink(DirectBufferSink<Bind::DepthStencil>::Make("depthStencil", depthStencil));

		RegisterSource(DirectBufferSource<Bind::RenderTarget>::Make("renderTarget", renderTarget));
		RegisterSource(DirectBufferSource<Bind::DepthStencil>::Make("depthStencil", depthStencil));
	}

	// see the note on HorizontalBlurPass::Execute
	void VerticalBlurPass::Execute(Graphics& graphics) const NOXND
	{
		auto buf = direction->GetBuffer();
		buf["isHorizontal"] = false;
		direction->SetBuffer(buf);

		direction->Bind(graphics);
		FullscreenPass::Execute(graphics);
	}
}