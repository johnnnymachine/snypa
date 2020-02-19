#ifndef TTF_H_
#define TTF_H_

#include <GL/gl.h>
#include "vector.h"
#include <string>

using namespace std;

class TTF
{
public:
	uint Height;
	uint Width;
	uint X_Offset;
	GLuint pTextures[128];
	string sFontFile;

	bool Init();
	void Char2D(char c, float X, float Y);
	void Char3D(char c, float X, float Y, float Z);
	void Text2D(char *sStr, float X, float Y);

	TTF();
};

#endif /* TTF_H_ */
