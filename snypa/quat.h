#ifndef QUAT_H_
#define QUAT_H_

#include <math.h>
#include "vector.h"
#include "matrix.h"

#define  PRECISION 	0.00001

struct Quat
{
	float w, x, y, z;
	
	Quat(): w(1), x(0), y(0), z(0) {}			// The Identity Quat. A 360 degree rotation. (w = +-1)
	Quat(float W, float X, float Y, float Z);
	Quat(float Angle, Vec3D &Axis);
	void Normalize();
	Quat Conjugate();	
	Quat operator*(const Quat &rhs);
	float* glMatrix();
};

#endif /* Quat_H_ */
