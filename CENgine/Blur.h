#pragma once

#include "BindableCommon.h"
#include "Math.h"
#include "imgui/imgui.h"

class Blur
{
public:

	Blur(Graphics& graphics, int radius = 7, float sigma = 2.6f, const char* shader = "Blur_PS.cso")
		:
		shader(graphics, shader),
		pixelConstantBuffer(graphics, 0u),
		controlConstantBuffer(graphics, 1u),
		radius(radius),
		sigma(sigma)
	{
		SetKernelGauss(graphics, radius, sigma);
	}

	void Bind(Graphics& graphics) noexcept
	{
		shader.Bind(graphics);
		pixelConstantBuffer.Bind(graphics);
		controlConstantBuffer.Bind(graphics);
	}

	void SetHorizontal(Graphics& graphics)
	{
		controlConstantBuffer.Update(graphics, { TRUE });
	}

	void SetVertical(Graphics& graphics)
	{
		controlConstantBuffer.Update(graphics, { FALSE });
	}

	void RenderWidgets(Graphics& graphics)
	{
		bool filterChanged = false;
		{
			const char* items[] = { "Gauss","Box" };
			static const char* curItem = items[0];
			if(ImGui::BeginCombo("Filter Type", curItem))
			{
				for(int n = 0; n < std::size(items); n++)
				{
					const bool isSelected = (curItem == items[n]);
					if(ImGui::Selectable(items[n], isSelected))
					{
						filterChanged = true;
						curItem = items[n];
						if(curItem == items[0])
						{
							kernelType = KernelType::Gauss;
						}
						else if(curItem == items[1])
						{
							kernelType = KernelType::Box;
						}
					}
					if(isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}

		bool radChange = ImGui::SliderInt("Radius", &radius, 0, 15);
		bool sigChange = ImGui::SliderFloat("Sigma", &sigma, 0.1f, 10.0f);
		if(radChange || sigChange || filterChanged)
		{
			if(kernelType == KernelType::Gauss)
			{
				SetKernelGauss(graphics, radius, sigma);
			}
			else if(kernelType == KernelType::Box)
			{
				SetKernelBox(graphics, radius);
			}
		}
	}

	// for more accurate coefs, need to integrate, but meh :/
	void SetKernelGauss(Graphics& graphics, int radius, float sigma) NOXND
	{
		assert(radius <= maxRadius);
		Kernel k;
		k.nTaps = radius * 2 + 1;
		float sum = 0.0f;
		for(int i = 0; i < k.nTaps; i++)
		{
			const auto x = float(i - radius);
			const auto g = Gauss(x, sigma);
			sum += g;
			k.coefficients[i].x = g;
		}
		for(int i = 0; i < k.nTaps; i++)
		{
			k.coefficients[i].x /= sum;
		}
		pixelConstantBuffer.Update(graphics, k);
	}

	void SetKernelBox(Graphics& graphics, int radius) NOXND
	{
		assert(radius <= maxRadius);
		Kernel k;
		k.nTaps = radius * 2 + 1;
		const float c = 1.0f / k.nTaps;
		for(int i = 0; i < k.nTaps; i++)
		{
			k.coefficients[i].x = c;
		}
		pixelConstantBuffer.Update(graphics, k);
	}

private:

	enum class KernelType
	{
		Gauss,
		Box,
	};

	struct Kernel
	{
		int nTaps;
		float padding[3];
		DirectX::XMFLOAT4 coefficients[31];
	};

	struct Control
	{
		BOOL horizontal;
		float padding[3];
	};

	KernelType kernelType = KernelType::Gauss;
	static constexpr int maxRadius = 15;
	int radius;
	float sigma;
	Bind::PixelShader shader;
	Bind::PixelConstantBuffer<Kernel> pixelConstantBuffer;
	Bind::PixelConstantBuffer<Control> controlConstantBuffer;
};

