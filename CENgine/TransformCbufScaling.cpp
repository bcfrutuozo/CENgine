#include "TransformCbufScaling.h"
#include "TechniqueProbe.h"

Bind::TransformCbufScaling::TransformCbufScaling(Graphics& graphics, float scale)
	:
	TransformCbuf(graphics),
	buffer(MakeLayout())
{
	buffer["scale"] = scale;
}

void Bind::TransformCbufScaling::Accept(TechniqueProbe& probe)
{
	probe.VisitBuffer(buffer);
}

void Bind::TransformCbufScaling::Bind(Graphics& graphics) NOXND
{
	const auto scale = buffer["scale"];
	const auto scaleMatrix = DirectX::XMMatrixScaling(scale, scale, scale);
	auto xf = GetTransforms(graphics);

	xf.modelView = xf.modelView * scaleMatrix;
	xf.modelViewProj = xf.modelViewProj * scaleMatrix;

	UpdateBindImpl(graphics, xf);
}

std::unique_ptr<Bind::CloningBindable> Bind::TransformCbufScaling::Clone() const noexcept
{
	return std::make_unique<TransformCbufScaling>(*this);
}

DRR::IncompleteLayout Bind::TransformCbufScaling::MakeLayout()
{
	DRR::IncompleteLayout layout;
	layout.Add<DRR::Type::Float>("scale");
	return layout;
}