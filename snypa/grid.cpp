#include "grid.h"
#include "core.h"

void Grid2I::CalcGrid()
{
	if(!Cell_X || !Cell_Y)
	{	p->Info("<Error> Grid2I::CalcGrid() - Invalid Cell Size\n");	return;
	}

	Grid_X = Size_X / (Cell_X);
	Grid_Y = Size_Y / (Cell_Y);
}

void Grid2I::CalcSize()
{
	Size_X = Grid_X * (Cell_X);
	Size_Y = Grid_Y * (Cell_Y);
}
