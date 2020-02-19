#ifndef GRID_H_
#define GRID_H_

class Grid2I
{	
public:
	int Size_X, Size_Y;
	int Grid_X, Grid_Y;
	int Cell_X, Cell_Y;
	
	Grid2I()	: Size_X(0), Size_Y(0), Grid_X(0),Grid_Y(0), Cell_X(32), Cell_Y(16) {}

	Grid2I(int Size_x, int Size_y, int Grid_x, int Grid_y, int Cell_x, int Cell_y)
				: Size_X(Size_x), Size_Y(Size_y), Grid_X(Grid_x), Grid_Y(Grid_y), Cell_X(Cell_x), Cell_Y(Cell_y)	{}
				
	void CalcGrid();
	void CalcSize();
};

#endif /* GRID_H_ */
