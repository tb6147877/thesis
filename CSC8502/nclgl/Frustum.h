#pragma once

#include "Plane.h"
#include "Matrix4.h"

class Frustum
{
public:
	Frustum(void) {};
	~Frustum(void) {};

	void FromMatrix(const Matrix4& mvp);
	bool InsideFrustum(const Vector3& pos,const float radius);
protected:
	Plane planes[6];
};

