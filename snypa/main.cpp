#include "core.h"

int main()
{
	p = new Core;
	if(p->Init())
	{
		p->MainLoop();
		p->Destroy();
		delete p;
		return 0;
	}
	else
	{
		p->Info("<Error> Core.Init()\n");
		delete p;
		return 1;
	}
}
