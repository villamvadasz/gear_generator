#include <string.h>
#include <stdio.h>
#include <math.h>
#include "geometry.h"
#include "gear.h"
#include "gear_sdl.h"
#include "k_stdtype.h"
#include "dxf.h"

#define GEAR_INVOLUTE_ELEMENT_CNT (32uL)

float gear_addendum = 1.0;
float gear_dedendum = 1.25;
unsigned int drawer_dxf = 0;
unsigned int drawer_sdl = 0;
FILE *dxf_file;

static void gear(unsigned int teeth, float module, float pressureAngleDeg, float rotation);
static void gear_draw_profile_shifter_calculator(unsigned int z1,unsigned int z2 , float module, float pressureAngleDeg, float x1, float x2, float rotation);

float gear_getModule(float pitch) {
	float result = 0.0;
	result = pitch / M_PI;
	return result;
}

float gear_getPitch(float module) {
	float result = 0.0;
	result = module * M_PI;
	return result;
}

float gear_getAddendum(float module) {//ha
	float result = 0.0;
	result = module * gear_addendum;
	return result;
}

float gear_getDedendum(float module) {//hf
	float result = 0.0;
	result = module * gear_dedendum;
	return result;
}

float gear_getToothDepth(float module) {//h
	float result = 0.0;
	result = module * 2.25;
	return result;
}

float gear_getWorkingDepth(float module) {
	float result = 0.0;
	result = module * 2.0;
	return result;
}

float gear_getTipAndRootClearance(float module) {
	float result = 0.0;
	result = module * 0.25;
	return result;
}

float gear_getDedendumFilletRadius(float module) {
	float result = 0.0;
	result = module * 0.38;
	return result;
}

void gear_dxf(unsigned int z1, unsigned int z2, float module, float pressureAngleDeg, float x1, float x2, char *str) {
	drawer_dxf = 1;
	dxf_file = fopen(str, "w");
	dxf_begin(dxf_file);
	
	//gear_draw_standard(teeth, module, pressureAngleDeg, rotation);
	gear_draw_profile_shifter_calculator(z1, z2, module, pressureAngleDeg, x1, x2, 0.0);
	
	dxf_end(dxf_file);
	drawer_dxf = 0;
}

void gear_sdl(unsigned int z1, unsigned int z2, float module, float pressureAngleDeg, float x1, float x2, float rotation) {
	drawer_sdl = 1;
	//gear_draw_standard(teeth, module, pressureAngleDeg, rotation);
	gear_draw_profile_shifter_calculator(z1, z2, module, pressureAngleDeg, x1, x2, rotation);
	drawer_sdl = 0;
}

float solve_theta(float phi) {
    float t = phi + 0.5f; // initial guess
    for(int i=0; i<10; i++) {
        float f  = t - atanf(t) - phi;
        float df = 1.0f - 1.0f/(1.0f + t*t);
        t = t - f/df;
    }
    return t;
}

