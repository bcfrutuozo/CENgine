#include "Matrix.h"

DirectX::XMFLOAT3 ExtractEulerAngles(const DirectX::XMFLOAT4X4& matrix)
{
	DirectX::XMFLOAT3 euler;

	euler.x = asinf(-matrix._32);					// Pitch
	if(cosf(euler.x) > 0.0001)						// Not at poles
	{
		euler.y = atan2f(matrix._31, matrix._33);	// Yaw
		euler.z = atan2f(matrix._12, matrix._22);	// Roll
	}
	else
	{
		euler.y = 0.0f;								// Yaw
		euler.z = atan2f(-matrix._21, matrix._11);	// Roll
	}

	return euler;
}

DirectX::XMFLOAT3 ExtractTranslation(const DirectX::XMFLOAT4X4& matrix)
{
	return { matrix._41, matrix._42, matrix._43 };
}

DirectX::XMMATRIX ScaleTranslation(DirectX::XMMATRIX matrix, float scale)
{
	matrix.r[3].m128_f32[0] *= scale;
	matrix.r[3].m128_f32[1] *= scale;
	matrix.r[3].m128_f32[2] *= scale;
	return matrix;
}