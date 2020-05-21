#pragma once

#include "ConstantBuffer.h"
#include "Drawable.h"
#include "DirectXMath.h"

namespace Bind {

	class TransformCbuf : public Bindable
	{
	public:

		struct Transforms
		{
			DirectX::XMMATRIX modelViewProj;
			DirectX::XMMATRIX model;
		};

		TransformCbuf(Graphics& graphics, const Drawable& parent, UINT slot = 0u);
		void Bind(Graphics& graphics) noexcept override;

	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pVertexConstantBuffer;
		const Drawable& parent;
	};
}