static void gear_draw_profile_shifter_calculator(unsigned int z1, unsigned int z2, float module, float pressureAngleDeg, float x1, float x2, float rotation) {
	GearStructure gearA;
	GearStructure gearB;

	float m = module;
	float alpha = DEG2RAD(pressureAngleDeg);

	gear_profile_shifter_calculator(m, alpha, z1, z2, x1, x2, &gearA, &gearB);

	Point Pa = createPoint(0.0, 0.0);

	Circle Creference = createCircle(Pa, gearA.d / 2.0);
	Circle Cbase = createCircle(Pa, gearA.db / 2.0);
	Circle Cworkingpitch = createCircle(Pa, gearA.dw / 2.0);
	Circle Ctip = createCircle(Pa, gearA.da / 2.0);
	Circle Croot = createCircle(Pa, gearA.df / 2.0);


#if 0
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
#endif

	float angle = 0.0;
	unsigned int x = 0;
	unsigned int t = 0;
	unsigned int max_step = 360;
	float anglePerTooth = DEG2RAD(360.0 / z1);
	float rb = Cbase.r;
	float rt = Ctip.r;
	
	gear_draw_profile_shifter_calculator_segment_start();
	
	for (t = 0; t < z1; t++) {
		for (x = 0; x < max_step; x++) {
			float angleInsideTooth = (anglePerTooth * ((float)x / (float)max_step));
			angle = t * anglePerTooth;

			if (angleInsideTooth < gearA.rising_phi) {
				//drawRed = 1;
				//rising + tip
				float tetha = solve_theta(angleInsideTooth);
				float resultx = rb * (cos(tetha) + (tetha * sin(tetha)) );
				float resulty = rb * (sin(tetha) - (tetha * cos(tetha)) );
				float distance = sqrt( pow(resulty, 2.0) + pow(resultx, 2.0) );

				Point pb = rotatePoint(createPoint(resultx, resulty), createPoint(0.0, 0.0), angle + rotation);
				//drawPoint_dxf(pb);
				gear_draw_profile_shifter_calculator_segment(pb);
				//drawRed = 0;
			} else if (angleInsideTooth < gearA.tip_phi) {
				//drawRed = 1;
				Point p = rotatePoint(createPoint(Ctip.r, 0.0), createPoint(0.0, 0.0), angle + angleInsideTooth + rotation);
				//drawPoint_dxf(p);
				gear_draw_profile_shifter_calculator_segment(p);
				//drawRed = 0;
			} else if (angleInsideTooth < gearA.falling_phi) {
				//drawRed = 1;
				float angleInsideToothLow = angleInsideTooth - gearA.tip_phi;
				float tetha = solve_theta(gearA.rising_dphi - angleInsideToothLow);
				float resultx = rb * (cos(tetha) + (tetha * sin(tetha)) );
				float resulty = rb * (sin(tetha) - (tetha * cos(tetha)) );

				//Point pb = rotatePoint(createPoint(resultx, -resulty), createPoint(0.0, 0.0), (t + 1.0) * anglePerTooth - (gearA.tip_phi - gearA.rising_dphi));
				Point pb = rotatePoint(createPoint(resultx, -resulty), createPoint(0.0, 0.0), (t * anglePerTooth) +  gearA.falling_phi + rotation);
				//drawPoint_dxf(pb);
				gear_draw_profile_shifter_calculator_segment(pb);
				//drawRed = 0;
			} else if (angleInsideTooth < gearA.root_phi) {
				float angleInsideToothLow = angleInsideTooth - gearA.tip_phi;
				Point p = rotatePoint(createPoint(Croot.r, 0.0), createPoint(0.0, 0.0), angle + angleInsideToothLow + gearA.tip_phi + rotation);
				//drawPoint_dxf(p);
				gear_draw_profile_shifter_calculator_segment(p);
			}
		}
	}
	gear_draw_profile_shifter_calculator_segment_end();
}

