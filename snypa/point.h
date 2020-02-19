#ifndef POINT_H_
#define POINT_H_

class Point2I
{
public:
	int X, Y;

	Point2I()								: X(0), Y(0)		{}
	Point2I(int x, int y) 					: X(x),	Y(y)		{}
};

class Point2D
{	
public:
	double X, Y;
		
	Point2D() 								: X(0), Y(0)		{}
	Point2D(double x, double y) 			: X(x),	Y(y)		{}
};

class Point3D
{	
public:
	double X, Y, Z;
	
	Point3D()								: X(0),	Y(0), Z(0) 	{}
	Point3D(double x, double y, double z) 	: X(x),	Y(y), Z(z) 	{}
};

#endif /* POINT_H_ */
