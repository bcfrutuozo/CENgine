#pragma once

#include <vector>
#include <memory>

class Camera;
class Graphics;

namespace RGP
{
	class RenderGraph;
}

class CameraContainer
{
public:

	~CameraContainer();
	void SpawnWindow(Graphics& graphics);
	void Bind(Graphics& graphics);
	void AddCamera(std::shared_ptr<Camera> pCamera);
	Camera* operator->();
	void LinkTechniques(RGP::RenderGraph& renderGraph);
	void Submit(size_t channels) const;
	Camera& GetActiveCamera();
private:

	Camera& GetControlledCamera();

	int active = 0;
	int controlled = 0;
	std::vector<std::shared_ptr<Camera>> cameras;
};