#include "coreglx.h"
#include "core.h"

static bool ctxErrorOccurred = false;
static int ctxErrorHandler(Display *dpy, XErrorEvent *ev)
{
    ctxErrorOccurred = true;
    return 0;
}

CoreGLX::CoreGLX()
{
	bDoubleBuffer = true;
	Width = 1920;
	Height = 1080;
	XMode = 0;
	nXModes = 0;
	bVSync = true;
	WinWidth = 1920;
	WinHeight = 1080;
	bFullScreen = false;
	cXScreen = 0;
	nXScreens = 0;
	XGLXContext = NULL;
	BestFBConfig = NULL;
	pXDisplay = NULL;
	pXF86VidModeModeInfo = NULL;
	XAtom = 0;
	XWindow = 0;
}

bool CoreGLX::Init()
{
	return Init(false);
}

bool CoreGLX::Init(bool bFS)
{
	p->Info("CoreGLX.Init()\n");
	pXDisplay = XOpenDisplay(NULL);
	nXScreens = XScreenCount(pXDisplay);
	cXScreen = DefaultScreen(pXDisplay);
	XF86VidModeGetAllModeLines(pXDisplay, cXScreen, &nXModes, &pXF86VidModeModeInfo);

	int *GLXAttributes;
	int Visual_AttribsDB[] =
	{
		GLX_X_RENDERABLE    , True,
		GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
		GLX_RENDER_TYPE     , GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
		GLX_RED_SIZE        , 8,
		GLX_GREEN_SIZE      , 8,
		GLX_BLUE_SIZE       , 8,
		GLX_ALPHA_SIZE      , 8,
		GLX_DEPTH_SIZE      , 24,
		GLX_STENCIL_SIZE    , 8,
		GLX_DOUBLEBUFFER    , True,
		GLX_SAMPLE_BUFFERS  , 1,
		GLX_SAMPLES         , 4,
		None
	};
	int Visual_AttribsSB[] =
	{
		GLX_X_RENDERABLE    , True,
		GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
		GLX_RENDER_TYPE     , GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
		GLX_RED_SIZE        , 8,
		GLX_GREEN_SIZE      , 8,
		GLX_BLUE_SIZE       , 8,
		GLX_ALPHA_SIZE      , 8,
		GLX_DEPTH_SIZE      , 24,
		GLX_STENCIL_SIZE    , 8,
		GLX_DOUBLEBUFFER    , False,
		GLX_SAMPLE_BUFFERS  , 1,
		GLX_SAMPLES         , 4,
		None
	};
	if(bDoubleBuffer)
		GLXAttributes = Visual_AttribsDB;
	else
		GLXAttributes = Visual_AttribsSB;

	int glx_major, glx_minor;	// FBConfigs were added in GLX version 1.3.
	if(!glXQueryVersion( pXDisplay, &glx_major, &glx_minor ) || ((glx_major == 1) && (glx_minor < 3) ) || (glx_major < 1))
	{
		p->Info("<Error> Invalid GLX Version.\n");
		return false;
	}

	int FBCount;
	GLXFBConfig* FBConfig = glXChooseFBConfig(pXDisplay, DefaultScreen(pXDisplay), GLXAttributes, &FBCount);
	if (!FBConfig)
	{
	    p->Info("<Error> Failed to retrieve a framebuffer config.\n");
	    return false;
	}
	// Pick the FB config/visual with the most samples per pixel
	int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
	for (int i = 0; i < FBCount; i++)
	{
	    XVisualInfo *vi = glXGetVisualFromFBConfig(pXDisplay, FBConfig[i]);
	    if (vi)
	    {
	    	int samp_buf, samples;
	    	glXGetFBConfigAttrib(pXDisplay, FBConfig[i], GLX_SAMPLE_BUFFERS, &samp_buf);
	    	glXGetFBConfigAttrib(pXDisplay, FBConfig[i], GLX_SAMPLES, &samples);
	    	if((best_fbc < 0 || samp_buf) && (samples > best_num_samp))	    		best_fbc = i, best_num_samp = samples;
	    	if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp)	    		worst_fbc = i, worst_num_samp = samples;
	    }
	    XFree(vi);
	}
	BestFBConfig = FBConfig[best_fbc];
	XFree(FBConfig);
	XVisualInfo *vi = glXGetVisualFromFBConfig(pXDisplay, BestFBConfig);
	XSetWindowAttributes swa;
	Colormap cmap;
	swa.colormap = cmap = XCreateColormap(pXDisplay, RootWindow(pXDisplay, vi->screen), vi->visual, AllocNone);
	swa.background_pixmap = None;
	swa.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask; // | ExposureMask;
	swa.border_pixel = 0;
	if(bFS)		// Fullscreen
	{
		Width = pXF86VidModeModeInfo[XMode]->hdisplay;
		Height = pXF86VidModeModeInfo[XMode]->vdisplay;
		swa.override_redirect = true;		// Dont use the window manager
		XWindow = XCreateWindow(pXDisplay, RootWindow(pXDisplay, vi->screen), 0, 0, Width, Height, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect, &swa);
		XMapRaised(pXDisplay, XWindow);
		XGrabKeyboard(pXDisplay, XWindow, True, GrabModeAsync, GrabModeAsync, CurrentTime);
	}
	else		//  Windowed
	{
		Width = WinWidth;
		Height = WinHeight;
		XWindow = XCreateWindow(pXDisplay, RootWindow(pXDisplay, vi->screen), 0, 0, Width, Height, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);
		XMapRaised(pXDisplay, XWindow);
	}

	if(!XWindow)
	{
		p->Info("<Error> Failed to create window.\n");
	    return false;
	}
	XFree(vi);
	XStoreName(pXDisplay, XWindow, "snypa");
	XMapWindow(pXDisplay, XWindow);

	const char *glxExts = glXQueryExtensionsString(pXDisplay, DefaultScreen(pXDisplay));
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc) glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");


	ctxErrorOccurred = false;
	int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&ctxErrorHandler);

	if(!IsExtensionSupported(glxExts, "GLX_ARB_create_context" ) || !glXCreateContextAttribsARB)
	{
		p->Info("<Error> glXCreateContextAttribsARB() not found.\n");
		return false;
	}

	else
	{
	    int Context_Attribs[] =
	    {
	        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,		// Open GL version 3.0
	        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
	        None
	    };
	    XGLXContext = glXCreateContextAttribsARB(pXDisplay, BestFBConfig, 0, True, Context_Attribs);
	    XSync(pXDisplay, False);	// Sync to ensure any errors generated are processed.
	}

	XSetErrorHandler(oldHandler);
	if(ctxErrorOccurred || !XGLXContext)
	{
	    p->Info("<Error> Failed to create an OpenGL context\n");
	    return false;
	}

	XAtom = XInternAtom(pXDisplay, "WM_DELETE_WINDOW", False); 	// Register Close Window Event with Xorg
	XSetWMProtocols(pXDisplay, XWindow, &XAtom, 1);
	glXMakeCurrent(pXDisplay, XWindow, XGLXContext);

	return true;
}

