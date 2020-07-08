#include "Camera.h"
#include "Math.h"
#include "imgui/imgui.h"

Camera::Camera() noexcept
{
	Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{	
	const auto& forwardBaseVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// Apply the camera rotations to a base vector
	const auto lookVector = DirectX::XMVector3Transform(forwardBaseVector, 
		DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f));

	/* Generate the camera transform (applied to all objects to arrange them relatively
	 * to the camera position/orientation in the world) from the camera position and
	 * direction. Camera "top" always faces towards +Y (cannot do a barrel roll) */
	const auto camPosition = DirectX::XMLoadFloat3(&position);
	const auto camTarget = DirectX::XMVectorAdd(camPosition, lookVector);
	return DirectX::XMMatrixLookAtLH(camPosition, camTarget, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void Camera::SpawnControlWindow() noexcept
{
	if(ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &position.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &position.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &position.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &pitch, 0.995f * -90.0f, 0.995f * 90.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);

		if(ImGui::Button("Reset"))
		{
			Reset();
		}
	}

	ImGui::End();
}

void Camera::Reset() noexcept
{
	position = { -13.5f, 6.0f, 3.5f };
	pitch = 0.0f;
	yaw = PI / 2.0f;
}

void Camera::Rotate(float dx, float dy) noexcept
{
	yaw = WrapAngle(yaw + dx * rotationSpeed);
	pitch = std::clamp(pitch + dy * rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	DirectX::XMStoreFloat3(&translation,
		DirectX::XMVector3Transform(
			DirectX::XMLoadFloat3(&translation),
			DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) *
			DirectX::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)));

	position = {
		position.x + translation.x,
		position.y + translation.y,
		position.z + translation.z
	};
}

DirectX::XMFLOAT3 Camera::GetPosition() const noexcept
{
	return position;
}
