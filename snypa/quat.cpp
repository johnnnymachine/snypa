#include "quat.h"

Quat::Quat(float W, float X, float Y, float Z)
{	
	w = W;	x = X;	y = Y;	z = Z;
	Normalize();
}
Quat::Quat(float Angle, Vec3D &Axis)
{	
	w = x = y = z = 0;
	float sinht = sin(Angle * 0.5);
	Axis.Normalize();
	Quat((cos(Angle * 0.5)), (Axis.X * sinht), (Axis.Y * sinht), (Axis.Z * sinht));
}
void Quat::Normalize()
{	
	float Mag = (w * w) + (x * x) + (y * y) + (z * z);
	if(Mag != 0 && (fabs(Mag - 1) > PRECISION))
	{	Mag = sqrt(Mag);	w /= Mag;	x /= Mag;	y /= Mag;	z /= Mag;
	}
}
Quat Quat::Conjugate()
{	
	return Quat(w, -x, -y, -z);
}
Quat Quat::operator*(const Quat &Q)
{	
	return Quat(	+ (w * Q.w) - (x * Q.x) - (y * Q.y) - (z * Q.z),
					+ (w * Q.x) + (x * Q.w) + (y * Q.z) - (z * Q.y),
					+ (w * Q.y) - (x * Q.z) + (y * Q.w) + (z * Q.x),
					+ (w * Q.z) + (x * Q.y) - (y * Q.x) + (z * Q.w)		);
}
float* Quat::glMatrix()
{	// Transposed for GL Format
	Matrix4x4 *pRet  = new Matrix4x4(	(1 - (2 * (x * x + z * z))),	(2 * (x * y - w * z)), 			(2 * (x * z + w * y)), 			(0),
										(2 * (x * y + w * z)), 			(1 - (2 * (x * x + z * z))),	(2 * (y * z - w * x)), 			(0),
										(2 * (x * z - w * y)), 			(2 * (y * z + w * x)),			(1 - (2 * (x * x + y * y))), 	(0),
										(0), 							(0), 							(0), 							(1)		);
	return pRet->m;
}

