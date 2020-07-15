#pragma once

#include "Projection.h"
#include "CameraIndicator.h"

#include <DirectXMath.h>
#include <string>

class Graphics;

namespace RGP
{
	class RenderGraph;
}

class Camera
{
public:

	Camera(Graphics& graphics, std::string name, DirectX::XMFLOAT3 homePosition = { 0.0f, 0.0f, 0.0f }, float homePitch = 0.0f, float homeYaw = 0.0f, bool isTethered = false) noexcept;
	void BindToGraphics(Graphics& graphics) const;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SpawnControlWidgets(Graphics& graphics) noexcept;
	void Reset(Graphics& graphics) noexcept;
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
	DirectX::XMFLOAT3 GetPosition() const noexcept;
	void SetPosition(const DirectX::XMFLOAT3& position_in) noexcept;
	const std::string& GetName() const noexcept;
	void LinkTechniques(RGP::RenderGraph& renderGraph);
	void Submit(size_t channels) const;
private:

	bool isTethered;
	std::string name;
	DirectX::XMFLOAT3 homePosition;
	float homePitch;
	float homeYaw;
	DirectX::XMFLOAT3 position;
	float pitch;
	float yaw;
	static constexpr float travelSpeed = 12.0f;
	static constexpr float rotationSpeed = 0.004f;
	bool enableCameraIndicator = true;
	bool enableFrustumIndicator = true;
	Projection projection;
	CameraIndicator indicator;
};

