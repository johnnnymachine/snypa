#include "mouse.h"
#include "core.h"

Mouse::Mouse()
{
	bInfo = false;
	X = Y = 0;
	pButton[0] = pButton[1] = pButton[2] = pButton[3] = pButton[4] = 0;
}

bool Mouse::Events(XEvent *pXEvent)
{
	switch(pXEvent->type)
	{	
		case ButtonRelease:
		pButton[pXEvent->xbutton.button - 1] = false;
		break;

		case ButtonPress:		pButton[pXEvent->xbutton.button - 1] = true;				break;
		case MotionNotify:		X = pXEvent->xmotion.x; 	Y = pXEvent->xmotion.y;			break;
	}
	
	if(bInfo)	p->Info("<Mouse> X:%6d Y:%6d B1: %2d B2: %2d B3: %2d B4: %2d B5: %2d \n", X, Y, pButton[0], pButton[1], pButton[2], pButton[3], pButton[4]);
					
	return true;
}
