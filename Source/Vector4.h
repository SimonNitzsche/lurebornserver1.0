#pragma once
#include "Vector3.h"

struct NiQuaternion {
	float x;
	float y;
	float z;
	float w;

	NiQuaternion Create(float X, float Y, float Z, float W) {
		NiQuaternion nq;
		nq.x = X;
		nq.y = Y;
		nq.z = Z;
		nq.w = W;
		return nq;
	}
};