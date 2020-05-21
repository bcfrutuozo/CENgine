#pragma once

#include "DrawableBase.h"
#include "Math.h"

template<class T>
class Object : public DrawableBase<T>
{
public:

	Object(Graphics& graphics, 
		std::mt19937& rng,
		std::uniform_real_distribution<float>& aDist,
		std::uniform_real_distribution<float>& dDist,
		std::uniform_real_distribution<float>& oDist,
		std::uniform_real_distribution<float>& rDist)
		:
		r( rDist( rng ) ),
		roll(0.0f),
		pitch(0.0f),
		yaw(0.0f),
		theta( aDist( rng ) ),
		phi( aDist( rng ) ),
		chi( aDist( rng ) ),
		dRoll( dDist( rng ) ),
		dPitch( dDist( rng ) ),
		dYaw( dDist( rng ) ),
		dTheta( oDist( rng ) ),
		dPhi( oDist( rng ) ),
		dChi( oDist( rng ) )
	{}

	void Update(const float dt) noexcept
	{
		roll = WrapAngle( roll + dRoll * dt );
		pitch = WrapAngle( pitch + dPitch * dt );
		yaw = WrapAngle( yaw + dYaw * dt );
		theta = WrapAngle( theta + dTheta * dt );
		phi = WrapAngle( phi + dPhi * dt );
		chi = WrapAngle( chi + dChi * dt );
	}

	DirectX::XMMATRIX GetTransformXM() const noexcept
	{
		return DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) * 
			DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) * 
			DirectX::XMMatrixRotationRollPitchYaw(theta, phi, chi);
	}
protected:
	// Positional
	float r;
	float roll;
	float pitch;
	float yaw;
	float theta;
	float phi;
	float chi;

	// Speed (delta/s)
	float dRoll;
	float dPitch;
	float dYaw;
	float dTheta;
	float dPhi;
	float dChi;
};

