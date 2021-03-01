#include "core.h"
#include "rend.h"
#include <boost/filesystem.hpp>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

using namespace std;

Rend::Rend()
{
	p->Info("Rend.Rend()\n");
	bActive			= true;
	bInfo			= true;
	InfoRate		= 10;
	bBlend			= true;
	bDepthTest		= true;
	Width			= 1920;
	Height			= 1080;
	Ratio			= (float)Width / (float)Height;
	FOV				= 45;
	Frame			= 0;
	sinFrame		= sin(0);
	cosFrame		= cos(0);
	ImageTexID		= 0;
	FractalTexID 	= 0;
	FractalWidth	= Width;
	FractalHeight	= Height;
	FractalZoom		= 1.0;
	FractalPanX		= 0.0;
	FractalPanY		= 0.0;
	pFractalBuf 	= new unsigned char[FractalWidth * FractalHeight * 4];
	pQuadrix 		= new Quadrix;
	pPath 			= new Path;
	pBezier			= new Bezier;
	Mode			= 1;
	ThreadCount 	= std::thread::hardware_concurrency();
	if(ThreadCount == 0) ThreadCount = 12;
	pTTFSmall		= NULL;
	pTTFMed			= NULL;
	pTTFLarge		= NULL;

}

Rend::~Rend()
{
	p->Info("Rend.~Rend()\n");
	if(pQuadrix)		delete pQuadrix;
	if(pFractalBuf)		delete [] pFractalBuf;
	if(pPath) 			delete pPath;
	if(pBezier)			delete pBezier;
	if(pTTFSmall)		delete pTTFSmall;
	if(pTTFMed)			delete pTTFMed;
	if(pTTFLarge)		delete pTTFLarge;
}

bool Rend::Init()
{
	p->Info("Rend.Init()\n");
	ReShape(p->pCoreGLX->Width, p->pCoreGLX->Height);

	glEnable(GL_TEXTURE_2D);
	SetBlend(bBlend);

	pTTFSmall = new TTF;
	pTTFSmall->Height = 32;		pTTFSmall->Width = 16;	pTTFSmall->X_Offset = 1;
	pTTFSmall->Init();
	pTTFMed = new TTF;
	pTTFMed->Height = 64;		pTTFMed->Width = 32;	pTTFMed->X_Offset = 2;
	pTTFMed->Init();
	pTTFLarge = new TTF;
	pTTFLarge->Height = 128;	pTTFLarge->Width = 64;	pTTFLarge->X_Offset = 4;
	pTTFLarge->Init();


	int ImgWidth, ImgHeight, nChannels;
	unsigned char *ImgData = stbi_load("image.png", &ImgWidth, &ImgHeight, &nChannels, 0);
	//stbi_write_png("image-output.png", ImgWidth, ImgHeight, nChannels, ImgData, ImgWidth * nChannels);
	glGenTextures(1, &ImageTexID);
	glBindTexture(GL_TEXTURE_2D, ImageTexID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	if(nChannels == 3)		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ImgWidth, ImgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, ImgData);
	if(nChannels == 4)		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImgWidth, ImgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, ImgData);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(ImgData);

	glGenTextures(1, &FractalTexID);
	pQuadrix->Init();

	pPath->sPath = "/";
	string sError = pPath->Load();
	if(sError == "")
		pQuadrix->StrData = pPath->StrPaths();
	else
		p->Info("<Error> Rend.Init() - %s\n", sError.c_str());

	return bActive = true;
}

void Rend::Destroy()
{
	p->Info("Rend.Destroy()\n");
}

