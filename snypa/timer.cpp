#include <stdio.h>
#include "timer.h"

Timer::Timer()
{
	Delta = 0;
	// For clock_gettime version
	tsStart.tv_sec = 0;
	tsStart.tv_nsec = 0;
	tsStop.tv_sec = 0;
	tsStop.tv_nsec = 0;

	// For rdtsc version
	tscRate = 4.7;
	tscStart = 0;
	tscStop = 0;
	tscDelta = 0;
}
Timer::Timer(double iDelta)
{
	Delta = iDelta;
	tsStart.tv_sec = 0;
	tsStart.tv_nsec = 0;
	tsStop.tv_sec = 0;
	tsStop.tv_nsec = 0;

	tscRate = 4.7;
	tscStart = 0;
	tscStop = 0;
	tscDelta = 0;

}
void Timer::operator =(const Timer &rhs)
{
	this->Delta = rhs.Delta;
}
void Timer::operator +=(const Timer &rhs)
{
	this->Delta += rhs.Delta;
}
void Timer::Start()
{
	clock_gettime(CLOCK_MONOTONIC, &tsStart);
}
void Timer::Stop()
{
	clock_gettime(CLOCK_MONOTONIC, &tsStop);
	Delta = (((float)tsStop.tv_sec + ((float)tsStop.tv_nsec / 1e9)) - ((float)tsStart.tv_sec + ((float)tsStart.tv_nsec / 1e9)));
}
void Timer::Profile(void (*Function)(void))
{
	int i;
	Start();
	for(i = 0; i < 10000; i++)
		(*Function)();
	Stop();
	printf(	"iterations: %d	 seconds: %9.12f  seconds/function call: %9.12f\n",	i, Delta, Delta / (double) i);
}

/*void Timer::Start()
{
	asm (	"xor %%rax,%%rax;"
			"cpuid; "
			"rdtsc; "
			"shl $32, %%rdx;"
			"add %%rdx, %%rax;"
			:"=a" (tscStart)
			:
			:"rbx", "rcx", "rdx"
		);
}

void Timer::Stop()
{
	asm (	"xor %%rax,%%rax;"
			"cpuid;"
			"rdtsc;"
			"shl $32, %%rdx;"
			"add %%rdx, %%rax;"
			:"=a" (tscStop)
			:
			:"rbx", "rcx", "rdx"
		);

	tscDelta = tscStop - tscStart;
	Delta = (double)tscDelta / (double)tscRate / 1e9;
}
*/
