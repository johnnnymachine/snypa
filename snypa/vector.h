#ifndef VECTOR_H_
#define VECTOR_H_

#include <math.h>

class Vec2D
{
public:
	double X, Y;
	
	Vec2D() 					: X(0), Y(0) 	{}
	Vec2D(double x, double y) 	: X(x), Y(y) 	{}
	
	void 	Normalize();
	Vec2D 	operator + (const Vec2D &rhs);
	Vec2D 	operator - (const Vec2D &rhs);
};

class Vec3D
{
public:
	double X, Y, Z;
	
	Vec3D() 							: X(0), Y(0), Z(0) 	{}
	Vec3D(double x, double y, double z) : X(x), Y(y), Z(z)	{}
	
	void 	Normalize();
	Vec3D 	operator + (const Vec3D &rhs);
	Vec3D 	operator - (const Vec3D &rhs);
	double	operator * (const Vec3D &rhs);	// Dot Product
	Vec3D 	operator ^ (const Vec3D &rhs);	// Cross Product
};

#endif /* VECTOR_H_ */
