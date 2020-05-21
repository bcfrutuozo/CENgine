#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"

#include <cassert>

using namespace Bind;

void Drawable::Draw(Graphics& graphics) const NOXND
{
	for(auto& b: binds)
	{
		b->Bind(graphics);
	}

	for(auto& b : GetStaticBinds())
	{
		b->Bind(graphics);
	}

	graphics.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) NOXND
{
	assert( "Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(Bind::IndexBuffer));
	binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> iBuffer) NOXND
{
	assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
	pIndexBuffer = iBuffer.get();
	binds.push_back(std::move(iBuffer));
}
