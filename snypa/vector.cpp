#include "vector.h"

void Vec2D::Normalize()
{	
	double Mag = sqrt((X * X) + (Y * Y));
	X /= Mag;	Y /= Mag;
}

Vec2D Vec2D::operator+(const Vec2D &rhs)
{	
	return Vec2D((X + rhs.X), (Y + rhs.Y));
}

Vec2D Vec2D::operator-(const Vec2D &rhs)
{	
	return Vec2D((X - rhs.X), (Y - rhs.Y));
}

void Vec3D::Normalize()
{	
	double Mag = sqrt((X * X) + (Y * Y) + (Z * Z));
	X /= Mag;	Y /= Mag;	Z /= Mag;
}

Vec3D Vec3D::operator+(const Vec3D &rhs)
{	
	return Vec3D((X + rhs.X), (Y + rhs.Y), (Z + rhs.Z)	);
}

Vec3D Vec3D::operator-(const Vec3D &rhs)
{	
	return Vec3D((X - rhs.X), (Y - rhs.Y), (Z - rhs.Z)	);
}

double Vec3D::operator*(const Vec3D &rhs)	// Dot product - Returns the sum of the products of the two vectors
{	
	return ((X * rhs.X) + (Y * rhs.Y) + (Z * rhs.Z));
}

Vec3D Vec3D::operator^(const Vec3D &rhs)	// Cross product - Returns a vector perpendicular to two vectors
{	
	return Vec3D(((Y * rhs.Z) - (rhs.Y * Z)), ((Z * rhs.X) - (rhs.Z * X)), ((X * rhs.Y) - (rhs.X * Y)));
}
