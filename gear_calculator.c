#include <math.h>
#include <stdio.h>
#include "gear_calculator.h"
#include "geometry.h"

static float gear_working_pressure_angle(float inv_alphaw);
static float involute_function(float alpha);
static float involute_function_d_to_phi(float d, float db);

void gear_teeth_calculator(float m, float a, float i, float *z1, float *z2) {
	if ((z1 != NULL) && (z2 != NULL)) {
		float z1z2 = (2.0 * a) / m;
		*z1 = (z1z2) / (i + 1);
		*z2 = (i*(z1z2)) / (i + 1);
	}
}

void gear_profile_shifter_calculator(float m, float alpha, float z1, float z2, float x1, float x2, GearStructure *gear) {
	if (gear != NULL) {
		//https://khkgears.net/new/gear_knowledge/gear_technical_reference/calculation_gear_dimensions.html
		gear->m = m;
		gear->alpha = alpha;
		gear->z = z1;
		gear->x = x1;

		gear->inv_alphaw = 2.0 * tan(gear->alpha) * ( (gear->x + x2) / (gear->z + z2) ) + involute_function(gear->alpha);
		gear->inv_alphaw = 2.0 * tan(gear->alpha) * ( (gear->x + x2) / (gear->z + z2) ) + involute_function(gear->alpha);
		gear->alphaw = gear_working_pressure_angle(gear->inv_alphaw);
		gear->y = ((gear->z + z2) / 2.0) * (((cos(gear->alpha) / cos (gear->alphaw)) - 1.0));
		gear->a = ((gear->z + z2) / (2.0 + (gear->y))) * gear->m;
		gear->a = (((gear->z + z2) / 2.0) + (gear->y)) * gear->m;
		gear->d = (gear->z * gear->m);
		gear->db = gear->d * cos(gear->alpha);
		gear->dw = gear->db / cos(gear->alphaw);
		gear->ha = (1.0 + gear->y - x2) * gear->m;
		gear->h = (2.25 + gear->y - (gear->x + x2)) * gear->m;
		gear->da = gear->d + 2.0 * gear->ha;
		gear->df = gear->da - (2.0 * gear->h);

		gear->tooth_angle = DEG2RAD(360.0 / gear->z);
		gear->tip_tetha = (M_PI + (4.0 * gear->x * tan(gear->alpha) )) / (gear->z);

		gear->rising_wp_dphi = involute_function_d_to_phi(gear->dw, gear->db);
		gear->rising_dphi = involute_function_d_to_phi(gear->da, gear->db);
		gear->tip_dphi = (DEG2RAD(180.0) / gear->z) - (2.0 * (gear->rising_dphi - gear->rising_wp_dphi));
		if (gear->tip_dphi <= 0.0) {
			gear->tip_dphi = 0;
		}
		gear->falling_dphi = gear->rising_dphi;
		gear->falling_wp_dphi = gear->rising_wp_dphi;
		gear->root_dphi = gear->tooth_angle - (gear->tip_dphi + gear->rising_dphi + gear->rising_dphi);
		if (gear->root_dphi <= 0.0) {
			gear->root_dphi = 0.0;
		}

		gear->rising_phi = gear->rising_dphi;
		gear->tip_phi = gear->rising_phi + gear->tip_dphi;
		gear->falling_phi = gear->tip_phi + gear->falling_dphi;
		gear->root_phi = gear->falling_phi + gear->root_dphi;
	}
}

