#include <stdio.h>
#include <stdlib.h>

#include "ttf.h"
#include "core.h"

//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" /* http://nothings.org/stb/stb_image_write.h */

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h" /* http://nothings.org/stb/stb_truetype.h */

TTF::TTF()
{
	Width = 32;
	Height = 64;
	X_Offset = 2;
	sFontFile = "JetBrainsMono-Medium.ttf";
}

bool TTF::Init()
{
	glGenTextures(128, pTextures);

	/* load font file */
    long size;
    unsigned char* fontBuffer;

    //FILE* fontFile = fopen("Terminus.ttf", "rb");
    FILE* fontFile = fopen(sFontFile.c_str(), "rb");
    if(fontFile == NULL)
   	{
    	string sError = "<Error> Font File '" + sFontFile + "' not found.\n";
    	p->Info(sError.c_str());
    	return false;
   	}
    fseek(fontFile, 0, SEEK_END);
    size = ftell(fontFile); /* how long is the file ? */
    fseek(fontFile, 0, SEEK_SET); /* reset */

    fontBuffer = (unsigned char*)malloc(size);

    fread(fontBuffer, size, 1, fontFile);
    fclose(fontFile);

    /* prepare font */
    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, fontBuffer, 0))
    {
        p->Info("<Error> stbtt_InitFont() failed\n");
        return false;
    }

    int b_w = Width; /* bitmap width */
    int b_h = Height; /* bitmap height */
    int l_h = Height; /* line height */

    /* create a bitmap for the phrase */
    unsigned char* bitmap, *bitmap32;

    /* calculate font scaling */
    float scale = stbtt_ScaleForPixelHeight(&info, l_h);
	char c = 0;
	int ascent, descent, lineGap;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
	ascent *= scale;
	descent *= scale;

	for (c = 0; c < 127; c++)
	{
		bitmap = (unsigned char *)malloc(b_w * b_h);
		bitmap32 = (unsigned char *)malloc(b_w * b_h * 4);
		memset(bitmap, 0, b_w * b_h);
		memset(bitmap32, 0, b_w * b_h * 4);
		// get bounding box for character (may be offset to account for chars that dip above or below the line
		int c_x1, c_y1, c_x2, c_y2;
		stbtt_GetCodepointBitmapBox(&info, c, scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
		// compute y (different characters have different heights
		int y = ascent + c_y1;
		// render character (stride and offset is important here)
		int byteOffset = X_Offset + (y  * b_w); // offset x to center the char
		stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, c);

		char out[256] = {0}, out32[256] = {0};
		sprintf(out, "out-%d.png", c);
		// save out a 1 channel image
		//stbi_write_png(out, b_w, b_h, 1, bitmap, b_w);

		// Convert 8 bit bitmap into 32 bit bitmap
		for(int i = 0; i < b_w * b_h; i++)
		{
			bitmap32[i * 4] 	= bitmap[i];
			bitmap32[i * 4 + 1] = bitmap[i];
			bitmap32[i * 4 + 2] = bitmap[i];
			bitmap32[i * 4 + 3] = bitmap[i];
		}
		sprintf(out32, "out32-%d.png", c);
		//stbi_write_png(out32, b_w, b_h, 4, bitmap32, b_w * 4);

		glBindTexture(GL_TEXTURE_2D, pTextures[(int)c]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, b_w, b_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap32);

		free(bitmap);
		free(bitmap32);
	}

	free(fontBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

void TTF::Char2D(char c, float X, float Y)
{
	glPushMatrix();
		glTranslatef(X, Y, 0);
		glColor4f(0, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, pTextures[(int)c]);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 1);	glVertex2f(0, 0);
		glTexCoord2f(1, 1);	glVertex2f(Width, 0);
		glTexCoord2f(1, 0);	glVertex2f(Width, Height);
		glTexCoord2f(0, 0);	glVertex2f(0, Height);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}

void TTF::Char3D(char c, float X, float Y, float Z)
{
	glPushMatrix();
		glTranslatef(X, Y, Z);
		glColor4f(0, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, pTextures[(int)c]);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0);		glVertex3f((float)-1 / 32, (float)1 / 32, 0);
			glTexCoord2f(1, 0);		glVertex3f((float)1 / 32, (float)1 / 32, 0);
			glTexCoord2f(1, 1);		glVertex3f((float)1 / 32, (float)-1 / 32, 0);
			glTexCoord2f(0, 1);		glVertex3f((float) -1 / 32, (float)-1 / 32, 0);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}

void TTF::Text2D(char *sStr, float X, float Y)
{
	int X1, X2, Y1, Y2;
	glPushMatrix();
		glTranslatef(X, Y, 0);

		for(uint x = 0; x < strlen(sStr); x++)
		{
			X1 = x * Width;			X2 = X1 + Width;			Y2 = 0;			Y1 = -Height;	// We Invert the Y Axis

			glBindTexture(GL_TEXTURE_2D, pTextures[(int)sStr[x]]);
			glBegin(GL_QUADS);
			glTexCoord2f(0, 1);	glVertex2f(X1, Y1);
			glTexCoord2f(0, 0);	glVertex2f(X1, Y2);
			glTexCoord2f(1, 0);	glVertex2f(X2, Y2);
			glTexCoord2f(1, 1);	glVertex2f(X2, Y1);
			glEnd();
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();


}
