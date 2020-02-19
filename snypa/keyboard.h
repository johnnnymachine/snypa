#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <X11/Xlib.h>

class Keyboard
{
public:
	bool 	bInfo;
	
			Keyboard();
	void 	Events(XKeyEvent *pXKeyEvent);
};

#endif /* KEYBOARD_H_ */
