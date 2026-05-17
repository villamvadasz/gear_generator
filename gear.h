#ifndef _GEAR_H_
#define _GEAR_H_

	#include "gear_calculator.h"

	extern unsigned int drawer_dxf;
	extern unsigned int drawer_sdl;
	extern FILE *dxf_file;
	
	extern void gear_dxf(float x, float y, float rotation, GearStructure *gear, char *str);
	extern void gear_sdl(float x, float y, float rotation, GearStructure *gear);

#endif
