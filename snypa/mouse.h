#ifndef MOUSE_H_
#define MOUSE_H_

#include <X11/Xlib.h>

class Mouse
{
public:
	bool 			bInfo;
	unsigned int 	X, Y;
	bool 			pButton[5];
	
					Mouse();
	bool 			Events(XEvent *pXEvent);
};

#endif /* MOUSE_H_ */
