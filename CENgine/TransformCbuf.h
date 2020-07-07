#pragma once

#include "ConstantBuffer.h"
#include "Drawable.h"
#include "DirectXMath.h"

namespace Bind {

	class TransformCbuf : public Bindable
	{
	public:

		TransformCbuf(Graphics& graphics, const Drawable& parent, UINT slot = 0u);
		void Bind(Graphics& graphics) noexcept override;
	protected:

		struct Transforms
		{
			DirectX::XMMATRIX modelView;
			DirectX::XMMATRIX modelViewProj;
		};

		void UpdateBindImpl(Graphics& graphics, const Transforms& tf) noexcept;
		Transforms GetTransforms(Graphics& graphics) noexcept;
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pVertexConstantBuffer;
		const Drawable& parent;
	};
}