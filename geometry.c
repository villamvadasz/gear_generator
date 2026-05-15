#include <string.h>
#include <stdio.h>
#include <math.h>
#include "geometry.h"

float getRadPointToPoint(Point a, Point b);
unsigned int crosspointOfLines(Line a, Line b, Point *p);

Point createPoint(float x, float y) {
	Point p;
	p.x = x;
	p.y = y;
	return p;
}

Point rotatePoint(Point p, Point o, float rad) {
	float angleInRads = rad;
	float s = sin(angleInRads);
	float c = cos(angleInRads);

	// translate point back to origin:
	p.x -= o.x;
	p.y -= o.y;

	// rotate point
	float xnew = p.x * c - p.y * s;
	float ynew = p.x * s + p.y * c;

	// translate point back:
	p.x = xnew + o.x;
	p.y = ynew + o.y;

	return p;
}

Point movePoint(Point p, Point o) {
	Point result = createPoint(p.x, p.y);
	result.x += o.x;
	result.y += o.y;
	return result;
}

Point movePointOnLine(Point p, Line l, float distance) {
	Point r;
	if (l.m == INFINITY) {
		r.x = p.x;
		r.y = p.y + distance;
	} else {
		r.x = p.x + (distance * cos(l.rad));
		r.y = p.y + (distance * sin(l.rad));
	}
	return r;
}

float distancePoint(Point p1, Point p2) {
	float distance;

	float x1 = p1.x;
	float y1 = p1.y;
	float x2 = p2.x;
	float y2 = p2.y;

	distance = sqrt( pow(y2-y1, 2.0) + pow(x2-x1, 2.0) );

	return distance;
}

float distancePointLine(Point p, Line l) {
	float D = (l.A * p.x) + (l.B * p.y) + l.C;
	return D;
}

Line createLine(Point A, Point B) {
	Line l;
	if ((B.x - A.x) == 0) {
		l.m = INFINITY;
		l.c = (A.x);
	} else {
		l.m = (B.y - A.y) / (B.x - A.x);
		l.c = (A.y) - (A.x * l.m);

	}
	{
		float x1 = A.x;
		float y1 = A.y;
		float x2 = B.x;
		float y2 = B.y;
		float A = y2 - y1;
		float B = -(x2-x1);
		float C = (-A * x1) - (B * y1);
		float M = sqrt(pow(A,2.0) + pow(B,2.0));
		A /= M;
		B /= M;
		C /= M;
		l.A = A;
		l.B = B;
		l.C = C;
	}
	l.rad = getRadPointToPoint(A, B);
	return l;
}

Line rotateLineAroundPoint(Line l, Point p, float rad) {
	Line lres;

	Point p1;
	Point p2;
	if (l.m == INFINITY) {
		//printf("INFINITY\r\n");
		p1.x = l.c;//y=mx+c
		p1.y = 0.0;
		p2.x = l.c;
		p2.y = 1.0;
	} else if (l.m == 0.0) {
		//printf("ZERO\r\n");
		p1.x = 0.0;
		p1.y = l.c;//y=mx+c
		p2.x = 100.0;
		p2.y = l.c;//y=mx+c
	} else {
		p1.x = -100.0;
		p1.y = (p1.x * l.m) + l.c;//y=mx+c

		p2.x = 100.0;
		p2.y = (p2.x * l.m) + l.c;//y=mx+c
	}

	Point p1_rotated = rotatePoint(p1, p, rad);
	Point p2_rotated = rotatePoint(p2, p, rad);

	lres = createLine(p1_rotated, p2_rotated);

	return lres;
}

LineSegment createLineSegment(Point A, Point B) {
	LineSegment l;
	l.a = A;
	l.b = B;
	return l;
}

Circle createCircle(Point o, float r) {
	Circle c;
	c.o = o;
	c.r = r;
	return c;
}

