#include "core.h"

Core *p;	// Global Root Node

Core::Core()
{
	ptRunTime = new Timer;
	ptRunTime->Start();
	bSaveInfo = true;
	fpInfo = fopen("infolog.txt", "wt");
	if(fpInfo == NULL)
	{	bSaveInfo = false;
		Info("<Error> Core::Core() Error opening infolog.txt\n");
	}
	else
		Info("infolog.txt Opened\n");
	Info("Core.Core()\n");
	bActive = true;
	Rate	= 60;
	ptRender = ptEvents = ptTotal = ptWait = ptSleep = NULL;
}

Core::~Core()
{
	Info("Core.~Core()\n");
	delete pKeyboard, delete pMouse, delete pJoystick, delete pRend, delete pCoreGLX, delete ptRunTime;
	fclose(fpInfo);
}
bool Core::Init()
{
	Info("Core.Init()\n");
	pCoreGLX 	= new CoreGLX;
	pRend 		= new Rend;
	pJoystick 	= new Joystick;
	pMouse 		= new Mouse;
	pKeyboard 	= new Keyboard;

	ptRender = new Timer; ptEvents = new Timer; ptTotal = new Timer; ptWait = new Timer; ptSleep = new Timer;

	if(!pCoreGLX->Init())		return false;
	if(!pJoystick->Init())		return false;
	if(!pRend->Init())			return false;

	return true;
}

void Core::Destroy()
{
	Info("Core.Destroy()\n");
	if(p->pCoreGLX->XMode)	p->pCoreGLX->SetXMode(0);
	pCoreGLX->Destroy();
	pRend->Destroy();
	pJoystick->Destroy();
	delete ptRender; delete ptEvents; delete ptTotal; delete ptWait; delete ptSleep;
}

void Core::Info(const char *format, ...)
{
	ptRunTime->Stop();
	printf("[%16.9f] ", ptRunTime->Delta);
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	va_start(args, format);
	if(bSaveInfo)
	{	fprintf(fpInfo, "[%16.9f] ", ptRunTime->Delta);
		vfprintf(fpInfo, format, args);
	}
	va_end(args);
}

void Core::MainLoop()
{
	Info("Core.MainLoop()\n");

	int Sleeps;
	while(bActive)
	{
		ptTotal->Start();
		if(pRend->bActive)					// Rendering
		{	ptRender->Start();
			pRend->Render();
			ptRender->Stop();
		}
		ptEvents->Start();					// Event Processing
		pCoreGLX->Events();
		pJoystick->Events();
		ptEvents->Stop();
		ptTotal->Stop();
		*ptWait = *ptTotal;
		Sleeps = 0;
		while(ptWait->Delta < (1.0 / Rate))	// Throttling
		{
			ptSleep->Start();
			usleep(1);
			ptSleep->Stop();
			Sleeps++;
			*ptWait += *ptSleep;
		}
	}
}