void Rend::Render()
{
	Frame++;
	sinFrame = sin((float)(Frame) / 180 * M_PI);
	cosFrame = cos((float)(Frame) / 180 * M_PI);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(p->pCoreGLX->bDoubleBuffer)		glDrawBuffer(GL_BACK);
	else								glDrawBuffer(GL_FRONT);

	// 3D
	glMatrixMode(GL_PROJECTION);	// Camera Matrix
	glLoadIdentity();
	gluPerspective(FOV, Ratio, 1, 255);
	glMatrixMode(GL_MODELVIEW);		// Objects Matrix
	glLoadIdentity();
	gluLookAt(0, 0, 4, 0, 0, 0, 0, 1, 0);

	switch(Mode)
	{
		case 1:		DrawTexture(ImageTexID);								break;
		case 2:		CalcFrac();				// Multi Thread Version
					// CalcFractal();		// Single Thread Version
					DrawTexture(FractalTexID);								break;
		case 3:		DrawSOD2D();											break;
		case 4:		glColor4f(0, 1, 0, 1);
					pQuadrix->Render3D();									break;
		case 5:		pBezier->Render3D();									break;
		case 8:		DrawSOD2DStatic();										break;
	}

	// 2D
	glMatrixMode(GL_PROJECTION);	// Camera Matrix
	glLoadIdentity();
	glOrtho(0, Width, 0, Height, -1, 1);
	glMatrixMode(GL_MODELVIEW);		// Objects Matrix
	glLoadIdentity();

	if(Mode == 6)	pQuadrix->Render2D();
	if(Mode == 7)
	{
		CalcFrac();
		DrawTexture2D(FractalTexID);
	}

	if(bInfo) DrawInfo();

	if(p->pCoreGLX->bDoubleBuffer)		glXSwapBuffers(p->pCoreGLX->pXDisplay, p->pCoreGLX->XWindow);
	else								glFinish();

}

void Rend::SetDepthTest(bool bState)
{
	p->Info("Rend.SetDepthTest(%d)\n", bState);
	bDepthTest = bState;
	if(bState)	glEnable(GL_DEPTH_TEST);
	else		glDisable(GL_DEPTH_TEST);

}
void Rend::SetBlend(bool bState)
{
	p->Info("Rend.SetBlend(%d)\n", bState);
	bBlend = bState;
	if(bState == true)
	{	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// Transparency, Points, Lines
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_LINE_SMOOTH);
		//glEnable(GL_POLYGON_SMOOTH);		// With quads you can see the diagonal line produced between the two triangles, so leave disabled
		glEnable(GL_BLEND);
	}
	else glDisable(GL_BLEND);
}