Circle createCirclePointAndTangentLine(Point o, Line l) {
	Circle c;
	c.o = o;

	if (l.m == INFINITY) {
		c.r = fabs(l.c - o.y);
	} else if (l.m == 0.0) {
		c.r = fabs(l.c - o.x);
	} else {
		Point pl1;
		pl1.x = 0;
		pl1.y = (l.m * pl1.x) + l.c;//y=mx+c
		Point pl2;
		pl2.y = 0;
		pl2.x = (pl2.y - l.c) / l.m;//(y-c)/m=x

		float x0 = o.x;
		float y0 = o.y;
		float x1 = pl1.x;
		float y1 = pl1.y;
		float x2 = pl2.x;
		float y2 = pl2.y;

		float fi = getRadPointToPoint(pl2, pl1);
		float cos_part = (cos(fi)*(pl1.y - y0));
		float sin_part = (sin(fi)*(pl1.x - x0));
		float diff_part = cos_part - sin_part;
		float abs_part = fabs(diff_part);
		c.r = abs_part;
	}

	return c;
}

Point createPointPointAndTangentLine(Point o, Line l) {
	Point p;

	if (l.m == INFINITY) {
		p.x = o.x;
		p.y = l.c;
	} else if (l.m == 0.0) {
		p.x = l.c;
		p.y = o.y;
	} else {
		float A = l.A;
		float B = l.B;
		float C = l.C;

		double distance = fabs(A * o.x + B * o.y + C) / sqrt(A * A + B * B);

		double D = A * o.x + B * o.y + C;
		double denominator = A * A + B * B;

		p.x = o.x - (A * D) / denominator;
		p.y = o.y - (B * D) / denominator;
	}

	return p;
}

unsigned int getTangentCircleLine(Circle c, Line l, Point *p1) {
	unsigned int result = 0;
	Point i;
	Point i1;
	Point i2;
	if (l.m == INFINITY) {
		if (
			((l.c) <= (c.o.x + c.r)) && 
			((c.o.x - c.r) <= (l.c))
		) {
			i1.x = l.c;
			i1.y = sqrt(pow(c.r, 2.0) - pow(l.c, 2.0));//y*y=r*r-x*x
			i2.x = l.c;
			i2.y = -sqrt(pow(c.r, 2.0) - pow(l.c, 2.0));

			i1.y += c.o.y;
			i2.y += c.o.y;
			//i1.x += c.o.x;
			//i2.x += c.o.x;
			if (i1.y != NAN) {
				result = 0;
				if (p1 != NULL) {
					*p1 = i1;
					result++;
				}
			}
			if (i2.y != NAN) {
			} else {
				result = 0;
				if (p1 != NULL) {
					*p1 = i1;
					result++;
				}
			}
		} else {
			result = 0;
		}		
	} else {
		float m = l.m;
		float b = l.c;

		float h = c.o.x;
		float k = c.o.y;
		float r = c.r;


		float A = 1.0 + m * m;
		float B = 2.0 * m * (b - k) - 2.0 * h;
		float C = h * h + (b - k) * (b - k) - r * r;

		// Calculate the discriminant
		float discriminant = B * B - 4.0 * A * C;
		//printf("A:                     %2.4f\r\n", A);
		//printf("B:                     %2.4f\r\n", B);
		//printf("C:                     %2.4f\r\n", C);
		//printf("discriminant:                     %2.4f\r\n", discriminant);

		// Check if there is an intersection
		if (discriminant > 0.0) {
			//not a tangent line
			result = 0;
		} else {
			Point i1;
			// One intersection point (tangent case)
			if (A != 0.0) {
				float x = -B / (2.0 * A);
				float y = m * x + b;

				i1.x = x;
				i1.y = y;

				result = 0;
				if (p1 != NULL) {
					*p1 = i1;
					result++;
				}
			}
		}
	}

	return result;
}

