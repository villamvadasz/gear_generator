#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

	#include <math.h>

	#define MAX(a,b) ((a) > (b) ? a : b)
	#define MIN(a,b) ((a) < (b) ? a : b)
	#define RAD2DEG(a) (((a)/M_PI)*180.0)
	#define DEG2RAD(a) (((a)/180.0)*M_PI)

	typedef struct _Point {
		float x;
		float y;
	} Point;

	//y=mx+c
	//Ax+By+C=0
	typedef struct _Line {
		float m;
		float c;
		//secondary
		float A;
		float B;
		float C;
		float rad;
	} Line;

	typedef struct _LineSegment {
		Point a;
		Point b;
	} LineSegment;

	//x^2+y^2=r*r
	//(x-x0)^2+(y-y0)^2=r^2
	typedef struct _Circle {
		Point o;
		float r;
	} Circle;

	typedef struct _MyArc {
		Circle c;
		float radStart;
		float radStop;
	} MyArc;

	extern Point createPoint(float x, float y);
	extern Point rotatePoint(Point p, Point o, float rad);
	extern Point movePoint(Point p, Point o);
	extern Point movePointOnLine(Point p, Line l, float distance);
	extern float distancePoint(Point p1, Point p2);
	extern float distancePointLine(Point p, Line l);
	extern Line createLine(Point A, Point B);
	extern Line rotateLineAroundPoint(Line l, Point p, float rad);
	extern LineSegment createLineSegment(Point A, Point B);
	extern Circle createCircle(Point o, float r);
	extern Circle createCirclePointAndTangentLine(Point o, Line l);
	extern Point movePointAroundCircle(Circle c, Point p, float distance);
	extern unsigned int getTangentCircleLine(Circle c, Line l, Point *p1);
	extern unsigned int getIntersectionCircleLine(Circle c, Line l, Point *p1, Point *p2);
	extern unsigned int getIntersectionCircleCircle(Circle c, Circle l, Point *p1, Point *p2);
	extern Line getCircleTangetLine(Circle c, Point p);
	extern MyArc createMyArc(Circle c, float radStart, float radStop);
	extern Point getEndpoint1MyArc(MyArc a);
	extern Point getEndpoint2MyArc(MyArc a);
	extern float getRadPointToPoint(Point a, Point b);
	extern float getRadCircleToPoint(Circle c, Point p);
	extern unsigned int crosspointOfLines(Line a, Line b, Point *p);

	extern MyArc LimitMyArcWithLine(MyArc arc, Line l, unsigned int normal);
	extern MyArc LimitMyArcWithCircle(MyArc arc, Circle c, unsigned int normal);
	extern unsigned int LimitLineSegmentWithLine(LineSegment ls, Line l, unsigned int normal, LineSegment *ls_truncated);
	extern unsigned int LimitLineSegmentWithCircle(LineSegment ls, Circle c, unsigned int normal, LineSegment *ls_truncated);

	extern Point mirrorPointLine(Point p, Line l);
	extern MyArc mirrorMyArcLine(MyArc arc, Line l);
	extern LineSegment mirrorLineSegmentLine(LineSegment l, Line m);


#endif
