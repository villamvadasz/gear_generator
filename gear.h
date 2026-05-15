#ifndef _GEAR_H_
#define _GEAR_H_

	#include "gear_calculator.h"

	extern unsigned int drawer_dxf;
	extern unsigned int drawer_sdl;
	extern FILE *dxf_file;
	
	extern void gear_dxf(unsigned int z1, unsigned int z2, float module, float pressureAngleDeg, float x1, float x2, char *str);
	extern void gear_sdl(unsigned int z1, unsigned int z2, float module, float pressureAngleDeg, float x1, float x2, float rotation);

#endif
