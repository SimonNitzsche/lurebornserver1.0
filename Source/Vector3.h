#pragma once

struct NiPoint3 {
	float x;
	float y;
	float z;

	NiPoint3 Create(float X, float Y, float Z) {
		NiPoint3 np;
		np.x = X;
		np.y = Y;
		np.z = Z;
		return np;
	}
};