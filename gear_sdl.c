#include <string.h>
#include <stdio.h>
#include <math.h>
#include "geometry.h"
#include "gear.h"
#include "k_stdtype.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "dxf.h"

float OFFSET_X = 0.0;
float OFFSET_Y = 400.0;
float ZOOM_FACTOR = 32.0;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int drawRed = 0;
int drawBlue = 0;

int psegment_first = 0;
Point psegment_start;
Point psegment_prev;


extern SDL_Renderer * renderer;
extern TTF_Font* font;
void drawPoint_dxf(Point p);

void setColor(void) {
	if (drawer_sdl) {
		if (drawRed) {
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		} else {
			if (drawBlue) {
				SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
			} else {
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			}
		}
	}
	if (drawer_dxf) {
		if (drawRed) {
			dxf_set_layer(dxf_file, "red");
		} else {
			if (drawBlue) {
				dxf_set_layer(dxf_file, "blue");
			} else {
				dxf_set_layer(dxf_file, "black");
			}
		}

	}
}

void renderText_dxf(const char* text, float x, float y) {
	if (drawer_sdl) {
		x *= ZOOM_FACTOR;
		y *= ZOOM_FACTOR;
		x += WINDOW_WIDTH / 2.0;
		y = (WINDOW_HEIGHT / 2.0)-y;

		x += OFFSET_X;
		y += OFFSET_Y;

		SDL_Color color = {255, 255, 255, 255};
		if (drawRed) {
			SDL_Color c = {255, 0, 0, 255};
			color = c;
		} else {
			if (drawBlue) {
				SDL_Color c = {0, 0, 255, 255};
				color = c;
			} else {
				SDL_Color c = {255, 255, 255, 255};
				color = c;
			}
		}

		SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		
		SDL_Rect dstRect = {x, y, surface->w, surface->h}; // Auto-size to text
		SDL_FreeSurface(surface); // We no longer need the surface

		SDL_RenderCopy(renderer, texture, NULL, &dstRect);
		SDL_DestroyTexture(texture); // Free texture after rendering
	}
	if (drawer_dxf) {
	}
}

void drawCircle_dxf_helper(SDL_Renderer * renderer, float centreXf, float centreYf, float radiusf) {
	if (drawer_dxf) {
		dxf_circle(dxf_file, centreXf, centreYf, radiusf);
	}
	if (drawer_sdl) {
		centreXf *= ZOOM_FACTOR;
		centreYf *= ZOOM_FACTOR;
		radiusf *= ZOOM_FACTOR;
		centreXf += WINDOW_WIDTH / 2.0;
		centreYf = (WINDOW_HEIGHT / 2.0)-centreYf;

		int32_t centreX = centreXf;
		int32_t centreY = centreYf;
		int32_t radius = radiusf;

		const int32_t diameter = (radius * 2.0);

		int32_t x = (radius - 1);
		int32_t y = 0;
		int32_t tx = 1;
		int32_t ty = 1;
		int32_t error = (tx - diameter);

		while (x >= y)
		{
			//  Each of the following renders an octant of the circle
			SDL_RenderDrawPoint(renderer, centreX + x + OFFSET_X, centreY - y + OFFSET_Y);
			SDL_RenderDrawPoint(renderer, centreX + x + OFFSET_X, centreY + y + OFFSET_Y);
			SDL_RenderDrawPoint(renderer, centreX - x + OFFSET_X, centreY - y + OFFSET_Y);
			SDL_RenderDrawPoint(renderer, centreX - x + OFFSET_X, centreY + y + OFFSET_Y);
			SDL_RenderDrawPoint(renderer, centreX + y + OFFSET_X, centreY - x + OFFSET_Y);
			SDL_RenderDrawPoint(renderer, centreX + y + OFFSET_X, centreY + x + OFFSET_Y);
			SDL_RenderDrawPoint(renderer, centreX - y + OFFSET_X, centreY - x + OFFSET_Y);
			SDL_RenderDrawPoint(renderer, centreX - y + OFFSET_X, centreY + x + OFFSET_Y);
			
			if (error <= 0)
			{
				++y;
				error += ty;
				ty += 2;
			}

			if (error > 0)
			{
				--x;
				tx += 2;
				error += (tx - diameter);
			}
		}
	}
}

