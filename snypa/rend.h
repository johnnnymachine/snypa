#ifndef REND_H_
#define REND_H_

#include <GL/glu.h>
#include <GL/glext.h>
#include <math.h>
#include "quadrix.h"
#include "path.h"
#include "bezier.h"
#include "ttf.h"

void FracPoint(unsigned char *pAddr, int x, int y);
void FracLoop(unsigned char *pAddr, int YSeg, int Segments);

class Rend	// GL Rendering
{
public:
	bool 			bActive;				// Rendering Enable/Disable
	bool			bInfo;					// On screen program info
	int				InfoRate;				// Update every N'th frame. Used for drawing some of the values at a slower rate.
	bool 			bBlend;					// Blending
	bool			bDepthTest;				// Depth Test check
	int 			Width;
	int 			Height;
	float			Ratio;
	float			FOV;					// Field Of View (Degrees)
	unsigned int	Frame;
	float 			sinFrame, cosFrame;
	GLuint			ImageTexID;
	GLuint 			FractalTexID;
	unsigned char 	*pFractalBuf;
	int				FractalWidth;
	int				FractalHeight;
	float 			FractalZoom;
	float			FractalPanX;
	float			FractalPanY;
	Quadrix			*pQuadrix;
	Path			*pPath;
	Bezier			*pBezier;
	char 			sRunTime[100], sFrame[100], sEvents[100], sRender[100], sWait[100], sSleep[100], sFPS[100], sRate[100];
	char 			sJLX[100], sJLY[100], sJRX[100], sJRY[100];
	char 			sFractalZoom[100], sFractalPanX[100], sFractalPanY[100];
	int				Mode;
	int				ThreadCount;
	TTF				*pTTFSmall, *pTTFMed, *pTTFLarge;

					Rend();
					~Rend();
	bool 			Init();
	void 			Destroy();
	void			Render();
	void 			SetBlend(bool bState);
	void			SetDepthTest(bool bState);
	void 			ReShape(int Width, int Height);
    void 			CalcFractal();
    void 			CalcFrac();
    unsigned char	FractalPoint(int X, int Y);
    void 			DrawSOD2D();
    void 			DrawSOD2DStatic();
    void 			DrawTexture(GLuint TexID);
    void 			DrawTexture2D(GLuint TexID);
    void 			DrawInfo();
    void 			ScreenShot();
};

#endif /* REND_H_ */
