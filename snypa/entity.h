#ifndef ENTITY_H_
#define ENTITY_H_

#include "point.h"
#include "quat.h"
#include "vector.h"

class Entity2D
{
public:
	bool 			bActive2D;
	bool 			bRender2D;
	Point2D 		Pos2D;					// Position
	Vec2D	 		Ori2D;					// Orientation
	Vec2D			Vel2D;					// Velocity
	Vec2D			Acc2D;					// Acceleration

					Entity2D():		bActive2D (true), bRender2D(true) {}
	void 			Render2D();
};

class Entity3D
{
public:
	bool 			bActive3D;
	bool			bRender3D;
	Point3D			Pos3D;					// Position
	Quat 			Ori3D;					// Orientation
	Vec3D			Vel3D;					// Velocity
	Vec3D			Acc3D;					// Acceleration
	Vec3D 			Right, Up, Forward;

					Entity3D():		bActive3D (true), bRender3D(true) {}
	void 			Render3D();
};

#endif /* ENTITY_H_ */