void DrawMyArc_helper(SDL_Renderer * renderer, float centreXf, float centreYf, float radiusf, float startAngle, float endAngle) {
	float angle = 0.0;
	if (startAngle < endAngle) {
		for (angle = startAngle; angle <= endAngle; angle += 0.001) { // Increment angle to draw a smooth arc
			float x = centreXf + (float)(radiusf * cos(angle));
			float y = centreYf + (float)(radiusf * sin(angle));
			Point p;
			p.x = x;
			p.y = y;
			drawPoint_dxf(p);
			//plot_pixel(renderer, x, y);  // Draw pixel at (x, y)
		}
	} else {
		for (angle = startAngle; angle >= endAngle; angle -= 0.001) { // Increment angle to draw a smooth arc
			float x = centreXf + (float)(radiusf * cos(angle));
			float y = centreYf + (float)(radiusf * sin(angle));
			Point p;
			p.x = x;
			p.y = y;
			drawPoint_dxf(p);
			//plot_pixel(renderer, x, y);  // Draw pixel at (x, y)
		}
	}
}

void drawSegment_dxf(LineSegment s) {
	float x1 = s.a.x;
	float y1 = s.a.y;
	float x2 = s.b.x;
	float y2 = s.b.y;
	if (drawer_dxf) {
		dxf_line(dxf_file, (double)x1, (double)y1, (double)x2, (double)y2);
	}
	if (drawer_sdl) {

		#ifdef _DEBUG_PRINT_LAYER2
			printf("x1:    %2.4f\r\n", x1);
			printf("y1:    %2.4f\r\n", y1);
			printf("x2:    %2.4f\r\n", x2);
			printf("y2:    %2.4f\r\n", y2);
		#endif

		x1 *= ZOOM_FACTOR;
		y1 *= ZOOM_FACTOR;
		x2 *= ZOOM_FACTOR;
		y2 *= ZOOM_FACTOR;

		x1 += WINDOW_WIDTH / 2.0;
		y1 = (WINDOW_HEIGHT / 2.0) - y1;
		x2 += WINDOW_WIDTH / 2.0;
		y2 = (WINDOW_HEIGHT / 2.0) - y2;

		x1 += OFFSET_X;
		y1 += OFFSET_Y;
		x2 += OFFSET_X;
		y2 += OFFSET_Y;

		#ifdef _DEBUG_PRINT_LAYER2
			printf("x1:    %2.4f\r\n", x1);
			printf("y1:    %2.4f\r\n", y1);
			printf("x2:    %2.4f\r\n", x2);
			printf("y2:    %2.4f\r\n", y2);
		#endif
		setColor();
		SDL_RenderDrawLine(renderer, (int)x1, (int)y1, (int)x2, (int)y2);
	}
}

void drawLine_dxf(Line l) {
	setColor();
	if (l.m == INFINITY) {
		if (drawer_sdl) {
			SDL_RenderDrawLine(renderer, (int)l.c, (int)500, (int)l.c, (int)-500);
		}
		float x1 = (l.c * ZOOM_FACTOR) + WINDOW_WIDTH / 2.0;
		float y1 = WINDOW_HEIGHT;//y=mx+c
		float x2 = (l.c * ZOOM_FACTOR) + WINDOW_WIDTH / 2.0;
		float y2 = -OFFSET_Y;
		x1 += OFFSET_X;
		y1 += OFFSET_Y;
		x2 += OFFSET_X;
		y2 += OFFSET_Y;
		if (drawer_dxf) {
			dxf_line(dxf_file, (double)x1, (double)y1, (double)x2, (double)y2);
		}
	} else {
		if (l.m == 0.0) { 
			if (drawer_sdl) {
				SDL_RenderDrawLine(renderer, (int)0, (int)l.c, (int)500, (int)l.c);
			}
			float x1 = 0;
			float y1 = l.c;
			float x2 = WINDOW_WIDTH / ZOOM_FACTOR;
			float y2 = l.c;

			x1 *= ZOOM_FACTOR;
			y1 *= ZOOM_FACTOR;
			//x1 += WINDOW_WIDTH / 2;
			y1 = (WINDOW_HEIGHT / 2.0)-y1;

			x2 *= ZOOM_FACTOR;
			y2 *= ZOOM_FACTOR;
			//x2 += WINDOW_WIDTH / 2;
			y2 = (WINDOW_HEIGHT / 2.0)-y2;

			x1 += OFFSET_X;
			y1 += OFFSET_Y;
			x2 += OFFSET_X;
			y2 += OFFSET_Y;

			if (drawer_dxf) {
				dxf_line(dxf_file, (double)x1, (double)y1, (double)x2, (double)y2);
			}
		} else {
			if (drawer_sdl) {
				SDL_RenderDrawLine(renderer, (int)-500, (int)(-500 * l.m) + l.c, (int)+500, (int)(500 * l.m) + l.c);
			}
			float x1 = -WINDOW_WIDTH / ZOOM_FACTOR;
			float y1 = (x1 * l.m) + l.c;				//y=mx+c
			float x2 = WINDOW_WIDTH / ZOOM_FACTOR;//(y-c)/m=x
			float y2 = (x2 * l.m) + l.c;

			x1 *= ZOOM_FACTOR;
			y1 *= ZOOM_FACTOR;
			x1 += WINDOW_WIDTH / 2.0;
			y1 = (WINDOW_HEIGHT / 2.0) - y1;

			x2 *= ZOOM_FACTOR;
			y2 *= ZOOM_FACTOR;
			x2 += WINDOW_WIDTH / 2.0;
			y2 = (WINDOW_HEIGHT / 2.0) - y2;

			#ifdef _DEBUG_PRINT_LAYER2
				printf("x1:    %2.4f\r\n", x1);
				printf("y1:    %2.4f\r\n", y1);
				printf("x2:    %2.4f\r\n", x2);
				printf("y2:    %2.4f\r\n", y2);
			#endif
			x1 += OFFSET_X;
			y1 += OFFSET_Y;
			x2 += OFFSET_X;
			y2 += OFFSET_Y;
			
			if (drawer_dxf) {
				dxf_line(dxf_file, (double)x1, (double)y1, (double)x2, (double)y2);
			}
		}
	}
}

