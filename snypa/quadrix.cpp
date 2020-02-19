#include "quadrix.h"
#include "core.h"

Quadrix::Quadrix()
{
	bCursor = true;
	CursorOnTime = 0.5f, CursorEndTime = 1.0f;
	Scroll_X = 0;
	Scroll_Y = 0;
	StrPos = 0;
	// Grid_X, Grid_Y & Pos2D is actually controlled in Render() for joystick use, but we leave defaults here
	Grid_X = 32;		Grid_Y = 8;
	Cell_X = 16;		Cell_Y = 32;
	Pos2D.X = 0;		Pos2D.Y = 0;
	Size_X = 0;			Size_Y = 0;
	Cursor.X = 0;		Cursor.Y = 0;
	pTopLeft.X = -1; 	pTopLeft.Y = 1; 	pTopLeft.Z = 0;
	pTopRight.X = 1;	pTopRight.Y = 1;	pTopRight.Z = 0;
	pBotLeft.X = -1; 	pBotLeft.Y = -1; 	pBotLeft.Z = 0;
	pBotRight.X = 1;	pBotRight.Y= -1; 	pBotRight.Z = 0;
}

Quadrix::~Quadrix()
{
}

bool Quadrix::Init()
{	
	// Size_X & Size_Y are calculated with CalcSize();
	CalcSize();

	for(unsigned char C = 0; C < 128; C++)
		StrData.push_back(C);

	ParseStringMap();
	tCursor.Start();
	//p->Info("StrData = %s", StrData.c_str());

	return true;
}

void Quadrix::ParseStringMap()
{
	// Takes the string of the Quadrix and parses it into a map of strings, indexed by number of newline characters.
	unsigned int MapIndex, StringIndex;
	int XPos;
	string NewString = "";
	StrMap.clear();

	for(MapIndex = 0, StringIndex = 0, XPos = 0; StringIndex < StrData.length(); StringIndex++, XPos++)
	{
		switch(StrData[StringIndex])
		{
			case '\n':		// Newline, save the string and start from next line
			{
				XPos = 0;
				NewString += " ";		// Replacing the newline char with a space, so we can edit the newline
				StrMap[MapIndex] = NewString;
				NewString = "";
				MapIndex++;
			}
			break;

			default:	NewString.push_back(StrData[StringIndex]);						break;
		}
	}
	if(StrData.length())
		StrMap[MapIndex] = NewString + " ";		// Appending a space so we can move the cursor to the end of the string
}

void Quadrix::GetStrPos()
{
	// Converts Cursor + Scroll coordinates into a linear index into the Quadrix String
	// Call ParseStringMap() prior to calling.
	unsigned int NewStrPos = 0;

	for(double Y = 0; Y < Cursor.Y + Scroll_Y; Y++)
	{
		for(double X = 0; X < StrMap[Y].size(); X++)
			NewStrPos++;
	}
	NewStrPos += Cursor.X + Scroll_X;
	StrPos = NewStrPos;
}

