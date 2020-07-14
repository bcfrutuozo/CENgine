#include "CameraContainer.h"
#include "Camera.h"
#include "Graphics.h"
#include "imgui/imgui.h"
#include "RenderGraph.h"

CameraContainer::~CameraContainer()
{ }

void CameraContainer::SpawnWindow(Graphics& graphics)
{
	if(ImGui::Begin("Cameras"))
	{
		if(ImGui::BeginCombo("Active Camera", (*this)->GetName().c_str()))
		{
			for(int i = 0; i < std::size(cameras); i++)
			{
				const bool isSelected = i == active;
				if(ImGui::Selectable(cameras[i]->GetName().c_str(), isSelected))
				{
					active = i;
				}
			}
			ImGui::EndCombo();
		}

		if(ImGui::BeginCombo("Controlled Camera", GetControlledCamera().GetName().c_str()))
		{
			for(int i = 0; i < std::size(cameras); i++)
			{
				const bool isSelected = i == controlled;
				if(ImGui::Selectable(cameras[i]->GetName().c_str(), isSelected))
				{
					controlled = i;
				}
			}
			ImGui::EndCombo();
		}

		GetControlledCamera().SpawnControlWidgets(graphics);
	}
	ImGui::End();
}

void CameraContainer::Bind(Graphics& graphics)
{
	graphics.SetCamera((*this)->GetMatrix());
}

void CameraContainer::AddCamera(std::shared_ptr<Camera> pCamera)
{
	cameras.push_back(std::move(pCamera));
}

Camera* CameraContainer::operator->()
{
	return &GetActiveCamera();
}

void CameraContainer::LinkTechniques(RGP::RenderGraph& renderGraph)
{
	for(auto& pCamera : cameras)
	{
		pCamera->LinkTechniques(renderGraph);
	}
}

void CameraContainer::Submit(size_t channels) const
{
	for(size_t i = 0; i < cameras.size(); i++)
	{
		if(i != active)
		{
			cameras[i]->Submit(channels);
		}
	}
}

Camera& CameraContainer::GetActiveCamera()
{
	return *cameras[active];
}

Camera& CameraContainer::GetControlledCamera()
{
	return *cameras[controlled];
}
