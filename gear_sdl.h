#ifndef _GEAR_SDL_H_
#define _GEAR_SDL_H_

	extern int drawRed;
	extern int drawBlue;

	extern void renderText_dxf(const char* text, float x, float y);
	extern void drawSegment_dxf(LineSegment s);
	extern void drawLine_dxf(Line l);
	extern void drawPoint_dxf(Point p);
	extern void drawCircle_dxf(Circle c);
	extern void drawMyArc_dxf(MyArc a);
	extern void drawMyArcRO_dxf(MyArc a, float rad, Point offset, Point center);
	extern void drawSegmentRO_dxf(LineSegment s, float rad, Point offset, Point center);

	extern void gear_draw_profile_shifter_calculator_segment_start(void);
	extern void gear_draw_profile_shifter_calculator_segment(Point p);
	extern void gear_draw_profile_shifter_calculator_segment_end(void);

#endif