void Quadrix::Render2D()
{
	float X1, X2, Y1, Y2;
	unsigned char C;
	unsigned int MapIndex = (unsigned int)Scroll_Y, MapStringIndex = 0;

	Grid_X = 16 + JRX * 50;
	Grid_Y = 16  - JRY * 50;
	CalcSize();

	ParseStringMap();
	GetStrPos();

	tCursor.Stop();
	if(tCursor.Delta > CursorEndTime)
		tCursor.Start();

	glPushMatrix();
		Pos2D.X = JLX * p->pCoreGLX->Width;
		Pos2D.Y = p->pCoreGLX->Height + JLY * p->pCoreGLX->Height;
		glTranslatef(Pos2D.X, Pos2D.Y, 0);

		if(bCursor && (tCursor.Delta < CursorOnTime))
		{
			glLineWidth(1);
			glBegin(GL_LINES);														// Draw the Cursor
				glVertex2f(Cursor.X * Cell_X + 1, -Cursor.Y * Cell_Y);
				glVertex2f(Cursor.X * Cell_X + 1, -(Cursor.Y + 1) * Cell_Y);
			glEnd();
		}

											// Draw the Text
			for(int X, Y = 0; Y < Grid_Y; Y++, MapIndex++)
			{
				MapStringIndex = Scroll_X;
				for(X = 0; X < Grid_X; X++, MapStringIndex++)
				{
					if(MapIndex < StrMap.size())
					{
						if(MapStringIndex < StrMap[MapIndex].size())	C = StrMap[MapIndex][MapStringIndex];
						else											C = ' ';
					}
					else												C = ' ';

					// For the 2D version we calculate the quad points by multiplying the matrix index by the cell size for a 1 to 1 pixel ratio
					X1 = X * Cell_X;	X2 = X1 + Cell_X;		Y2 = -Y * Cell_Y;			Y1 = Y2 - Cell_Y;	// We Invert the Y Axis

					glBindTexture(GL_TEXTURE_2D, p->pRend->pTTFLarge->pTextures[(int)C]);
					//glBindTexture(GL_TEXTURE_2D, pCF->TexID);

					glBegin(GL_QUADS);
					glTexCoord2f(0, 1);	glVertex2f(X1, Y1);
					glTexCoord2f(0, 0);	glVertex2f(X1, Y2);
					glTexCoord2f(1, 0);	glVertex2f(X2, Y2);
					glTexCoord2f(1, 1);	glVertex2f(X2, Y1);

					/*glTexCoord2f(pCF->u1[C], pCF->v1[C]);	glVertex2f(X1, Y1);
					glTexCoord2f(pCF->u1[C], pCF->v2[C]);	glVertex2f(X1, Y2);
					glTexCoord2f(pCF->u2[C], pCF->v2[C]);	glVertex2f(X2, Y2);
					glTexCoord2f(pCF->u2[C], pCF->v1[C]);	glVertex2f(X2, Y1);*/
					glEnd();
				}
				MapStringIndex = 0;
			}

	glPopMatrix();
	glFinish();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Quadrix::Render3D()
{
	unsigned char C;
	unsigned int MapIndex = (unsigned int)Scroll_Y, MapStringIndex = 0;
	/*Grid_X = 16 + JRX * 50;
	Grid_Y = 8 - JRY * 50;
	if(Grid_X < 0) Grid_X = 0;
	if(Grid_Y < 0) Grid_Y = 0;*/
	Grid_X = 16;
	Grid_Y = 8;
	ParseStringMap();
	GetStrPos();
	tCursor.Stop();
	if(tCursor.Delta > CursorEndTime)		tCursor.Start();
	CalcNormal();												// Normal is used when drawing the cursor
	Point3D paLeftEdge[Grid_Y + 1], paRightEdge[Grid_Y + 1];	// 2 point arrays for storing the interpolated edge values.
	Point3D pStep;												// It is Grid_Y + 1 because we want to store the start _and_ end points.
	glPointSize(2);
	// Begin interpolation, left edge
	pStep.X = (pBotLeft.X - pTopLeft.X) / (float) Grid_Y;
	pStep.Y = (pBotLeft.Y - pTopLeft.Y) / (float) Grid_Y;
	pStep.Z = (pBotLeft.Z - pTopLeft.Z) / (float) Grid_Y;
	//glColor3f(1,0,0);
	for(int i = 0; i <= Grid_Y; i++)
	{
		paLeftEdge[i].X = pTopLeft.X + (float)(i * pStep.X);
		paLeftEdge[i].Y = pTopLeft.Y + (float)(i * pStep.Y);
		paLeftEdge[i].Z = pTopLeft.Z + (float)(i * pStep.Z);
		/*glBegin(GL_POINTS);
			glVertex3f(paLeftEdge[i].X, paLeftEdge[i].Y, paLeftEdge[i].Z);
		glEnd();*/
	}
	// Begin interpolation, right edge
	pStep.X = (pBotRight.X - pTopRight.X) / (float) Grid_Y;
	pStep.Y = (pBotRight.Y - pTopRight.Y) / (float) Grid_Y;
	pStep.Z = (pBotRight.Z - pTopRight.Z) / (float) Grid_Y;
	//glColor3f(0,1,0);
	for(int i = 0; i <= Grid_Y; i++)
	{
		paRightEdge[i].X = pTopRight.X + (float)(i * pStep.X);
		paRightEdge[i].Y = pTopRight.Y + (float)(i * pStep.Y);
		paRightEdge[i].Z = pTopRight.Z + (float)(i * pStep.Z);
		/*glBegin(GL_POINTS);
			glVertex3f(paRightEdge[i].X, paRightEdge[i].Y, paRightEdge[i].Z);
		glEnd();*/
	}

	Point3D pMatrix[Grid_Y + 1][Grid_X + 1];								// 2d point array of 3d points for rendering.
	//glColor3f(0,0,1);
	for(int Y = 0; Y <= Grid_Y; Y++)										// Calculate the matrix of points for rendering our quads and cursor
	{																		// by interpolating between our edge arrays.
		pStep.X = (paRightEdge[Y].X - paLeftEdge[Y].X) / (float) Grid_X;	// First calculate the step for this row
		pStep.Y = (paRightEdge[Y].Y - paLeftEdge[Y].Y) / (float) Grid_X;
		pStep.Z = (paRightEdge[Y].Z - paLeftEdge[Y].Z) / (float) Grid_X;

		for(int X = 0; X <= Grid_X; X++)
		{
			pMatrix[Y][X].X = paLeftEdge[Y].X + (float)(X * pStep.X);		// Do the interpolation
			pMatrix[Y][X].Y = paLeftEdge[Y].Y + (float)(X * pStep.Y);
			pMatrix[Y][X].Z = paLeftEdge[Y].Z + (float)(X * pStep.Z);
			/*glBegin(GL_POINTS);
				glVertex3f(pMatrix[Y][X].X, pMatrix[Y][X].Y, pMatrix[Y][X].Z);
			glEnd();*/
		}
	}
	glPushMatrix();
		glRotatef(JRX * 100, 0, 1.0, 0);
		glRotatef(JRY * 100, 1.0, 0, 0);
		Pos3D.X = JLX * 2;
		Pos3D.Y = JLY * 2;
		Pos3D.Z = 0;
		glTranslatef(Pos3D.X, Pos3D.Y, Pos3D.Z);

		if(bCursor && (tCursor.Delta < CursorOnTime))
		{
			glLineWidth(2);
			glBegin(GL_LINES);														// Draw the cursor, we add a fraction of the normal to raise the cursor
				glVertex3f(	pMatrix[Cursor.Y][Cursor.X].X + (Normal.X / 50),		// above the quads (ie to be visible)
							pMatrix[Cursor.Y][Cursor.X].Y + (Normal.Y / 50),
							pMatrix[Cursor.Y][Cursor.X].Z + (Normal.Z / 50));

				glVertex3f(	pMatrix[Cursor.Y + 1][Cursor.X].X + (Normal.X / 50),
							pMatrix[Cursor.Y + 1][Cursor.X].Y + (Normal.Y / 50),
							pMatrix[Cursor.Y + 1][Cursor.X].Z + (Normal.Z / 50));
			glEnd();
		}

		for(int X, Y = 0; Y < Grid_Y; Y++, MapIndex++)
			{
				MapStringIndex = Scroll_X;
				for(X = 0; X < Grid_X; X++, MapStringIndex++)
				{
					if(MapIndex < StrMap.size())
					{
						if(MapStringIndex < StrMap[MapIndex].size())	C = StrMap[MapIndex][MapStringIndex];
						else											C = ' ';
					}
					else												C = ' ';

					glBindTexture(GL_TEXTURE_2D, p->pRend->pTTFLarge->pTextures[(int)C]);
					glBegin(GL_QUADS);															// Draw the Text
					glTexCoord2f(0, 0);	glVertex3f(pMatrix[Y][X].X, pMatrix[Y][X].Y, pMatrix[Y][X].Z);							// Top Left
					glTexCoord2f(0, 1);	glVertex3f(pMatrix[Y][X].X, pMatrix[Y + 1][X].Y, pMatrix[Y + 1][X].Z);					// Bottom Left
					glTexCoord2f(1, 1);	glVertex3f(pMatrix[Y + 1][X + 1].X, pMatrix[Y + 1][X + 1].Y, pMatrix[Y + 1][X + 1].Z);	// Bottom Right
					glTexCoord2f(1, 0);	glVertex3f(pMatrix[Y][X + 1].X, pMatrix[Y][X].Y, pMatrix[Y][X + 1].Z);				// Top Right

					// This the old bitmap font system
					// Draw the quads, inverting the y texture coordinate
					/*glTexCoord2f(pCF->u1[C], pCF->v2[C]);	glVertex3f(pMatrix[Y][X].X, pMatrix[Y][X].Y, pMatrix[Y][X].Z);							// Top Left
					glTexCoord2f(pCF->u1[C], pCF->v1[C]);	glVertex3f(pMatrix[Y][X].X, pMatrix[Y + 1][X].Y, pMatrix[Y + 1][X].Z);					// Bottom Left
					glTexCoord2f(pCF->u2[C], pCF->v1[C]);	glVertex3f(pMatrix[Y + 1][X + 1].X, pMatrix[Y + 1][X + 1].Y, pMatrix[Y + 1][X + 1].Z);	// Bottom Right
					glTexCoord2f(pCF->u2[C], pCF->v2[C]);	glVertex3f(pMatrix[Y][X + 1].X, pMatrix[Y][X].Y, pMatrix[Y][X + 1].Z);					// Top Right*/
					glEnd();
				}
				MapStringIndex = 0;
			}

	glPopMatrix();
	glFinish();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Quadrix::CalcNormal()
{
	Vec3D vLeftEdge, vBotEdge;
	vLeftEdge.X = pTopLeft.X - pBotLeft.X;
	vLeftEdge.Y = pTopLeft.Y - pBotLeft.Y;
	vLeftEdge.Z = pTopLeft.Z - pBotLeft.Z;
	vBotEdge.X = pBotRight.X - pBotLeft.X;
	vBotEdge.Y = pBotRight.Y - pBotLeft.Y;
	vBotEdge.Z = pBotRight.Z - pBotLeft.Z;
	Normal =  vBotEdge ^ vLeftEdge;
	Normal.Normalize();
	//p->Info("Normal.X = %f, Normal.Y = %f, Normal.Z = %f\n", Normal.X, Normal.Y, Normal.Z);
}

void Quadrix::SetCursorPos(double X, double Y)
{
	// This is the magic function, it can accept values larger than the grid size, and adjust scrolling accordingly
	ParseStringMap();

	// First we process the Y Coordinate
	if(Y + Scroll_Y > StrMap.size())
	{
		if(Y > StrMap.size())
		{
			Cursor.Y = Grid_Y - 1;
			Scroll_Y = Y - StrMap.size();
		}
		else
			Cursor.Y = Grid_Y - 1;
		Scroll_Y = StrMap.size() - Grid_Y;
		if(Scroll_Y < 0)
			Scroll_Y = 0;
	}
	else
	{
		if(Y + Scroll_Y < 0)
		{
			Cursor.Y = 0;
			Scroll_Y = 0;
		}

		else
		{
			if(Y >= Grid_Y)
			{
				Cursor.Y = Grid_Y - 1;
				Scroll_Y = Y - Grid_Y - 1;
			}
			else
			{
				if(Y < 0)
				{
					Scroll_Y += Y;
					Cursor.Y = 0;
				}
				else
					Cursor.Y = Y;
			}
		}
	}

	// Now we process the X Coordinate with the updated Cursor.Y value
	if(X + Scroll_X >= StrMap[Cursor.Y + Scroll_Y].size())
	{
		if(StrMap[Cursor.Y + Scroll_Y].size() >= (unsigned int)Grid_X)
		{
			Cursor.X = Grid_X - 1;
			Scroll_X = StrMap[Cursor.Y + Scroll_Y].size() - Grid_X;
		}
		else
		{
			Cursor.X = StrMap[Cursor.Y + Scroll_Y].size() - 1;
			Scroll_X = 0;
		}
	}
	else
	{
		if(Cursor.X + Scroll_X < 0)
		{
			Cursor.X = 0;
			Scroll_X = 0;
		}
		else
			Cursor.X = X;
	}
	GetStrPos();
}

void Quadrix::CursorLeft()
{
	ParseStringMap();
	tCursor.Start();
	if(Cursor.X + Scroll_X > 0)
	{
		if(Cursor.X > 0)
			SetCursorPos(Cursor.X - 1, Cursor.Y);
		else
			Scroll_X--;
	}
	else
	{	// Else we at X = 0, so need to wrap around to previous line;
		if(Scroll_Y + Cursor.Y) 	// Check we are not at beginning line
		{
			if(Cursor.Y > 0)
			{
				if(StrMap[Cursor.Y + Scroll_Y - 1].size() > (unsigned int)Grid_X)
				{
					Scroll_X = StrMap[Cursor.Y + Scroll_Y - 1].size() - Grid_X;
					SetCursorPos(Grid_X, Cursor.Y - 1);
				}
				else
					SetCursorPos(StrMap[Cursor.Y + Scroll_Y - 1].size() - 1, Cursor.Y - 1);
			}

			else		// Else cursor Y = 0; so we need to scroll up
			{
				if(Scroll_Y > 0)
				{
					Scroll_Y--;
					SetCursorPos(StrMap[Cursor.Y + Scroll_Y].size() - 1, Cursor.Y);
				}
			}
		}
	}
}

void Quadrix::CursorRight()
{
	ParseStringMap();
	tCursor.Start();
	if((unsigned int)Cursor.X + Scroll_X < StrMap[Cursor.Y + Scroll_Y].size() - 1)
	{
		if(Cursor.X < Grid_X - 1)
			SetCursorPos(Cursor.X + 1, Cursor.Y);
		else
			Scroll_X++;
	}
	else
	{	// Go to new line
		if((unsigned int)Cursor.Y + Scroll_Y + 1 < StrMap.size())
		{
			Cursor.X = 0;
			Scroll_X = 0;
			if(Cursor.Y + 1 < Grid_Y)
				Cursor.Y++;
			else
				Scroll_Y++;
		}
	}
}

void Quadrix::CursorDown()
{
	ParseStringMap();
	tCursor.Start();
	if((unsigned int)Cursor.Y + Scroll_Y < StrMap.size() - 1)
	{
		if(Cursor.Y < Grid_Y - 1)
		{
			if((unsigned int)Cursor.X + Scroll_X >= StrMap[Cursor.Y + Scroll_Y + 1].size())
				Cursor.X = StrMap[Cursor.Y + Scroll_Y + 1].size() - 1;

			SetCursorPos(Cursor.X, Cursor.Y + 1);
		}
		else
		{
			if((unsigned int)Cursor.X + Scroll_X >= StrMap[Cursor.Y + Scroll_Y + 1].size())
			{
				if(StrMap[Cursor.Y + Scroll_Y + 1].size() > (unsigned int)Grid_X)
				{
					Cursor.X = Grid_X - 1;
					Scroll_X = StrMap[Cursor.Y + Scroll_Y + 1].size() - Grid_X;
				}
				else
				{
					Cursor.X = StrMap[Cursor.Y + Scroll_Y + 1].size() - 1;
					Scroll_X = 0;
				}
			}
			Scroll_Y++;
		}
	}
}

void Quadrix::CursorUp()
{
	ParseStringMap();
	tCursor.Start();
	if(Cursor.Y + Scroll_Y)
	{
		if(Cursor.Y)
		{
			if((unsigned int)Cursor.X + Scroll_X >= StrMap[Cursor.Y + Scroll_Y - 1].size())
				Cursor.X = StrMap[Cursor.Y + Scroll_Y - 1].size() - 1;

			SetCursorPos(Cursor.X, Cursor.Y - 1);
		}
		else
		{
			if((unsigned int)Cursor.X + Scroll_X >= StrMap[Cursor.Y + Scroll_Y - 1].size())
			{
				if(StrMap[Cursor.Y + Scroll_Y - 1].size() > (unsigned int)Grid_X)
				{
					Cursor.X = Grid_X - 1;
					Scroll_X = StrMap[Cursor.Y + Scroll_Y - 1].size() - Grid_X;
				}
				else
				{
					Cursor.X = StrMap[Cursor.Y + Scroll_Y - 1].size() - 1;
					Scroll_X = 0;
				}
			}
			Scroll_Y--;
		}
	}
}

void Quadrix::AddCharacter(char *pC)
{
	char C = *pC;

	if(StrPos < StrData.size())
	{
		StrData.insert(StrPos, (string)&C);
		CursorRight();
	}
	if(StrPos == StrData.size())
	{
		StrData.append((string)&C);
		CursorRight();
	}
}

char Quadrix::GetChar()
{
	char c = 0;

	if(StrPos < StrData.size())
		c = (char)StrData[StrPos];
	if(StrPos == StrData.size())
		c = ' ';

	return c;
}

void Quadrix::Return()
{
	char C = '\n';
	StrData.insert(StrPos, (string)&C);
	if(Cursor.Y + 1 < Grid_Y)
		SetCursorPos(0, Cursor.Y + 1);
	else
	{
		SetCursorPos(0, Cursor.Y);
		Scroll_Y++;
	}
	Scroll_X = 0;
}

void Quadrix::Backspace()
{
	if(StrData.size() && StrPos > 0)
	{
		StrData.erase(StrPos - 1, 1);
		if(Cursor.X + Scroll_X)
			CursorLeft();
		else		// At beginning of line
			SetCursorPos(StrMap[Cursor.Y + Scroll_Y - 1].size() - 1, Cursor.Y - 1);
	}
}

void Quadrix::Delete()
{
	if(StrData.size())
		StrData.erase(StrPos, 1);
	ParseStringMap();
}

void Quadrix::Home()
{
	if(Cursor.X == 0)
		Scroll_X = 0;
	else
		SetCursorPos(0, Cursor.Y);
}

void Quadrix::End()
{
	ParseStringMap();
	SetCursorPos(StrMap[Cursor.Y + Scroll_Y].size() - 1, Cursor.Y);
}

void Quadrix::PageUp()
{
	if(Cursor.Y == 0)
		SetCursorPos(Cursor.X, -Grid_Y);
	else
		SetCursorPos(Cursor.X, 0);
}

void Quadrix::PageDown()
{
	if(Cursor.Y == Grid_Y - 1)
		SetCursorPos(Cursor.X, Grid_Y + Grid_Y + Scroll_Y + 1);
	else
		SetCursorPos(Cursor.X, Grid_Y - 1 + Scroll_Y);
}

void Quadrix::Save()
{
	FILE *pFile;
	pFile = fopen("quadrix.txt", "wt");
	fprintf(pFile, "%s", StrData.c_str());
	fclose(pFile);
	p->Info("quadrix.txt saved\n");
}