static void gear_draw_standard(unsigned int teeth, float module, float pressureAngleDeg, float rotation) {
	//https://www.fictiv.com/articles/creating-involute-gears-in-cad
	//https://khkgears.net/new/gear_knowledge/gear_technical_reference/involute_gear_profile.html
	unsigned int result_points = 0;
	float teethf = teeth;
	Point Pa;
	Point Pb;
	Point Pbase;
	Point PpressureLineBaseCircle;

	Circle Coutside;
	Circle Creference;//pitch
	Circle Cbase;
	Circle Croot;
	
	Circle debug_red_circle;
	
	Line Lab;
	Line Labrot;
	Line LabrotHalf;
	Line LPressureLine;

	MyArc AtipMyArc;
	MyArc ArootMyArc;
	Point Pinvolute[GEAR_INVOLUTE_ELEMENT_CNT];
	float length_involute[GEAR_INVOLUTE_ELEMENT_CNT] = {0};
	Line Linvolute[GEAR_INVOLUTE_ELEMENT_CNT] = {0};
	LineSegment LSinvolute[GEAR_INVOLUTE_ELEMENT_CNT] = {0};
	LineSegment LSinvolute_untruncated[GEAR_INVOLUTE_ELEMENT_CNT] = {0};
	LineSegment LSinvolute_truncated[GEAR_INVOLUTE_ELEMENT_CNT] = {0};
	unsigned int LSinvolute_truncated_cnt = 0;

	float pitch = gear_getPitch(module);

	//https://khkgears.net/new/gear_knowledge/gear_technical_reference/calculation_gear_dimensions.html
	float tip_diameter = 0.0;		//da 	d + 2m
	float reference_diameter = 0.0;	//d		zm
	float base_diameter = 0.0;		//db 	d*cos(alfa)
	float root_diameter = 0.0;		//df 	d-2.5m

	reference_diameter = teethf * module;
	tip_diameter = reference_diameter + (gear_getAddendum(module) * 2.0);
	base_diameter = reference_diameter * cos(DEG2RAD(pressureAngleDeg));
	root_diameter = reference_diameter - (gear_getDedendum(module) * 2.0);

	float tip_radius = tip_diameter / 2.0;
	float reference_radius = reference_diameter / 2.0;
	float base_radius = base_diameter / 2.0;
	float root_radius = root_diameter / 2.0;

	#ifdef _DEBUG_PRINT_LAYER1
		printf("module:                  %2.4f\r\n", module);
		printf("teeth:                   %u\r\n", teeth);
		printf("pressure angle:          %2.4f\r\n", pressureAngleDeg);
		printf("tooth depth h:           %2.4f\r\n", gear_getToothDepth(module));
		printf("addendum ha:             %2.4f\r\n", gear_getAddendum(module));
		printf("dedendum ha:             %2.4f\r\n", gear_getDedendum(module));
		printf("tooth pitch p:           %2.4f\r\n", gear_getPitch(module));
		printf("tooth thickness s0:      %2.4f\r\n", gear_getPitch(module) / 2.0);


		printf("\r\n");
		printf("tip_diameter:            %2.4f\r\n", tip_diameter);
		printf("reference_diameter:      %2.4f\r\n", reference_diameter);
		printf("base_diameter:           %2.4f\r\n", base_diameter);
		printf("root_diameter:           %2.4f\r\n", root_diameter);

		printf("\r\n");
		printf("tip_radius:              %2.4f\r\n", tip_radius);
		printf("reference_radius:        %2.4f\r\n", reference_radius);
		printf("base_radius:             %2.4f\r\n", base_radius);
		printf("root_radius:             %2.4f\r\n", root_radius);

	#endif

	Pa = createPoint(0.0, 0.0);
	Pb = createPoint(0.0, tip_diameter * 1.5);
	Creference = createCircle(Pa, reference_radius);
	Croot = createCircle(Pa, root_radius);
	Cbase = createCircle(Pa, base_radius);
	Coutside = createCircle(Pa, tip_radius);
	Lab = createLine(Pa, Pb);
	float cta = DEG2RAD((360.0 / teethf) * 0.5);
	Labrot = createLine(Pa, rotatePoint(Pb, Pa, cta));
	LabrotHalf = createLine(Pa, rotatePoint(Pb, Pa, cta / 2.0));

	{
		Point Ppitch1;
		Point Ppitch2;
		result_points = getIntersectionCircleLine(Cbase, Lab, &Ppitch1, &Ppitch2);
		if (result_points != 2) {
			return;
		}
		Pbase = Ppitch1;
	}
	LPressureLine = rotateLineAroundPoint(getCircleTangetLine(Cbase, Pbase), Pbase, DEG2RAD(pressureAngleDeg));

	{
		unsigned int x = 0;
		float chord_move = module;

		Pinvolute[0] = Pbase;
		length_involute[0] = 0.0;

		for (x = 1; x < GEAR_INVOLUTE_ELEMENT_CNT; x++) {
			Pinvolute[x] = movePointAroundCircle(Cbase, Pbase, chord_move * x);
			length_involute[x] = (chord_move * x);
		}

		for (x = 0; x < GEAR_INVOLUTE_ELEMENT_CNT; x++) {
			Linvolute[x] = getCircleTangetLine(Cbase, Pinvolute[x]);
			Point EndPoint = movePointOnLine(Pinvolute[x], Linvolute[x], length_involute[x]);
			LSinvolute[x] = createLineSegment(Pinvolute[x], EndPoint);
		}
		for (x = 1; x < GEAR_INVOLUTE_ELEMENT_CNT; x++) {
			LSinvolute[x] = createLineSegment(LSinvolute[x - 1].b, LSinvolute[x].b);
		}
	}

	{
		unsigned int x = 0;
		unsigned int element = GEAR_INVOLUTE_ELEMENT_CNT;
		for (x = 0; x < element; x++) {
			LSinvolute_untruncated[x] = LSinvolute[x];
		}
		for (x = 0; x < element; x++) {
			LineSegment ls_truncated;
			result_points = LimitLineSegmentWithLine(LSinvolute[x], Lab, 0, &ls_truncated);
			if (result_points == 1) {
				LSinvolute_truncated[LSinvolute_truncated_cnt] = ls_truncated;
				LSinvolute_truncated_cnt++;
			}
		}
	}
	{
		unsigned int x = 0;
		unsigned int element = LSinvolute_truncated_cnt;
		LSinvolute_truncated_cnt = 0;
		for (x = 0; x < element; x++) {
			LSinvolute_untruncated[x] = LSinvolute_truncated[x];
		}
		for (x = 0; x < element; x++) {
			LineSegment ls_truncated;
			result_points = LimitLineSegmentWithLine(LSinvolute_untruncated[x], LabrotHalf, 1, &ls_truncated);
			if (result_points == 1) {
				LSinvolute_truncated[LSinvolute_truncated_cnt] = ls_truncated;
				LSinvolute_truncated_cnt++;
			}
		}
	}

	{
		unsigned int x = 0;
		unsigned int element = LSinvolute_truncated_cnt;
		LSinvolute_truncated_cnt = 0;
		for (x = 0; x < element; x++) {
			LSinvolute_untruncated[x] = LSinvolute_truncated[x];
		}
		for (x = 0; x < element; x++) {
			LineSegment ls_truncated;
			result_points = LimitLineSegmentWithCircle(LSinvolute_untruncated[x], Coutside, 1, &ls_truncated);
			if (result_points == 1) {
				LSinvolute_truncated[LSinvolute_truncated_cnt] = ls_truncated;
				LSinvolute_truncated_cnt++;
			}
		}
	}
	{
		Point PtipRight = LSinvolute_truncated[LSinvolute_truncated_cnt - 1].b;
		Point PtipLeft = mirrorLineSegmentLine(LSinvolute_truncated[LSinvolute_truncated_cnt - 1], LabrotHalf).b;
		
		debug_red_circle = createCircle(PtipRight, 0.5);
		drawCircle_dxf(debug_red_circle);
		//printf("PtipRight.x:                  %2.4f\r\n", PtipRight.x);
		//printf("PtipRight.y:                  %2.4f\r\n", PtipRight.y);

		float radStart = getRadCircleToPoint(Coutside, PtipRight);
		float radStop = getRadCircleToPoint(Coutside, PtipLeft);
		AtipMyArc = createMyArc(Coutside, radStart, radStop);
	}
	LineSegment LSh1;
	LineSegment LSh2;
	{
		float rad = cta;
		Point PtipRight = rotatePoint(Pbase, Croot.o, rad);
		Point PtipLeft = rotatePoint(Pbase, Croot.o, 2.0 * rad);
		float radStart = getRadCircleToPoint(Croot, PtipRight);
		float radStop = getRadCircleToPoint(Croot, PtipLeft);
		ArootMyArc = createMyArc(Croot, radStart, radStop);
		
		LSh1 = createLineSegment(PtipRight, getEndpoint1MyArc(ArootMyArc));
		LSh2 = createLineSegment(PtipLeft, getEndpoint2MyArc(ArootMyArc));
	}

#if 1
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

	sprintf(tempStr, "Coutside d=%2.4f", Coutside.r * 2.0);
	renderText_dxf(tempStr, Coutside.o.x, Coutside.o.y + Coutside.r);
	drawCircle_dxf(Coutside);

	sprintf(tempStr, "Creference d=%2.4f", Creference.r * 2.0);
	renderText_dxf(tempStr, Creference.o.x, Creference.o.y + Creference.r);
	drawCircle_dxf(Creference);
#endif

#if 1
	drawLine_dxf(Lab);
	drawLine_dxf(Labrot);
	drawLine_dxf(LabrotHalf);
#endif

	{//Draw Single teeth
		float rad = cta * 2.0;
		Point offset = createPoint(0.0, 0.0);
		Point center = createPoint(Pa.x, Pa.y);

		for (int i = 0; i < teeth; i++) {
			float rad = i;
			rad *= (cta * 2.0);
			rad +=  + rotation;

			unsigned int x = 0;
			for (x = 0; x < LSinvolute_truncated_cnt; x++) {
				drawSegmentRO_dxf(LSinvolute_truncated[x], rad, offset, center);
				drawSegmentRO_dxf(mirrorLineSegmentLine(LSinvolute_truncated[x], LabrotHalf), rad, offset, center);
			}
			drawMyArcRO_dxf(AtipMyArc, rad, offset, center);
			drawMyArcRO_dxf(ArootMyArc, rad, offset, center);

			drawSegmentRO_dxf(LSh1, rad, offset, center);
			drawSegmentRO_dxf(LSh2, rad, offset, center);
		}
	}
}
