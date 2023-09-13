#ifndef CONFIG_H
#define CONFIG_H

#define LED		(B9)

#define MAIN_LOOP_FREQ 1000

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

#define INRANGE(num, max)   if(num > max) num = max; else if(num < -max) num = -max;

#endif
