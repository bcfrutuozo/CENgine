#pragma once

#include "ConstantBuffer.h"
#include "Drawable.h"
#include <DirectXMath.h>

class Camera;

namespace Bind
{
	class SkyboxTransformCbuf : public Bindable
	{
	public:

		SkyboxTransformCbuf(Graphics& graphics, UINT slot = 0u);
		void Bind(Graphics& graphics) NOXND override;
	protected:

		struct Transforms
		{
			DirectX::XMMATRIX viewProj;
		};

		void UpdateBindImpl(Graphics& graphics, const Transforms& tf) NOXND;
		Transforms GetTransforms(Graphics& graphics) NOXND;
	private:

		std::unique_ptr<VertexConstantBuffer<Transforms>> pVertexConstantBuffer;
	};
}