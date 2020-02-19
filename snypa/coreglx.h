#ifndef COREGLX_H_
#define COREGLX_H_

// Required for glXSwapIntervalEXT()
#define GLX_GLXEXT_PROTOTYPES

#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>
#include 	<unistd.h>
#include	<string.h>
#include 	<X11/Xlib.h>
#include 	<X11/Xutil.h>
#include 	<GL/gl.h>
#include 	<GL/glx.h>
#include 	<X11/extensions/xf86vmode.h>
#include 	<GL/glxext.h>

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

class CoreGLX
{
public:
	Display 				*pXDisplay;
	bool					bDoubleBuffer;
	Window					XWindow;
	int						Width, Height;
	Atom					XAtom;
	GLXContext				XGLXContext;
	XF86VidModeModeInfo 	**pXF86VidModeModeInfo;
	int						XMode, nXModes;
	int						nXScreens, cXScreen;
	XEvent					gXEvent;
	bool 					bVSync;
	bool 					bFullScreen;
	int						WinWidth, WinHeight;
	GLXFBConfig 			BestFBConfig;

							CoreGLX();
	bool 					Init();
	void 					Destroy();
	bool 					IsExtensionSupported(const char *extList, const char *extension);
	void 					Events();
	void 					ReShape();
	void 					PrintModes();
	void 					SetVSync(bool bState);
	void 					SetXMode(int XMode);
	void 					SetFullScreen(bool bState);
	bool					Init(bool bFS);
};

#endif /* COREGLX_H_ */