void CoreGLX::Destroy()
{
	p->Info("CoreGLX.Destroy()\n");
	if(XGLXContext)
	{
		glXMakeCurrent(pXDisplay, None, NULL);
		glXDestroyContext(pXDisplay, XGLXContext);
	}
	XGLXContext = NULL;
	XDestroyWindow(pXDisplay, XWindow);
	XCloseDisplay(pXDisplay);
}

bool CoreGLX::IsExtensionSupported(const char *extList, const char *extension)
{
	const char *start, *where, *terminator;
	/* Extension names should not have spaces. */
	where = strchr(extension, ' ');
	if(where || *extension == '\0')	return false;
	/* It takes a bit of care to be fool-proof about parsing the OpenGL extensions string. Don't be fooled by sub-strings, etc. */
	for(start = extList;;)
	{
		where = strstr(start, extension);
		if(!where)	break;
		terminator = where + strlen(extension);
		if(where == start || *(where - 1) == ' ')
		if(*terminator == ' ' || *terminator == '\0')	return true;
		start = terminator;
	}
	return false;
}

void CoreGLX::Events()
{
	while(XPending(pXDisplay) > 0)
	{
		XNextEvent(pXDisplay, &gXEvent);
		switch(gXEvent.type)
		{
			case ButtonRelease:		p->Info("CoreGLX.Events() ButtonRelease\n");
									p->pMouse->Events(&gXEvent);																						break;
			case ButtonPress:		p->Info("CoreGLX.Events() ButtonPress\n");
									p->pMouse->Events(&gXEvent);																						break;
			case MotionNotify:		p->pMouse->Events(&gXEvent);																						break;
			case KeyRelease:		p->Info("CoreGLX.Events() KeyRelease\n");
									p->pKeyboard->Events(&gXEvent.xkey);																				break;
			case KeyPress:			p->Info("CoreGLX.Events() KeyPress\n");
									p->pKeyboard->Events(&gXEvent.xkey);																				break;
			case ClientMessage:		p->Info("CoreGLX.Events() ClientMessage\n");
									if(!strcmp(XGetAtomName(pXDisplay, gXEvent.xclient.message_type), "WM_PROTOCOLS"))
										p->bActive = false;
																																						break;
			case ConfigureNotify:	p->Info("CoreGLX.Events() ConfigureNotify\n");
									if((gXEvent.xconfigure.width != p->pCoreGLX->Width) || (gXEvent.xconfigure.height != p->pCoreGLX->Height))
									{
										p->Info("gXEvent.xconfigure.width = %d, gXEvent.xconfigure.height = %d\n", gXEvent.xconfigure.width,
												gXEvent.xconfigure.height);
										p->pRend->ReShape(gXEvent.xconfigure.width, gXEvent.xconfigure.height);
										p->pCoreGLX->Width = p->pCoreGLX->WinWidth = gXEvent.xconfigure.width;
										p->pCoreGLX->Height = p->pCoreGLX->WinHeight = gXEvent.xconfigure.height;

									}																													break;
			default:				p->Info("CoreGLX.Events() gXEvent.type = %d\n", gXEvent.type);														break;
		}
	}
}