void Rend::ReShape(int NewWidth, int NewHeight)
{
	Width = NewWidth;
	Height = NewHeight;
	Ratio = (float)Width / (float)Height;
	glViewport(0, 0, Width, Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FracPoint(unsigned char *pAddr, int x, int y)
{
	float zx = (((float)x / p->pRend->FractalWidth) * 4 * (p->pRend->FractalZoom)) - 2 + p->pRend->FractalPanX;
	float zy = (((float)y / p->pRend->FractalHeight) * 4 * (p->pRend->FractalZoom)) - 2 + p->pRend->FractalPanY;

	//float zx = (((float)x / p->pRend->FractalWidth) * 4 * JLY) - 2 - JRX * 5 - JLY * 2.5;
	//float zy = (((float)y / p->pRend->FractalHeight) * 4 * JLY) - 2 + JRY  * 5 - JLY * 2.5;

	float cx = JLX;
	float cy = -JLY;
	float newzx, newzy;
	unsigned char i;

	for(i = 0; i < 32 && (zx * zx) + (zy * zy) < 4; i++)
	{	newzx = zx * zx - zy * zy + cx;
		newzy = 2 * zx * zy + cy;
		zx = newzx;
		zy = newzy;
	}
	if(i >= 32)	i = 0;
	*pAddr = i * 8;
}

void FracLoop(unsigned char *pAddr, int YSeg, int Segments)
{
	for(int y = YSeg * p->pRend->FractalHeight / Segments; y < (YSeg + 1) * p->pRend->FractalHeight / Segments; y++)
		for(int x = 0; x < p->pRend->FractalWidth; x++)
		{
			FracPoint(pAddr + (y * p->pRend->FractalWidth * 4) + x * 4, x, y);
			*(pAddr + (y * p->pRend->FractalWidth * 4) + x * 4 + 3) = 0xff;					// Set the alpha channel to 255
		}
}

void Rend::CalcFrac()
{
/*    FractalZoom += -JLY / 100;
    if(FractalZoom < 0.0) FractalZoom = 0.0;
    FractalPanX += JRX / 10;
    if(FractalPanX < 0.0) FractalPanX = 0.0;
    FractalPanY += JRY / 10;
    if(FractalPanY < 0.0) FractalPanY = 0.0;
    */

	vector<thread> Threads;

    for(int i = 0; i < ThreadCount; i++)
    {
    	thread T(FracLoop, pFractalBuf, i,  ThreadCount);
        Threads.push_back(move(T));
    }

    for(auto &T : Threads)
    {
    	T.join();
    }

	glBindTexture(GL_TEXTURE_2D, FractalTexID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FractalWidth, FractalHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pFractalBuf);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Rend::CalcFractal()
{
	for(int y = 0; y < FractalHeight; y++)
		for(int x = 0; x < FractalWidth; x++)
		{
			pFractalBuf[(y * FractalWidth  * 4) + x * 4] = FractalPoint(x, y);
			pFractalBuf[(y * FractalWidth * 4) + x * 4 + 3] = 0xff;					// Set the alpha channel to 255
		}

	glBindTexture(GL_TEXTURE_2D, FractalTexID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FractalWidth, FractalHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pFractalBuf);
	glBindTexture(GL_TEXTURE_2D, 0);
}


unsigned char Rend::FractalPoint(int x, int y)
{
	float zx = (((float)x / FractalWidth) * 4) - 2;
	float zy = (((float)y / FractalHeight) * 4) - 2;
 	float cx = JLX;
	float cy = -JLY;
	float newzx, newzy;
	unsigned char i;

	for(i = 0; i < 32 && (zx * zx) + (zy * zy) < 4; i++)
	{	newzx = zx * zx - zy * zy + cx;
		newzy = 2 * zx * zy + cy;
		zx = newzx;
		zy = newzy;
	}

	if(i >= 32)	return 0;
	return i * 8;
}

void Rend::DrawSOD2D()
{
	glLineWidth(1);
	int Lines = 100;
	for(int i = 0; i < Lines; i++)
	{	glPushMatrix();
			glRotatef((sinFrame) * (Frame + i), 0, 0, 1.0);
			glColor3f(0.7, 0, 0);
			glBegin(GL_LINES);
				glVertex3f(0, 1, sinFrame);						glVertex3f(sin(M_PI/3), -0.5, 3 * sinFrame);
				glVertex3f(sin(M_PI/3), -0.5, 3 * sinFrame);	glVertex3f(-sin(M_PI/3), -0.5, 3 * sinFrame);
				glVertex3f(-sin(M_PI/3), -0.5, 3 * sinFrame);	glVertex3f(0, 1, sinFrame);
			glEnd();
		glPopMatrix();

		glPushMatrix();
			glRotatef((cosFrame) * (Frame + i), 0, 0, 1.0);
			glColor3f(0, 0.7, 0.0);
			glBegin(GL_LINES);
				glVertex3f(0, -1, cosFrame);					glVertex3f(-sin(M_PI/3), 0.5, 3 * cosFrame);
				glVertex3f(-sin(M_PI/3), 0.5, 3 * cosFrame);	glVertex3f(sin(M_PI/3), 0.5, 3 * cosFrame);
				glVertex3f(sin(M_PI/3), 0.5, 3 * cosFrame);		glVertex3f(0, -1, 3 * cosFrame);
			glEnd();
		glPopMatrix();

		glPushMatrix();
			glRotatef((-sinFrame) * (Frame + i), 0, 0, 1.0);
			glColor3f(0.0, 0.0, 0.7);
			glBegin(GL_LINES);
				glVertex3f(0, 1, -sinFrame);					glVertex3f(-sin(M_PI/3), -0.5, -sinFrame);
				glVertex3f(-sin(M_PI/3), -0.5, -sinFrame);		glVertex3f(sin(M_PI/3), -0.5, -sinFrame);
				glVertex3f(sin(M_PI/3), -0.5, -sinFrame);		glVertex3f(0, 1, -sinFrame);
			glEnd();
		glPopMatrix();
	}
}

void Rend::DrawSOD2DStatic()
{
	glLineWidth(10);
	glTranslatef(0,0,1);
	glPushMatrix();
		glRotatef(sin(0), 0, 0, 1.0);
		glColor3f(0.7, 0, 0);
		glBegin(GL_LINES);
			glVertex3f(0, 1, 0);					glVertex3f(sin(M_PI/3), -0.5, 0);
			glVertex3f(sin(M_PI/3), -0.5, 0);	glVertex3f(-sin(M_PI/3), -0.5, 0);
			glVertex3f(-sin(M_PI/3), -0.5, 0);	glVertex3f(0, 1, 0);
		glEnd();
	glPopMatrix();

	glPushMatrix();
		glRotatef(cos(0), 0, 0, 1.0);
		glColor3f(0, 0.7, 0.0);
		glBegin(GL_LINES);
			glVertex3f(0, -1, 0);					glVertex3f(-sin(M_PI/3), 0.5, 0);
			glVertex3f(-sin(M_PI/3), 0.5, 0);	glVertex3f(sin(M_PI/3), 0.5, 0);
			glVertex3f(sin(M_PI/3), 0.5, 0);	glVertex3f(0, -1, 0);
		glEnd();
	glPopMatrix();

	glLineWidth(3);
	glPushMatrix();
		glRotatef(sin(0), 0, 0, 1.0);
		glColor3f(0.7, 0.7, 0);
		glBegin(GL_LINES);
			glVertex3f(0, 1, 0);					glVertex3f(sin(M_PI/3), -0.5, 0);
			glVertex3f(sin(M_PI/3), -0.5, 0);	glVertex3f(-sin(M_PI/3), -0.5, 0);
			glVertex3f(-sin(M_PI/3), -0.5, 0);	glVertex3f(0, 1, 0);
		glEnd();
	glPopMatrix();

	glPushMatrix();
		glRotatef(cos(0), 0, 0, 1.0);
		glColor3f(0.7, 0.7, 0.0);
		glBegin(GL_LINES);
			glVertex3f(0, -1, 0);					glVertex3f(-sin(M_PI/3), 0.5, 0);
			glVertex3f(-sin(M_PI/3), 0.5, 0);	glVertex3f(sin(M_PI/3), 0.5, 0);
			glVertex3f(sin(M_PI/3), 0.5, 0);	glVertex3f(0, -1, 0);
		glEnd();
	glPopMatrix();

}

void Rend::DrawTexture(GLuint TexID)
{
	glPushMatrix();
		glTranslatef(sinFrame, cosFrame, 0);
		//glRotatef(JRX * 100, 0, 1.0, 0);
		//glRotatef(JRY * 100, 1.0, 0, 0);
		glColor4f(1, 1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, TexID);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0);		glVertex3f(-1, 1, 0);
			glTexCoord2f(1, 0);		glVertex3f(1, 1, 0);
			glTexCoord2f(1, 1);		glVertex3f(1, -1, 0);
			glTexCoord2f(0, 1);		glVertex3f(-1, -1, 0);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}

void Rend::DrawTexture2D(GLuint TexID)
{
	glPushMatrix();
		glColor4f(1, 1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, TexID);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0);		glVertex2f(0, 0);
			glTexCoord2f(1, 0);		glVertex2f(p->pCoreGLX->Width, 0);
			glTexCoord2f(1, 1);		glVertex2f(p->pCoreGLX->Width, p->pCoreGLX->Height);
			glTexCoord2f(0, 1);		glVertex2f(0, p->pCoreGLX->Height);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}

void Rend::DrawInfo()
{
	// Some values get updated every 10 frames so u can actually read them
	glColor4f(0, 1, 0, 1);
	if(Frame % InfoRate == 0)		sprintf(sWait, "   Wait (us): %5.0f", p->ptWait->Delta * 1e6);
	pTTFSmall->Text2D(sWait, Width - 384, 48);
	if(Frame % InfoRate == 0)		sprintf(sSleep, "  Sleep (us): %5.0f", (p->ptWait->Delta - p->ptEvents->Delta - p->ptRender->Delta) * 1e6);
	pTTFSmall->Text2D(sSleep, Width - 384, 80);
	if(Frame % InfoRate == 0)		sprintf(sEvents, " Events (us): %5.0f", p->ptEvents->Delta * 1e6);
	pTTFSmall->Text2D(sEvents, Width - 384, 112);
	if(Frame % InfoRate == 0)		sprintf(sRender, " Render (us): %5.0f", p->ptRender->Delta * 1e6);
	pTTFSmall->Text2D(sRender, Width - 384, 140);
	if(Frame % InfoRate == 0)		sprintf(sFPS, "         FPS:   %07.3f", 1.0 / p->ptWait->Delta);
	pTTFSmall->Text2D(sFPS, Width - 384, 172);
	sprintf(sRate, "        Rate: %5.0f", p->Rate);
	pTTFSmall->Text2D(sRate, Width - 384, 204);
	sprintf(sFrame, "       Frame: %5d", p->pRend->Frame);
	pTTFSmall->Text2D(sFrame, Width - 384, 236);
	p->ptRunTime->Stop();
	sprintf(sRunTime, "Run Time (s):   %07.3f", p->ptRunTime->Delta);
	pTTFSmall->Text2D(sRunTime, Width - 384, 268);

	sprintf(sJRY, "         JRY:   %01.3f", JRY);
	pTTFSmall->Text2D(sJRY, Width - 384, 300);
	sprintf(sJRX, "         JRX:   %01.3f", JRX);
	pTTFSmall->Text2D(sJRX, Width - 384, 332);
	sprintf(sJLY, "         JLY:   %01.3f", JLY);
	pTTFSmall->Text2D(sJLY, Width - 384, 364);
	sprintf(sJLX, "         JLX:   %01.3f", JLX);
	pTTFSmall->Text2D(sJLX, Width - 384, 396);
}

void Rend::ScreenShot()
{
	unsigned char *ImgDataRow = new unsigned char[Width * 4];
	unsigned char *ImgData = new unsigned char[Width * Height * 4];

	int Index = 0;
	string sIndex = to_string(Index);
	string sFileName = "screenshot" + string(5 - sIndex.length(), '0') + sIndex + ".png";

	for(; boost::filesystem::exists(sFileName); Index++)
	{
		sIndex = to_string(Index);
		sFileName = "screenshot" + string(5 - sIndex.length(), '0') + sIndex + ".png";
	}
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	for(int y = 0; y < Height; y++)			// We need invert the image (Y-Axis), so load one row at a time and copy into buffer
	{
		glReadPixels(0, y, Width, 1, GL_RGBA, GL_UNSIGNED_BYTE, ImgDataRow);
		memcpy(ImgData + ((Height - y - 1) * Width * 4), ImgDataRow, Width * 4);
	}

	stbi_write_png(sFileName.c_str(), p->pCoreGLX->Width, p->pCoreGLX->Height, 4, ImgData, p->pCoreGLX->Width * 4);
	string Info = "Screenshot - " + sFileName + " saved\n";
	p->Info(Info.c_str());
}