unsigned int getIntersectionCircleLine(Circle c, Line l, Point *p1, Point *p2) {
	unsigned int result = 0;
	Point i;
	Point i1;
	Point i2;
	if (l.m == INFINITY) {
		if (
			((l.c) <= (c.o.x + c.r)) && 
			((c.o.x - c.r) <= (l.c))
		) {
			i1.x = l.c;
			i1.y = sqrt(pow(c.r, 2.0) - pow(l.c, 2.0));//y*y=r*r-x*x
			i2.x = l.c;
			i2.y = -sqrt(pow(c.r, 2.0) - pow(l.c, 2.0));

			i1.y += c.o.y;
			i2.y += c.o.y;
			//i1.x += c.o.x;
			//i2.x += c.o.x;
			if (i1.y != NAN) {
				result = 0;
				if (p1 != NULL) {
					*p1 = i1;
					result++;
				}
			}
			if (i2.y != NAN) {
				if (p2 != NULL) {
					if ((i1.x != i2.x) || (i1.y != i2.y)) {
						*p2 = i2;
						result++;
					}
				}
			} else {
				result = 0;
				if (p1 != NULL) {
					*p1 = i1;
					result++;
				}
			}
		} else {
			result = 0;
		}		
	} else {
		float m = l.m;
		float b = l.c;

		float h = c.o.x;
		float k = c.o.y;
		float r = c.r;


		float A = 1.0 + m * m;
		float B = 2.0 * m * (b - k) - 2.0 * h;
		float C = h * h + (b - k) * (b - k) - r * r;

		// Calculate the discriminant
		float discriminant = B * B - 4.0 * A * C;
		//printf("A:                     %2.4f\r\n", A);
		//printf("B:                     %2.4f\r\n", B);
		//printf("C:                     %2.4f\r\n", C);
		//printf("discriminant:                     %2.4f\r\n", discriminant);

		// Check if there is an intersection
		if (discriminant > 0.0) {
			Point i1;
			Point i2;
			// Two intersection points
			float x1 = (-B + sqrt(discriminant)) / (2 * A);
			float x2 = (-B - sqrt(discriminant)) / (2 * A);
			float y1 = m * x1 + b;
			float y2 = m * x2 + b;

			i1.x = x1;
			i1.y = y1;
			i2.x = x2;
			i2.y = y2;

			result = 0;
			if (p1 != NULL) {
				*p1 = i1;
				result++;
			}
			if (p2 != NULL) {
				if ((i1.x != i2.x) || (i1.y != i2.y)) {
					*p2 = i2;
					result++;
				}
			}
		} else if (discriminant == 0) {
			Point i1;
			// One intersection point (tangent case)
			float x = -B / (2 * A);
			float y = m * x + b;

			i1.x = x;
			i1.y = y;

			result = 0;
			if (p1 != NULL) {
				*p1 = i1;
				result++;
			}
		} else {
			result = 0;
		}
	}

	return result;
}

unsigned int getIntersectionCircleCircle(Circle c, Circle l, Point *p1, Point *p2) {
	unsigned int result = 0;
	Point p;
    // Distance between the centers of the circles
	float x1 = c.o.x;
	float y1 = c.o.y;
	float r1 = c.r;
	float x2 = l.o.x;
	float y2 = l.o.y;
	float r2 = l.r;

    float d = sqrt(pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0));
    
    if (d > (r1 + r2) || d < fabs(r1 - r2) || d == 0.0) {
       // No intersection or the circles are identical
        //printf("No intersection points or circles are identical.\n");
        result = 0;
    } else {
		if ((c.o.x == l.o.x) && (c.o.y == l.o.y) && (c.r == l.r)) {
			result = 3;
		} else {
			float a = (pow(r1, 2.0) - pow(r2, 2.0) + pow(d, 2.0)) / (2.0 * d);
			float h = sqrt(pow(r1, 2.0) - pow(a, 2.0));

			float x0 = x1 + a * (x2 - x1) / d;
			float y0 = y1 + a * (y2 - y1) / d;

			float xi1 = x0 + h * (y2 - y1) / d;
			float yi1 = y0 - h * (x2 - x1) / d;
			
			float xi2 = x0 - h * (y2 - y1) / d;
			float yi2 = y0 + h * (x2 - x1) / d;

			Point i1 = createPoint(xi1, yi1);
			Point i2 = createPoint(xi2, yi2);
			result = 0;
			if (p1 != NULL) {
				*p1 = i1;
				result++;
			}
			if (p2 != NULL) {
				if ((i1.x != i2.x) || (i1.y != i2.y)) {
					*p2 = i2;
					result++;
				}
			}
		}
	}

	return result;
}

Line getCircleTangetLine(Circle c, Point p) {
	Line l;

	Point plr = rotatePoint(c.o, p, (90.0 / 180.0) * M_PI );

	l = createLine(p, plr);

	return l;
}

