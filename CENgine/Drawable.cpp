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
	graphics.DrawIndexed(pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::shared_ptr<Bindable> bind) NOXND
{
	// Special case for index buffer
	if(typeid(*bind) == typeid(IndexBuffer))
	{
		assert("Binding multiple index buffers are not allowed" && pIndexBuffer == nullptr);
		pIndexBuffer = &static_cast<IndexBuffer&>(*bind);
	}
	binds.push_back(std::move(bind));
}