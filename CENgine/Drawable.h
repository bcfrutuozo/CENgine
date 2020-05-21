#pragma once

#include "Graphics.h"
#include "Conditional_noexcept.h"
#include <DirectXMath.h>

namespace Bind
{
	class Bindable;
	class IndexBuffer;
}

class Drawable
{
	template<class T>
	friend class DrawableBase;

public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& graphics) const NOXND;
	virtual void Update(float dt) noexcept
	{};
	virtual ~Drawable() = default;
protected:

	template<class T>
	T* QueryBindable() noexcept
	{
		for (auto& pb : binds)
		{
			if (auto pt = dynamic_cast<T*>(pb.get()))
			{
				return pt;
			}
		}
		return nullptr;
	}

	void AddBind(std::unique_ptr<Bind::Bindable> bind) NOXND;
	void AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer> iBuffer) NOXND;

private:
	virtual const std::vector<std::unique_ptr<Bind::Bindable>>& GetStaticBinds() const noexcept = 0;

	const Bind::IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bind::Bindable>> binds;
};

