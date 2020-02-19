#ifndef BEZIER_H_
#define BEZIER_H_

#include "entity.h"
#include "point.h"

#include <vector>

class Bezier : Entity3D
{
public:
	Point3D 			P0, P1, P2, P3;
	int					Steps;
	bool 				bPoints;

						Bezier();
						Bezier(Point3D iP0, Point3D iP1, Point3D iP2, Point3D iP3, int iSteps, bool iPoints) : P0(iP0),  P1(iP1),  P2(iP2), P3(iP3), Steps(iSteps), bPoints(iPoints){}
	Point3D *			GetPoint(float T);
	void 				Render3D();
	vector<Point3D> * 	GetPath();
};



#endif /* BEZIER_H_ */