void drawPoint_dxf(Point p) {
	setColor();
	if (drawer_dxf) {
		dxf_point(dxf_file, p.x, p.y);
	}

	if (drawer_sdl) {
		p.x *= ZOOM_FACTOR;
		p.y *= ZOOM_FACTOR;
		p.x += WINDOW_WIDTH / 2.0;
		p.y = (WINDOW_HEIGHT / 2.0)-p.y;

		p.x += OFFSET_X;
		p.y += OFFSET_Y;


		int dx = 0;
		int dy = 0;
		for (dx = -5; dx < 5; dx++) {
			//SDL_RenderDrawPoint(renderer, p.x + dx, p.y);
		}
		for (dy = -5; dy < 5; dy++) {
			//SDL_RenderDrawPoint(renderer, p.x, p.y + dy);
		}
		SDL_RenderDrawPoint(renderer, p.x, p.y);
	}
}

void drawCircle_dxf(Circle c) {
	setColor();
	drawCircle_dxf_helper(renderer, c.o.x, c.o.y, c.r);
	//drawPoint_dxf(c.o);
}

void drawMyArc(MyArc a) {
	setColor();
	DrawMyArc_helper(renderer, a.c.o.x, a.c.o.y, a.c.r, a.radStart, a.radStop);
	//drawCircle_dxf_helper(renderer, a.c.o.x, a.c.o.y, a.c.r);
	//drawPoint_dxf(c.o);
}

void drawMyArcRO_dxf(MyArc a, float rad, Point offset, Point center) {
	a.c.o = rotatePoint(a.c.o, center, rad);
	a.c.o = movePoint(a.c.o, offset);
	a.radStart += rad;
	a.radStop += rad;
	drawMyArc(a);
}

void drawSegmentRO_dxf(LineSegment s, float rad, Point offset, Point center) {
	s.a = rotatePoint(s.a, center, rad);
	s.b = rotatePoint(s.b, center, rad);
	s.a = movePoint(s.a, offset);
	s.b = movePoint(s.b, offset);
	drawSegment_dxf(s);
}

void gear_draw_profile_shifter_calculator_segment_start(void) {
	psegment_first = 1;
}

void gear_draw_profile_shifter_calculator_segment(Point p) {
	if (psegment_first) {
		psegment_first = 0;
		psegment_start = p;
		psegment_prev = p;
	} else {
		LineSegment s = createLineSegment(psegment_prev, p);
		drawSegment_dxf(s);
		psegment_prev = p;
	}
}

void gear_draw_profile_shifter_calculator_segment_end(void) {
		LineSegment s = createLineSegment(psegment_prev, psegment_start);
		drawSegment_dxf(s);
}

