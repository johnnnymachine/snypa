#ifndef QUADRIX_H_
#define QUADRIX_H_

#include "grid.h"
#include "point.h"
#include "entity.h"
#include "coreglx.h"
#include "timer.h"

#include <string>
#include <map>

using namespace std;

class Quadrix : Entity2D, Entity3D, Grid2I
{
public:
	string StrData;										// The main string
	map <int, string> StrMap;							// An indexed map of the string, line by line
	bool bCursor;										// Render the cursor on/off
	Point2I Cursor;										// Cursor position. Range 0-Grid_X, 0-Grid_Y
	Timer tCursor;
	float CursorOnTime, CursorEndTime;
	int Scroll_X, Scroll_Y;
	unsigned int StrPos;								// The current string index
	Point3D	pTopLeft, pTopRight, pBotLeft, pBotRight;	// 4 points for rendering the 3d version
	Vec3D Normal;										// The normal vector to the plane specified by 3 of the 4 points.

	Quadrix();
	~Quadrix();
	
	bool Init();
	void ParseStringMap();
	void GetStrPos();
	void SetCursorPos(double X, double Y);
	void Render2D();
	void Render3D();
	void CalcNormal();
	void CursorLeft();
	void CursorRight();
	void CursorDown();
	void CursorUp();
	void AddCharacter(char *pC);
	char GetChar();
	void Return();
	void Backspace();
	void Delete();
	void Home();
	void End();
	void PageUp();
	void PageDown();
	void Save();
};

#endif /* QUADRIX_H_ */