void gear_print(GearStructure *gear) {
	if (gear != NULL) {
		printf("m:         %2.4f\r\n",	(gear->m));
		printf("alpha:     %2.4f\r\n",	RAD2DEG(gear->alpha));
		printf("z:         %2.4f\r\n", 	(gear->z));
		printf("x:         %2.4f\r\n", 	(gear->x));

		printf("inv_alphaw:%2.4f\r\n",	(gear->inv_alphaw));
		printf("alphaw:    %2.4f\r\n",	RAD2DEG(gear->alphaw));

		printf("y:         %2.4f\r\n",	(gear->y));
		printf("a:         %2.4f\r\n",	(gear->a));
		printf("d:         %2.4f\r\n", 	(gear->d));
		printf("db:        %2.4f\r\n", 	(gear->db));
		printf("dw:        %2.4f\r\n", 	(gear->dw));
		printf("ha:        %2.4f\r\n", 	(gear->ha));
		printf("h:         %2.4f\r\n",	(gear->h));
		printf("da:        %2.4f\r\n", 	(gear->da));
		printf("df:        %2.4f\r\n", 	(gear->df));
		
		
		
		printf("tooth_angle        %2.4f\r\n",	RAD2DEG(gear->tooth_angle));
		printf("tip_tetha          %2.4f\r\n",	RAD2DEG(gear->tip_tetha));

		printf("rising_wp_dphi:   %2.4f\r\n",	RAD2DEG(gear->rising_wp_dphi));
		printf("rising_dphi:      %2.4f\r\n",	RAD2DEG(gear->rising_dphi));
		printf("tip_dphi:         %2.4f\r\n",	RAD2DEG(gear->tip_dphi));
		printf("falling_dphi:     %2.4f\r\n",	RAD2DEG(gear->falling_dphi));
		printf("falling_wp_dphi:  %2.4f\r\n",	RAD2DEG(gear->falling_wp_dphi));
		printf("root_dphi:        %2.4f\r\n",	RAD2DEG(gear->root_dphi));

		printf("rising_phi:       %2.4f\r\n",	RAD2DEG(gear->rising_phi));
		printf("tip_phi:          %2.4f\r\n",	RAD2DEG(gear->tip_phi));
		printf("falling_phi:      %2.4f\r\n",	RAD2DEG(gear->falling_phi));
		printf("root_phi:         %2.4f\r\n",	RAD2DEG(gear->root_phi));
	}
}

void gear_calculator_x1x2(float *x1, float *x2, float a, float m, float z1, float z2, float alpha) {
	if ((x1 != NULL) && (x2 != NULL)) {
		float x1x2 = 0.0;

		float y = (a / m) - ((z1 + z2) / 2.0);
		float alphaw = acos( cos(alpha) / (((2.0 * y) / (z1+z2)) + 1.0) );

		x1x2 = (z1 + z2);
		x1x2 *= (involute_function(alphaw) - involute_function(alpha));
		x1x2 /= 2.0 * tan(alpha);
		//printf("x1x2:         %2.4f\r\n", 	x1x2);
		if (z1 < z2) {
			*x1 = (x1x2 / (z1 + z2) * z2);
			*x2 = x1x2 - *x1;
		} else {
			*x2 = (x1x2 / (z1 + z2) * z2);
			*x1 = x1x2 - *x1;
		}
	}
}

static float gear_working_pressure_angle(float inv_alphaw) {
	unsigned int x = 10;
	float result = 0.0;
	
	result = DEG2RAD(20.0);
	while (x) {
		float a = involute_function(result) - inv_alphaw;
		float b = pow( (1.0 / (cos(result))),2.0) - 1.0;
		result = result - ((a) / (b));
		x--;
	}
	return result;
}

static float involute_function(float alpha) {
	float result = 0.0;
	result = tan(alpha) - alpha;
	return result;
}

static float involute_function_d_to_phi(float d, float db) {
	float result = 0.0;
	float r = d / 2.0;
	float rb = db / 2.0;
	float a = sqrt( pow((r/rb), 2.0) - 1.0 );
	float b = atan(a);
	result = a - b;
	return result;
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

void involute_curve(float *x, float *y, float rb, float tetha) {
	if ((x != NULL) && (y != NULL)) {
		*x = rb * (cos(tetha) + (tetha * sin(tetha)) );
		*y = rb * (sin(tetha) - (tetha * cos(tetha)) );
	}
}
