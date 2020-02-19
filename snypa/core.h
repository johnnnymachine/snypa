#ifndef CORE_H_
#define CORE_H_

#include 	"coreglx.h"
#include 	"timer.h"
#include 	"rend.h"
#include 	"keyboard.h"
#include 	"mouse.h"
#include 	"joystick.h"

class Core
{
public:
	bool 		bActive, bSaveInfo;
	CoreGLX		*pCoreGLX;
	Timer		*ptRunTime;
	float		Rate;
	FILE		*fpInfo;
	Rend		*pRend;
	Keyboard	*pKeyboard;
	Mouse		*pMouse;
	Joystick	*pJoystick;
	Timer 		*ptRender, *ptEvents, *ptTotal, *ptWait, *ptSleep;

				Core();
	bool 		Init();
	void 		MainLoop();
	void 		Destroy();
				~Core();
	void 		Info(const char *format, ...);
};

extern Core *p;

#endif /* CORE_H_ */
