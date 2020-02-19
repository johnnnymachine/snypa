#ifndef TIMER_H_
#define TIMER_H_

#include <time.h>

struct Timer
{
	unsigned long int 	tscStart, tscStop, tscDelta;
	float  				tscRate; 							// cycles/ns (or cpu clock speed in ghz)

	double 		Delta;
	timespec 	tsStart, tsStop;

			Timer();
			Timer(double iDelta);
	void 	Start();
	void 	Stop();
	void 	Profile(void (*Function)(void));
	void	operator =(const Timer &rhs);
	void 	operator +=(const Timer &rhs);
};


#endif /* TIMER_H_ */
