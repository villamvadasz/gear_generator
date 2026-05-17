#include <string.h>
#include <stdio.h>
#include <math.h>
#include "geometry.h"
#include "gear.h"
#include "gear_sdl.h"
#include "k_stdtype.h"
#include "dxf.h"

unsigned int drawer_dxf = 0;
unsigned int drawer_sdl = 0;
FILE *dxf_file;

static void gear_draw_profile_shifter_calculator(float x, float y, float rotation, unsigned int visible_diameters, GearStructure *gear);

void gear_dxf(float x, float y, float rotation, unsigned int visible_diameters, GearStructure *gear, char *str) {
	drawer_dxf = 1;
	dxf_file = fopen(str, "w");
	dxf_begin(dxf_file);
	
	gear_draw_profile_shifter_calculator(x, y, rotation, visible_diameters, gear);
	
	dxf_end(dxf_file);
	drawer_dxf = 0;
}

void gear_sdl(float x, float y, float rotation, unsigned int visible_diameters, GearStructure *gear) {
	drawer_sdl = 1;
	gear_draw_profile_shifter_calculator(x, y, rotation, visible_diameters, gear);
	drawer_sdl = 0;
}

static void gear_draw_profile_shifter_calculator(float x, float y, float rotation, unsigned int visible_diameters, GearStructure *gear) {
	if (gear != NULL) {
		
		float angle = 0.0;
		unsigned int i = 0;
		unsigned int j = 0;
		unsigned int max_step = 360;
		float anglePerTooth = DEG2RAD(360.0 / gear->z);
		float rb = gear->db / 2.0;
		float rt = gear->da / 2.0;

		rotation += -anglePerTooth / 4.0;


		Point Pa = createPoint(x, 0.0);
		Circle Creference = createCircle(Pa, gear->d / 2.0);
		Circle Cbase = createCircle(Pa, gear->db / 2.0);
		Circle Cworkingpitch = createCircle(Pa, gear->dw / 2.0);
		Circle Ctip = createCircle(Pa, gear->da / 2.0);
		Circle Croot = createCircle(Pa, gear->df / 2.0);

		if (visible_diameters) {
			unsigned char tempStr[128];
			drawRed = 1;
			sprintf(tempStr, "Croot d=%2.4f", Croot.r * 2.0);
			renderText_dxf(tempStr, Croot.o.x, Croot.o.y + Croot.r);
			drawCircle_dxf(Croot);
			drawRed = 0;

			drawBlue = 1;
			sprintf(tempStr, "Cbase d=%2.4f", Cbase.r * 2.0);
			renderText_dxf(tempStr, Cbase.o.x, Cbase.o.y + Cbase.r);
			drawCircle_dxf(Cbase);
			drawBlue = 0;

			sprintf(tempStr, "Ctip d=%2.4f", Ctip.r * 2.0);
			renderText_dxf(tempStr, Ctip.o.x, Ctip.o.y + Ctip.r);
			drawCircle_dxf(Ctip);

			sprintf(tempStr, "Creference d=%2.4f", Creference.r * 2.0);
			renderText_dxf(tempStr, Creference.o.x, Creference.o.y + Creference.r);
			drawCircle_dxf(Creference);

			sprintf(tempStr, "Cworkingpitch d=%2.4f", Cworkingpitch.r * 2.0);
			renderText_dxf(tempStr, Cworkingpitch.o.x, Cworkingpitch.o.y + Cworkingpitch.r);
			drawCircle_dxf(Cworkingpitch);
		}

		
		gear_draw_profile_shifter_calculator_segment_start();
		
		for (i = 0; i < gear->z; i++) {
			for (j = 0; j < max_step; j++) {
				float angleInsideTooth = (anglePerTooth * ((float)j / (float)max_step));
				angle = i * anglePerTooth;

				if (angleInsideTooth < gear->rising_phi) {
					//rising + tip
					float tetha = solve_theta(angleInsideTooth);
					float resultx = 0.0;
					float resulty = 0.0;
					involute_curve(&resultx, &resulty, rb, tetha);

					Point p = rotatePoint(createPoint(resultx, resulty), createPoint(0.0, 0.0), angle + rotation);
					//drawPoint_dxf(p);
					p = movePoint(p, createPoint(x, y));
					gear_draw_profile_shifter_calculator_segment(p);
				} else if (angleInsideTooth < gear->tip_phi) {
					Point p = rotatePoint(createPoint(Ctip.r, 0.0), createPoint(0.0, 0.0), angle + angleInsideTooth + rotation);
					//drawPoint_dxf(p);
					p = movePoint(p, createPoint(x, y));
					gear_draw_profile_shifter_calculator_segment(p);
				} else if (angleInsideTooth < gear->falling_phi) {
					//drawRed = 1;
					float angleInsideToothLow = angleInsideTooth - gear->tip_phi;
					float tetha = solve_theta(gear->rising_dphi - angleInsideToothLow);
					float resultx = 0.0;
					float resulty = 0.0;
					involute_curve(&resultx, &resulty, rb, tetha);

					Point p = rotatePoint(createPoint(resultx, -resulty), createPoint(0.0, 0.0), (i * anglePerTooth) +  gear->falling_phi + rotation);
					//drawPoint_dxf(p);
					p = movePoint(p, createPoint(x, y));
					gear_draw_profile_shifter_calculator_segment(p);
				} else if (angleInsideTooth < gear->root_phi) {
					float angleInsideToothLow = angleInsideTooth - gear->tip_phi;
					Point p = rotatePoint(createPoint(Croot.r, 0.0), createPoint(0.0, 0.0), angle + angleInsideToothLow + gear->tip_phi + rotation);
					//drawPoint_dxf(p);
					p = movePoint(p, createPoint(x, y));
					gear_draw_profile_shifter_calculator_segment(p);
				}
			}
		}
		gear_draw_profile_shifter_calculator_segment_end();
	}
}