void CoreGLX::PrintModes()
{
	p->Info("CoreGLX.PrintModes()\n");
	p->Info("#   resolution  dotclock hsyncs hsynce vsyncs vsynce flags \n");
	for(int i = 0; i < nXModes; i++)
		p->Info("%02d   %04dx%04d   %06d   %04d   %04d   %04d   %04d   %02d\n", i,
			pXF86VidModeModeInfo[i]->hdisplay, 		pXF86VidModeModeInfo[i]->vdisplay,	pXF86VidModeModeInfo[i]->dotclock,
			pXF86VidModeModeInfo[i]->hsyncstart,	pXF86VidModeModeInfo[i]->hsyncend,	pXF86VidModeModeInfo[i]->vsyncstart,
			pXF86VidModeModeInfo[i]->vsyncend,		pXF86VidModeModeInfo[i]->flags);
}

void CoreGLX::SetVSync(bool bState)
{
	p->Info("CoreGLX.SetVSync(%d)\n", bState);
	if(bState)	glXSwapIntervalEXT(pXDisplay, XWindow, 1);
	else		glXSwapIntervalEXT(pXDisplay, XWindow, 0);

	bVSync = bState;
}

void CoreGLX::SetXMode(int NewXMode)
{
	p->Info("CoreGLX.SetXMode(%d)\n", NewXMode);
	if(NewXMode > 0 && NewXMode < nXModes)
	{	XMode = NewXMode;
		XF86VidModeSwitchToMode(pXDisplay, cXScreen, pXF86VidModeModeInfo[XMode]);
		XF86VidModeSetViewPort(pXDisplay, cXScreen, 0, 0);
		XFlush(pXDisplay);
		Width = pXF86VidModeModeInfo[XMode]->hdisplay;
		Height = pXF86VidModeModeInfo[XMode]->vdisplay;
	}
}

void CoreGLX::SetFullScreen(bool bState)
{	// Requires Reinitialization
	p->Info("CoreGLX.SetFullScreen(%d)\n", bState);
	if(bFullScreen != bState)
	{	bFullScreen = bState;
		//p->pRend->Destroy();
		Destroy();
		Init(bFullScreen);
		p->pRend->Init();
	}
}
