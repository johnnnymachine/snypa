#include "core.h"
#include "bezier.h"

Bezier::Bezier()
{
	P0.X = -2; P0.Y = 0; P0.Z = 0;
	P1.X = -1; P1.Y = 1; P1.Z = 0;
	P2.X = 1; P2.Y = -1; P2.Z = 0;
	P3.X = 2; P3.Y = 0; P3.Z = 0;
	Steps = 100;
	bPoints = false;
}

void Bezier::Render3D()
{
	Point3D *pPStart = GetPoint(0), *pPEnd;

	P0.X = -2 * p->pRend->sinFrame; P0.Y = p->pRend->cosFrame; P0.Z = 0;
	P3.X = 2 * p->pRend->sinFrame; P3.Y = p->pRend->cosFrame; P3.Z = 0;
	if(bPoints)
	{
		glPointSize(5.0);
		glBegin(GL_POINTS);
			glVertex3f(P0.X, P0.Y, P0.Z);
			glVertex3f(P1.X, P1.Y, P1.Z);
			glVertex3f(P2.X, P2.Y, P2.Z);
			glVertex3f(P3.X, P3.Y, P3.Z);
		glEnd();
	}

	glBegin(GL_LINES);
		int i = 0;
		for(float t = 1 / (float)Steps; i < Steps; t += 1 / (float)Steps, i++)
		{
			pPEnd = GetPoint(t);
			glVertex3f(pPStart->X, pPStart->Y, pPStart->Z);
			glVertex3f(pPEnd->X, pPEnd->Y, pPEnd->Z);
			delete pPStart;
			pPStart = pPEnd;
		}
		delete pPStart;
	glEnd();

}

Point3D * Bezier::GetPoint(float t)		// GetPoint Allocates a Point3D and returns a pointer, be sure to delete the object after use.
{
	Point3D *pP = new Point3D;
	pP->X = (pow((1 - t), 3) * P0.X) + (3 * t * pow((1 - t), 2) * P1.X)  + (3 * (1 - t) * pow(t, 2)* P2.X) + (pow (t, 3) * P3.X);
	pP->Y = (pow((1 - t), 3) * P0.Y) + (3 * t * pow((1 - t), 2) * P1.Y)  + (3 * (1 - t) * pow(t, 2)* P2.Y) + (pow (t, 3) * P3.Y);
	pP->Z = (pow((1 - t), 3) * P0.Z) + (3 * t * pow((1 - t), 2) * P1.Z)  + (3 * (1 - t) * pow(t, 2)* P2.Z) + (pow (t, 3) * P3.Z);
	return pP;
}

vector<Point3D> * Bezier::GetPath()		// GetPath allocates and generates a vector of points, returning a pointer, delete the object after use
{
	vector<Point3D> *pPath = new vector<Point3D>;

	Point3D *pP;
	int i = 0;
	for(float t = 0; i < Steps; t += 1 / (float)Steps, i++)
	{
		pP = GetPoint(t);
		pPath->push_back(*pP);
	}

	return pPath;
}