MyArc createMyArc(Circle c, float radStart, float radStop) {
	MyArc a;
	a.c = c;
	a.radStart = radStart;
	a.radStop = radStop;
	return a;
}

Point getEndpoint1MyArc(MyArc a) {
	float x = a.c.o.x + (float)(a.c.r * cos(a.radStart));
	float y = a.c.o.y + (float)(a.c.r * sin(a.radStart));
	Point p = createPoint(x, y);
	return p;
}

Point getEndpoint2MyArc(MyArc a) {
	float x = a.c.o.x + (float)(a.c.r * cos(a.radStop));
	float y = a.c.o.y + (float)(a.c.r * sin(a.radStop));
	Point p = createPoint(x, y);
	return p;
}

float getRadPointLine(Line l) {
	float result = 0;
	result = l.rad;
	return result;
}

float getRadPointToPoint(Point a, Point b) {
	float result = 0.0;
	float dx = b.x - a.x;
	float dy = b.y - a.y;

	result = atan2(dy, dx);

	return result;
}

float getRadCircleToPoint(Circle c, Point p) {
	float result = 0.0;

	result = getRadPointToPoint(c.o, p);

	return result;
}

MyArc LimitMyArcWithLine(MyArc arc, Line l, unsigned int normal) {
	MyArc result;
	unsigned int result_points = 0;
	Point Pt1;
	Point Pt2;
	
	result_points = getIntersectionCircleLine(arc.c, l, &Pt1, &Pt2);
	if (result_points == 2) {
		result = arc;
		float rad_a = getRadPointToPoint(arc.c.o, Pt1);
		float rad_b = getRadPointToPoint(arc.c.o, Pt2);
		if (rad_a < rad_b) {
			float temp = rad_a;
			rad_a = rad_b;
			rad_b = temp;
		}
		result.radStart = MIN(rad_a, result.radStart);
		result.radStop = MAX(rad_b, result.radStop);
		if (normal == 0) {
		} else {
			float swap = 0.0;
			swap = result.radStart;
			result.radStart = result.radStop;
			result.radStop = swap;
		}
	} else {
		result = arc;
	}

	return result;
}

MyArc LimitMyArcWithCircle(MyArc arc, Circle c, unsigned int normal) {
	MyArc result;
	unsigned int result_points = 0;
	Point Pt1;
	Point Pt2;
	
	result_points = getIntersectionCircleCircle(arc.c, c, &Pt1, &Pt2);
	if (result_points == 2) {
		result = arc;
		float rad_a = getRadPointToPoint(arc.c.o, Pt1);
		float rad_b = getRadPointToPoint(arc.c.o, Pt2);
		if (rad_a < rad_b) {
			float temp = rad_a;
			rad_a = rad_b;
			rad_b = temp;
		}

		if (normal == 0) {
			result.radStart = MIN(rad_a, result.radStart);
			result.radStop = MAX(rad_b, result.radStop);
		} else {
			result.radStart = MIN(rad_b, result.radStart);
			result.radStop = MAX(rad_a, result.radStop);
		}
	} else {
		result = arc;
	}

	return result;
}

unsigned int crosspointOfLineSegmentLine(LineSegment a, Line b, Point *p) {
	unsigned int result = 0;
	if (p != NULL) {
		result = crosspointOfLines(createLine(a.a, a.b), b, p);
		if (result == 1) {

			float left_x = MIN(a.a.x, a.b.x);
			float right_x = MAX(a.a.x, a.b.x);
			float up_y = MAX(a.a.y, a.b.y);
			float down_y = MIN(a.a.y, a.b.y);

			if ((left_x <= p->x) && (p->x <= right_x)) {
				if ((down_y <= p->y) && (p->y <= up_y)) {
					result++;
				}
			}
		}
	}
	return result;
}

