#include <string.h>
#include <stdio.h>
#include <math.h>
#include "geometry.h"
#include "gear.h"
#include "gear_sdl.h"
#include "k_stdtype.h"

void gear_simple(unsigned int teeth, float pitch, float pressureAngleDeg, float rotation) {
//http://www.gearseds.com/files/Approx_method_draw_involute_teeth_rev2.pdf
	unsigned int result_points = 0;
	Point A;
	Point B;
	Point Ppitch;
	Point Pbase;
	Point T;
	Circle Croot;
	Circle Cpitch;
	Circle Coutside;
	Circle Cbase;
	Circle CpitchPoint;
	Circle Cteeth;
	Line Lab;
	Line Labrot;
	Line LabrotHalf;
	Line LPressureLine;
	
	MyArc Ateeth_side;
	MyArc Ateeth_outside;
	LineSegment Lteeth_side;
	MyArc Atootconnect;
	MyArc Ateeth_side_mirror;
	MyArc Ateeth_outside_mirror;
	LineSegment Lteeth_side_mirror;

	float bigN = teeth;
	float bigP = pitch;
	float bigD = bigN / bigP;

	float bigRD = (bigN - 2.0) / bigP;

	float bigOD = (bigN + 2.0) / bigP;

	#ifdef _DEBUG_PRINT_LAYER1
		printf("Root Circle diameter:    %2.4f\r\n", bigRD);
		printf("Pitch Circle diameter:   %2.4f\r\n", bigD);
		printf("Outside Circle diameter: %2.4f\r\n", bigOD);
	#endif

	A = createPoint(0.0, 0.0);
	B = createPoint(0.0, bigOD * 1.5);
	Croot = createCircle(A, bigRD);//Root Circle
	Cpitch = createCircle(A, bigD); //Pitch Circle
	Coutside = createCircle(A, bigOD);//Outside Circle
	Lab = createLine(A, B);
	
	float cta = DEG2RAD((360.0 / teeth) * 0.5);
	#ifdef _DEBUG_PRINT_LAYER1
		printf("cta:                     %2.4f\r\n", cta);
		printf("cta:                     %2.4f\r\n", RAD2DEG(cta));
	#endif

	{
		Point Brotated = rotatePoint(B, A, cta);
		Point Brotated2 = rotatePoint(B, A, cta / 2.0);
		#ifdef _SDL_DRAWER_LAYER2
			drawPoint(Brotated);
			drawPoint(Brotated2);
		#endif

		Labrot = createLine(A, Brotated);
		LabrotHalf = createLine(A, Brotated2);
	}

	{
		Point Ppitch1;
		Point Ppitch2;
		result_points = getIntersectionCircleLine(Cpitch, Lab, &Ppitch1, &Ppitch2);
		if (result_points != 2) {
			return;
		}
		Ppitch = Ppitch1;
	}
	#ifdef _DEBUG_PRINT_LAYER1
		printf("result_points:           %u\r\n", result_points);
		printf("Cpitch X:                %2.4f\r\n", Cpitch.o.x);
		printf("Cpitch Y:                %2.4f\r\n", Cpitch.o.y);
		printf("Cpitch r:                %2.4f\r\n", Cpitch.r);

		printf("Lab m:                   %2.4f\r\n", Lab.m);
		printf("Lab c:                   %2.4f\r\n", Lab.c);

		printf("Ppitch1 X:               %2.4f\r\n", Ppitch1.x);
		printf("Ppitch1 Y:               %2.4f\r\n", Ppitch1.y);
		printf("Ppitch2 X:               %2.4f\r\n", Ppitch2.x);
		printf("Ppitch2 Y:               %2.4f\r\n", Ppitch2.y);
	#endif

	{
		Line Lpressurehelper = getCircleTangetLine(Cpitch, Ppitch);
		#ifdef _DEBUG_PRINT_LAYER1
			printf("Lpressurehelper m:       %2.4f\r\n", Lpressurehelper.m);
			printf("Lpressurehelper c:       %2.4f\r\n", Lpressurehelper.c);
		#endif
		float pressureangle = DEG2RAD(pressureAngleDeg);

		LPressureLine = rotateLineAroundPoint(Lpressurehelper, Ppitch, pressureangle);
		#ifdef _DEBUG_PRINT_LAYER1
			printf("LPressureLine m:         %2.4f\r\n", LPressureLine.m);
			printf("LPressureLine c:         %2.4f\r\n", LPressureLine.c);
		#endif
	}

	Cbase = createCirclePointAndTangentLine(A, LPressureLine);
	#ifdef _DEBUG_PRINT_LAYER1
		printf("Cbase X:                %2.4f\r\n", Cbase.o.x);
		printf("Cbase Y:                %2.4f\r\n", Cbase.o.y);
		printf("Cbase r:                %2.4f\r\n", Cbase.r);
	#endif

	CpitchPoint = createCircle(Ppitch, (bigD / 8.0) * 2.0);
	#ifdef _DEBUG_PRINT_LAYER1
		printf("CpitchPoint X:          %2.4f\r\n", CpitchPoint.o.x);
		printf("CpitchPoint Y:          %2.4f\r\n", CpitchPoint.o.y);
		printf("CpitchPoint r:          %2.4f\r\n", CpitchPoint.r);
	#endif

	{
		Point Pt1;
		Point Pt2;
		getIntersectionCircleLine(Croot, Lab, &Pt1, &Pt2);
		Pbase = Pt1;
	}

	{
		Point Intersect;
		Point Intersect1;
		Point Intersect2;
		result_points = getIntersectionCircleCircle(CpitchPoint, Cbase, &Intersect1, &Intersect2);
		if (result_points != 2) {
			return;
		}
		Intersect = Intersect1;
		#ifdef _DEBUG_PRINT_LAYER1
			printf("result_points:          %u\r\n", result_points);
			printf("Intersect1 X:           %2.4f\r\n", Intersect1.x);
			printf("Intersect1 Y:           %2.4f\r\n", Intersect1.y);
			printf("Intersect2 X:           %2.4f\r\n", Intersect2.x);
			printf("Intersect2 Y:           %2.4f\r\n", Intersect2.y);
			printf("Intersect X:            %2.4f\r\n", Intersect.x);
			printf("Intersect Y:            %2.4f\r\n", Intersect.y);
		#endif
		Cteeth = createCircle(Intersect, (bigD / 8.0) * 2.0);
		#ifdef _DEBUG_PRINT_LAYER1
			printf("Cteeth X:               %2.4f\r\n", Cteeth.o.x);
			printf("Cteeth Y:               %2.4f\r\n", Cteeth.o.y);
			printf("Cteeth r:               %2.4f\r\n", Cteeth.r);
		#endif
	}

	{
		MyArc Ateeth = createMyArc(Cteeth, M_PI, -M_PI);
		#ifdef _DEBUG_PRINT_LAYER1
			printf("Ateeth Start:           %2.4f\r\n", (Ateeth.radStart / M_PI) * 180.0);
			printf("Ateeth Stop:            %2.4f\r\n", (Ateeth.radStop / M_PI) * 180.0);
		#endif

		MyArc Ateeth_L1 = LimitMyArcWithLine(Ateeth, Lab, 1);
		#ifdef _DEBUG_PRINT_LAYER1
			printf("Ateeth_L1 Start:        %2.4f\r\n", (Ateeth_L1.radStart / M_PI) * 180.0);
			printf("Ateeth_L1 Stop:         %2.4f\r\n", (Ateeth_L1.radStop / M_PI) * 180.0);
		#endif

		MyArc Ateeth_L2 = LimitMyArcWithLine(Ateeth, Labrot, 0);
		#ifdef _DEBUG_PRINT_LAYER1
			printf("Ateeth_L2 Start:        %2.4f\r\n", (Ateeth_L2.radStart / M_PI) * 180.0);
			printf("Ateeth_L2 Stop:         %2.4f\r\n", (Ateeth_L2.radStop / M_PI) * 180.0);
		#endif

		MyArc Ateeth_L3 = LimitMyArcWithCircle(Ateeth, Cpitch, 0);
		#ifdef _DEBUG_PRINT_LAYER1
			printf("Ateeth_L3 Cpitch Start: %2.4f\r\n", (Ateeth_L3.radStart / M_PI) * 180.0);
			printf("Ateeth_L3 Cpitch Stop:  %2.4f\r\n", (Ateeth_L3.radStop / M_PI) * 180.0);
		#endif

		MyArc Ateeth_L4 = LimitMyArcWithCircle(Ateeth, Coutside, 0);
		#ifdef _DEBUG_PRINT_LAYER1
			printf("Ateeth_L4 Coutside Start:%2.4f\r\n", (Ateeth_L4.radStart / M_PI) * 180.0);
			printf("Ateeth_L4 Coutside Stop:%2.4f\r\n", (Ateeth_L4.radStop / M_PI) * 180.0);
		#endif

		//Ez fontos resze
		Ateeth_side = createMyArc(Cteeth, Ateeth_L3.radStart, Ateeth_L4.radStop);
		#ifdef _DEBUG_PRINT_LAYER2
			printf("MyArc :                   %2.4f\r\n", (Ateeth_L3.radStart / M_PI) * 180.0);
			printf("MyArc :                   %2.4f\r\n", (Ateeth_L4.radStop / M_PI) * 180.0);
		#endif
	}
	{
		Point Pt1;
		Point Pt2;
		getIntersectionCircleCircle(Cteeth, Coutside, &Pt1, &Pt2);
		T = Pt2;
		float fromT = getRadPointToPoint(A, T);
		float radT = fromT - (M_PI/2.0);
		//float toT = (cta - radT) + (M_PI / 2.0);
		float toT = fromT + (cta / 2.0) - radT;
		#ifdef _DEBUG_PRINT_LAYER2
			printf("fromT :                 %2.4f\r\n", RAD2DEG(fromT));
			printf("toT :                   %2.4f\r\n", RAD2DEG(toT));
			printf("radT :                  %2.4f\r\n", RAD2DEG(radT));
		#endif
		//Ez is fontos
		Ateeth_outside = createMyArc(Coutside, fromT, toT);

		//Point Pbase_mirror = mirrorPointLine(Pbase, LabrotHalf);
		Atootconnect = createMyArc(Croot, toT + (0.5 * cta) , toT + (1.5 * cta));

	}

	//Ez is fontos
	Lteeth_side = createLineSegment(Pbase, Ppitch);

	//Mirror part
	Ateeth_outside_mirror = mirrorMyArcLine(Ateeth_outside, LabrotHalf);
	Ateeth_side_mirror = mirrorMyArcLine(Ateeth_side, LabrotHalf);
	Lteeth_side_mirror = mirrorLineSegmentLine(Lteeth_side, LabrotHalf);

	{//Draw Single teeth
		float rad = cta * 2.0;
		Point offset = createPoint(0.0, 0.0);
		Point center = createPoint(A.x, A.y);

		for (int i = 0; i < teeth; i++) {
			float rad = i * (cta * 2.0) + rotation;
			drawMyArcRO_dxf(Ateeth_outside, rad, offset, center);
			drawMyArcRO_dxf(Ateeth_side, rad, offset, center);
			drawSegmentRO_dxf(Lteeth_side, rad, offset, center);
			drawMyArcRO_dxf(Atootconnect, rad, offset, center);
			drawMyArcRO_dxf(Ateeth_outside_mirror, rad, offset, center);
			drawMyArcRO_dxf(Ateeth_side_mirror, rad, offset, center);
			drawSegmentRO_dxf(Lteeth_side_mirror, rad, offset, center);
		}
	}
	#ifdef _SDL_DRAWER_LAYER2
		drawPoint_dxf(A);
		drawCircle_dxf(Croot);
		drawCircle_dxf(Cpitch);
		drawCircle_dxf(Coutside);
		drawPoint_dxf(B);
		drawLine_dxf(Lab);
		drawLine_dxf(Labrot);
		drawLine_dxf(LabrotHalf);
		drawPoint_dxf(Ppitch1);
		drawPoint_dxf(Ppitch2);
		drawLine_dxf(LPressureLine);
		drawCircle_dxf(Cbase);
		drawCircle_dxf(CpitchPoint);
		drawCircle_dxf(Cteeth);
		drawPoint_dxf(T);
	#endif

	#ifdef _SDL_DRAWER_LAYER1
		drawMyArc_dxf(Ateeth_side);
		drawMyArc_dxf(Ateeth_outside);
		drawSegment_dxf(Lteeth_side);
		drawSegment_dxf(Atootconnect);

		drawMyArc_dxf(Ateeth_outside_mirror);
		drawMyArc_dxf(Ateeth_side_mirror);
		drawSegment_dxf(Lteeth_side_mirror);
	#endif

}
