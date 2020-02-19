#include "joystick.h"
#include "core.h"

Joystick::Joystick()
{	
	p->Info("Joystick.Joystick()\n");
	bInfo 		= false;
	File 		= 0;
	pName 		= NULL;
	nAxes 		= 8;
	pAxes 		= NULL;	
	nButtons 	= 12;
	pButton 	= NULL;
}

bool Joystick::Init()
{	
	p->Info("Joystick.Init()\n");
	pName = new char[80];
	if((File = open("/dev/input/js0", O_RDONLY)) != -1)
	{
		ioctl(File, JSIOCGAXES, &nAxes);
		ioctl(File, JSIOCGBUTTONS, &nButtons);
		ioctl(File, JSIOCGNAME(80), pName);
		p->Info("Joystick Found: %s: %d Axes, %d Button\n", pName, nAxes, nButtons);
		fcntl(File, F_SETFL, O_NONBLOCK);
	}

	else
		p->Info("<Error> Joystick.Init()->open(\"/dev/input/js0\", O_RDONLY))\n" );

	pAxes = new float[nAxes];
	memset(pAxes, 0, sizeof(float[nAxes]));
	pButton = new char[nButtons];
	memset(pButton, 0, sizeof(char[nButtons]));

	return true;
}

void Joystick::Destroy()
{	
	p->Info("Joystick.Destroy()\n");
	close(File);
	delete [] pName; delete pAxes; delete pButton;
}

void Joystick::Events()
{	
	int nBytes = read(File, &Event, sizeof(struct js_event));
	while(nBytes > 0)
	{	switch (Event.type)
		{	case JS_EVENT_AXIS:		pAxes[Event.number] = (float)Event.value / 32767;	break;			
			case JS_EVENT_BUTTON:	pButton[Event.number] = Event.value;				break;
		}
		if(bInfo)
		{	p->Info("<Joystick> B:");
			for(int i = 0; i < nButtons; i++)		printf("%d", pButton[i]);

			// Using printf here to append to the initial p->Info()
			printf(" X1:%8.5f Y1:%8.5f ", pAxes[0], pAxes[1]);			
			if(nAxes > 3)	printf("X2:%8.5f Y2:%8.5f ", pAxes[2], pAxes[3]);
			if(nAxes > 5)	printf("X3:%8.5f Y3:%8.5f ", pAxes[4], pAxes[5]);
			printf("\n");
		}			
		nBytes = read(File, &Event, sizeof(struct js_event));
	}
}