unsigned int crosspointOfLines(Line a, Line b, Point *p) {
	unsigned int result = 0;
	//y=max+ca

	//y=mbx+cb
	
	//max+ca=mbx+cb
	//max-mbx=cb-ca
	//x(ma-mb)=cb-ca
	//x=(cb-ca)/(ma-mb)
	
	if (a.m != b.m) {
		if ((a.m == INFINITY) && (b.m == INFINITY)) {
			result = 0;
		} else if ((a.m == INFINITY) && (b.m != INFINITY)) {
			float xcal = a.c;
			float ycal = b.m * a.c + b.c;
			if (p != NULL) {
				p->x = xcal;
				p->y = ycal;
				result = 1;
			}
		} else if ((a.m != INFINITY) && (b.m == INFINITY)) {
			float xcal = b.c;
			float ycal = a.m * b.c + a.c;
			if (p != NULL) {
				p->x = xcal;
				p->y = ycal;
				result = 1;
			}
		} else {
			float xcal = (b.c - a.c) / (a.m - b.m);
			float ycal = a.m * xcal + a.c;
			if (p != NULL) {
				p->x = xcal;
				p->y = ycal;
				result = 1;
			}
		}
	}
	return result;
}

unsigned int LimitLineSegmentWithLine(LineSegment ls, Line l, unsigned int normal, LineSegment *ls_truncated) {
	unsigned int result = 0;
	Point p;
	if (ls_truncated != NULL) {
		unsigned int crossingPointCnt = crosspointOfLineSegmentLine(ls, l, &p);
		if (crossingPointCnt == 2) {
			result = 1;
			//printf("Crossing point on line\r\n");
			if (normal) {
				*ls_truncated = createLineSegment(ls.a, p);
			} else {
				*ls_truncated = createLineSegment(ls.b, p);
			}
		} else if (crossingPointCnt == 1) {
			float distance = distancePointLine(ls.b, l);
			if (distance >= 0.0) {
				if (normal) {
					result = 1;
					*ls_truncated = ls;
				}
			} else if (distance <= 0.0) {
				if (!normal) {
					result = 1;
					*ls_truncated = ls;
				}
			} else {
			}
		} else {
			result = 0;
		}
	}
	return result;
}

unsigned int LimitLineSegmentWithCircle(LineSegment ls, Circle c, unsigned int normal, LineSegment *ls_truncated) {
	unsigned int result = 0;
	unsigned int doLimit = 0;

	Point p1;
	Point p2;
	Point Pintersect;

	unsigned int cnt = getIntersectionCircleLine(c, createLine(ls.a, ls.b), &p1, &p2);
	if (cnt == 2) {
		if (normal) {
			Pintersect = p2;
		} else {
			Pintersect = p1;
		}
		doLimit = 1;
	} else if (cnt == 1) {
		Pintersect = p1;
		doLimit = 1;
	} else {
	}
	
	if (doLimit) {
		Line Ltangent = getCircleTangetLine(c, Pintersect);

		result = LimitLineSegmentWithLine(ls, Ltangent, normal, ls_truncated);
	}

	return result;
}

Point mirrorPointLine(Point p, Line l) {
	Point m;
	float D = distancePointLine(p, l);
	m.x = p.x - (2.0 * l.A) * D;
	m.y = p.y - (2.0 * l.B) * D;
	return m;
}

MyArc mirrorMyArcLine(MyArc arc, Line l) {
	MyArc result;
	result.c.o = mirrorPointLine(arc.c.o, l);
	result.c.r = arc.c.r;
	result.radStart = arc.radStart;
	result.radStop = arc.radStop;

	float radStartDiff = arc.radStart - l.rad;
	float radStopDiff = arc.radStop - l.rad;

	result.radStart -= 2.0 * radStartDiff;
	result.radStop -= 2.0 * radStopDiff;

	return result;
}

LineSegment mirrorLineSegmentLine(LineSegment l, Line m) {
	LineSegment result;
	result = l;
	result.a = mirrorPointLine(result.a, m);
	result.b = mirrorPointLine(result.b, m);
	return result;
}

Point movePointAroundCircle(Circle c, Point p, float distance) {
	Point result;

	//float radToPoint = getRadCircleToPoint(c, p);

	float rp2 = 2.0 * c.r * M_PI;
	float radAdded = (distance / rp2) * (2.0 * M_PI);
	//printf("rp2:    %2.4f\r\n", rp2);
	//printf("distance:    %2.4f\r\n", distance);
	//printf("radAdded:    %2.4f\r\n", radAdded);

	//radToPoint += radAdded;

	result = rotatePoint(p, c.o, radAdded);

	return result;
}
