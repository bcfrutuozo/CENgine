#include "HorizontalBlurPass.h"
#include "PixelShader.h"
#include "RenderTarget.h"
#include "Sink.h"
#include "Source.h"
#include "Blender.h"
#include "Sampler.h"
#include "DynamicConstantBuffer.h"

namespace RGP
{
	HorizontalBlurPass::HorizontalBlurPass(std::string name, Graphics& graphics, unsigned int fullWidth, unsigned int fullHeight)
		:
		FullscreenPass(std::move(name), graphics)
	{
		AddBind(Bind::PixelShader::Resolve(graphics, "BlurOutline_PS.cso"));
		AddBind(Bind::Blender::Resolve(graphics, false));
		AddBind(Bind::Sampler::Resolve(graphics, Bind::Sampler::Type::Point, true));

		AddBindSink<Bind::RenderTarget>("scratchIn");
		AddBindSink<Bind::DynamicCachingPixelConstantBuffer>("kernel");
		RegisterSink(DirectBindableSink<Bind::DynamicCachingPixelConstantBuffer>::Make("direction", direction));

		// the renderTarget is internally sourced and then exported as a Bindable
		renderTarget = std::make_shared<Bind::ShaderInputRenderTarget>(graphics, fullWidth / 2, fullHeight / 2, 0u);
		RegisterSource(DirectBindableSource<Bind::RenderTarget>::Make("scratchOut", renderTarget));
	}

	// this override is necessary because we cannot (yet) link input bindables directly into
	// the container of bindables (mainly because vector growth buggers references)
	void HorizontalBlurPass::Execute(Graphics& graphics) const NOXND
	{
		auto buf = direction->GetBuffer();
		buf["isHorizontal"] = true;
		direction->SetBuffer(buf);

		direction->Bind(graphics);
		FullscreenPass::Execute(graphics);
	}
}