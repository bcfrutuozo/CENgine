#include "Camera.h"
#include "Math.h"
#include "imgui/imgui.h"
#include "Graphics.h"

Camera::Camera(Graphics& graphics, std::string name, DirectX::XMFLOAT3 homePosition, float homePitch, float homeYaw, bool isTethered) noexcept
	:
	name(std::move(name)),
	homePosition(homePosition),
	homePitch(homePitch),
	homeYaw(homeYaw),
	projection(graphics, 1.0f, (9.0f / 16.0f), 0.5f, 400.0f),
	indicator(graphics),
	isTethered(isTethered)
{
	if(isTethered)
	{
		position = homePosition;
		indicator.SetPosition(position);
		projection.SetPosition(position);
	}

	Reset(graphics);
}

void Camera::BindToGraphics(Graphics& graphics) const
{
	graphics.SetCamera(GetMatrix());
	graphics.SetProjection(projection.GetMatrix());
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

DirectX::XMMATRIX Camera::GetProjection() const noexcept
{
	return projection.GetMatrix();
}

void Camera::SpawnControlWidgets(Graphics& graphics) noexcept
{
	bool rotDirty = false;
	bool posDirty = false;
	const auto dcheck = [](bool d, bool& carry) { carry = carry || d; };

	if(!isTethered)
	{
		ImGui::Text("Position");
		dcheck(ImGui::SliderFloat("X", &position.x, -80.0f, 80.0f, "%.1f"), posDirty);
		dcheck(ImGui::SliderFloat("Y", &position.y, -80.0f, 80.0f, "%.1f"), posDirty);
		dcheck(ImGui::SliderFloat("Z", &position.z, -80.0f, 80.0f, "%.1f"), posDirty);
	}
	ImGui::Text("Orientation");
	dcheck(ImGui::SliderAngle("Pitch", &pitch, 0.995f * -90.0f, 0.995f * 90.0f), rotDirty);
	dcheck(ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f), rotDirty);
	projection.RenderWidgets(graphics);
	ImGui::Checkbox("Camera Indicator", &enableCameraIndicator);
	ImGui::Checkbox("Frustum Indicator", &enableFrustumIndicator);

	if(ImGui::Button("Reset"))
	{
		Reset(graphics);
	}

	if(rotDirty)
	{
		const DirectX::XMFLOAT3 angles = { pitch,yaw,0.0f };
		indicator.SetRotation(angles);
		projection.SetRotation(angles);
	}
	if(posDirty)
	{
		indicator.SetPosition(position);
		projection.SetPosition(position);

		projection.RenderWidgets(graphics);
	}
}

void Camera::Reset(Graphics& graphics) noexcept
{
	if(!isTethered)
	{
		position = homePosition;
		indicator.SetPosition(position);
		projection.SetPosition(position);
	}

	pitch = homePitch;
	yaw = homeYaw;

	const DirectX::XMFLOAT3 angles = { pitch,yaw,0.0f };
	indicator.SetRotation(angles);
	projection.SetRotation(angles);
	projection.Reset(graphics);
}

void Camera::Rotate(float dx, float dy) noexcept
{
	yaw = WrapAngle(yaw + dx * rotationSpeed);
	pitch = std::clamp(pitch + dy * rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);

	const DirectX::XMFLOAT3 angles = { pitch, yaw, 0.0f };
	indicator.SetRotation(angles);
	projection.SetRotation(angles);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	if(!isTethered)
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

		indicator.SetPosition(position);
		projection.SetPosition(position);
	}
}

DirectX::XMFLOAT3 Camera::GetPosition() const noexcept
{
	return position;
}

void Camera::SetPosition(const DirectX::XMFLOAT3& position_in) noexcept
{
	position = position_in;
	indicator.SetPosition(position_in);
	projection.SetPosition(position_in);
}

const std::string& Camera::GetName() const noexcept
{
	return name;
}

void Camera::LinkTechniques(RGP::RenderGraph& renderGraph)
{
	indicator.LinkTechniques(renderGraph);
	projection.LinkTechniques(renderGraph);
}

void Camera::Submit(size_t channels) const
{
	if(enableCameraIndicator)
	{
		indicator.Submit(channels);
	}
	if(enableFrustumIndicator)
	{
		projection.Submit(channels);
	}
}