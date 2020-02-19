#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <string.h> 
#include <linux/joystick.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

// The Y-Axis increases downwards on Joystick, but upwards in GL so we invert the Y-axis

#define JLX  (float)p->pJoystick->pAxes[0]
#define JLY -(float)p->pJoystick->pAxes[1]
#define JRX  (float)p->pJoystick->pAxes[2]
#define JRY -(float)p->pJoystick->pAxes[3]

class Joystick
{
public:
	bool 	bInfo;
	int		File;
	char   	*pName;
	int		nAxes;
	float	*pAxes;	
	int		nButtons;
	char 	*pButton;
	struct 	js_event Event;
	
			Joystick();
	bool 	Init();
	void 	Destroy();
	void 	Events();
};

#endif /* JOYSTICK_H_ */
