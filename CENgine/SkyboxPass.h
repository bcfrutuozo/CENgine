#pragma once

#include "RenderQueuePass.h"
#include "Job.h"
#include "Sink.h"
#include "Source.h"
#include "Stencil.h"
#include "Camera.h"
#include "Sampler.h"
#include "Rasterizer.h"
#include "DepthStencil.h"
#include "Cube.h"
#include "CubeTexture.h"
#include "SkyboxTransformCbuf.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Topology.h"
#include "InputLayout.h"
#include "Sphere.h"

#include <vector>

class Graphics;

namespace RGP
{
	class SkyboxPass : public BindingPass
	{
	public:
		SkyboxPass(Graphics& graphics, std::string name)
			:
			BindingPass(std::move(name))
		{
			RegisterSink(DirectBufferSink<Bind::RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<Bind::DepthStencil>::Make("depthStencil", depthStencil));
			AddBind(std::make_shared<Bind::CubeTexture>(graphics, "Images\\SpaceBox"));
			AddBind(Bind::Stencil::Resolve(graphics, Bind::Stencil::Mode::DepthFirst));
			AddBind(Bind::Sampler::Resolve(graphics, Bind::Sampler::Type::Bilinear));
			AddBind(Bind::Rasterizer::Resolve(graphics, true));
			AddBind(std::make_shared<Bind::SkyboxTransformCbuf>(graphics));
			AddBind(Bind::PixelShader::Resolve(graphics, "Skybox_PS.cso"));
			AddBind(Bind::Topology::Resolve(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
			{
				auto pvs = Bind::VertexShader::Resolve(graphics, "Skybox_VS.cso");
				{ // Cube
					auto model = Cube::Make();
					const auto geometryTag = "$cube_map";
					pCubeVertices = Bind::VertexBuffer::Resolve(graphics, geometryTag, std::move(model.vertices));
					pCubeIndices = Bind::IndexBuffer::Resolve(graphics, geometryTag, std::move(model.indices));
					cubeCount = (UINT)model.indices.size();
					// Layout is shared between cube and sphere; use cube data to generate
					AddBind(Bind::InputLayout::Resolve(graphics, model.vertices.GetLayout(), *pvs));
				}
				{ // Sphere
					auto model = Sphere::Make();
					const auto geometryTag = "$sphere_map";
					pSphereVertices = Bind::VertexBuffer::Resolve(graphics, geometryTag, std::move(model.vertices));
					pSphereIndices = Bind::IndexBuffer::Resolve(graphics, geometryTag, std::move(model.indices));
					sphereCount = (UINT)model.indices.size();
				}
				AddBind(std::move(pvs));
			}
			RegisterSource(DirectBufferSource<Bind::RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<Bind::DepthStencil>::Make("depthStencil", depthStencil));
		}

		void BindMainCamera(const Camera& camera) noexcept
		{
			pMainCamera = &camera;
		}

		void Execute(Graphics& graphics) const NOXND override
		{
			assert(pMainCamera);

			UINT indexCount;
			pMainCamera->BindToGraphics(graphics);

			if(useSphere)
			{
				pSphereVertices->Bind(graphics);
				pSphereIndices->Bind(graphics);
				indexCount = sphereCount;
			}
			else
			{
				pCubeVertices->Bind(graphics);
				pCubeIndices->Bind(graphics);
				indexCount = cubeCount;
			}

			BindAll(graphics);
			graphics.DrawIndexed(indexCount);
		}

		void RenderWidget()
		{
			if(ImGui::Begin("Skybox"))
			{
				ImGui::Checkbox("Use sphere", &useSphere);
			}
			ImGui::End();
		}

	private:

		bool useSphere = true;
		const Camera* pMainCamera = nullptr;
		std::shared_ptr<Bind::VertexBuffer> pCubeVertices;
		std::shared_ptr<Bind::IndexBuffer> pCubeIndices;
		UINT cubeCount;
		std::shared_ptr<Bind::VertexBuffer> pSphereVertices;
		std::shared_ptr<Bind::IndexBuffer> pSphereIndices;
		UINT sphereCount;
	};
}