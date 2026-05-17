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

void gear_standard_calculator(float m, float alpha, float z1, float z2, GearStructure *gearA, GearStructure *gearB) {
	if ((gearA != NULL) && (gearB != NULL)) {
		gearA->m = m;
		gearA->alpha = alpha;
		gearA->z = z1;
		gearA->x = 0.0;

		gearB->m = m;
		gearB->alpha = alpha;
		gearB->z = z2;
		gearB->x = 0.0;

		gearA->a = (((gearA->z + gearB->z) * gearA->m) / 2.0);
		gearA->d = (gearA->z * gearA->m);
		gearA->db = gearA->d * cos(gearA->alpha);
		gearA->ha = 1.0 * gearA->m;
		gearA->h = 2.25 * gearA->m;
		gearA->da = gearA->d + 2.0 * gearA->ha;
		gearA->df = gearA->da - (2.5 * gearA->h);
		
	}
}

void gear_profile_shifter_calculator(float m, float alpha, float z1, float z2, float x1, float x2, GearStructure *gearA, GearStructure *gearB) {
	if ((gearA != NULL) && (gearB != NULL)) {
		//https://khkgears.net/new/gear_knowledge/gear_technical_reference/calculation_gear_dimensions.html
		gearA->m = m;
		gearA->alpha = alpha;
		gearA->z = z1;
		gearA->x = x1;

		gearB->m = m;
		gearB->alpha = alpha;
		gearB->z = z2;
		gearB->x = x2;

		gearA->inv_alphaw = 2.0 * tan(gearA->alpha) * ( (gearA->x + gearB->x) / (gearA->z + gearB->z) ) + involute_function(gearA->alpha);
		gearA->alphaw = gear_working_pressure_angle(gearA->inv_alphaw);
		gearA->y = ((gearA->z + gearB->z) / 2.0) * (((cos(gearA->alpha) / cos (gearA->alphaw)) - 1.0));
		gearA->a = ((gearA->z + gearB->z) / (2.0 + (gearA->y))) * gearA->m;
		gearA->a = (((gearA->z + gearB->z) / 2.0) + (gearA->y)) * gearA->m;
		gearA->d = (gearA->z * gearA->m);
		gearA->db = gearA->d * cos(gearA->alpha);
		gearA->dw = gearA->db / cos(gearA->alphaw);
		gearA->ha = (1.0 + gearA->y - gearB->x) * gearA->m;
		gearA->h = (2.25 + gearA->y - (gearA->x + gearB->x)) * gearA->m;
		gearA->da = gearA->d + 2.0 * gearA->ha;
		gearA->df = gearA->da - (2.0 * gearA->h);

		gearA->tooth_angle = DEG2RAD(360.0 / gearA->z);
		gearA->tip_tetha = (M_PI + (4.0 * gearA->x * tan(gearA->alpha) )) / (gearA->z);

		gearA->rising_wp_dphi = involute_function_d_to_phi(gearA->dw, gearA->db);
		gearA->rising_dphi = involute_function_d_to_phi(gearA->da, gearA->db);
		gearA->tip_dphi = (DEG2RAD(180.0) / gearA->z) - (2.0 * (gearA->rising_dphi - gearA->rising_wp_dphi));
		if (gearA->tip_dphi <= 0.0) {
			gearA->tip_dphi = 0;
		}
		gearA->falling_dphi = gearA->rising_dphi;
		gearA->falling_wp_dphi = gearA->rising_wp_dphi;
		gearA->root_dphi = gearA->tooth_angle - (gearA->tip_dphi + gearA->rising_dphi + gearA->rising_dphi);
		if (gearA->root_dphi <= 0.0) {
			gearA->root_dphi = 0.0;
		}

		gearA->rising_phi = gearA->rising_dphi;
		gearA->tip_phi = gearA->rising_phi + gearA->tip_dphi;
		gearA->falling_phi = gearA->tip_phi + gearA->falling_dphi;
		gearA->root_phi = gearA->falling_phi + gearA->root_dphi;

		gearB->inv_alphaw = 2.0 * tan(gearB->alpha) * ( (gearB->x + gearA->x) / (gearB->z + gearA->z) ) + involute_function(gearB->alpha);
		gearB->alphaw = gear_working_pressure_angle(gearB->inv_alphaw);
		gearB->y = ((gearB->z + gearA->z) / 2.0) * (((cos(gearB->alpha) / cos (gearB->alphaw)) - 1.0));
		gearB->a = ((gearB->z + gearA->z) / (2.0 + (gearB->y))) * gearB->m;
		gearB->a = (((gearB->z + gearA->z) / 2.0) + (gearB->y)) * gearB->m;
		gearB->d = (gearB->z * gearB->m);
		gearB->db = gearB->d * cos(gearB->alpha);
		gearB->dw = gearB->db / cos(gearB->alphaw);
		gearB->ha = (1.0 + gearB->y - gearA->x) * gearB->m;
		gearB->h = (2.25 + gearB->y - (gearB->x + gearA->x)) * gearB->m;
		gearB->da = gearB->d + 2.0 * gearB->ha;
		gearB->df = gearB->da - (2.0 * gearB->h);

		gearB->tooth_angle = DEG2RAD(360.0 / gearA->z);
		gearB->tip_tetha = (M_PI + (4.0 * gearB->x * tan(gearB->alpha) )) / (gearB->z);

		gearB->rising_wp_dphi = involute_function_d_to_phi(gearB->dw, gearB->db);
		gearB->rising_dphi = involute_function_d_to_phi(gearB->da, gearB->db);
		gearB->tip_dphi = (DEG2RAD(180.0) / gearB->z) - (2.0 * (gearB->rising_dphi - gearB->rising_wp_dphi));
		if (gearB->tip_dphi <= 0.0) {
			gearB->tip_dphi = 0;
		}
		gearB->falling_dphi = gearB->rising_dphi;
		gearB->falling_wp_dphi = gearB->rising_wp_dphi;
		gearB->root_dphi = gearB->tooth_angle - (gearB->tip_dphi + gearB->rising_dphi + gearB->rising_dphi);
		if (gearB->root_dphi <= 0.0) {
			gearB->root_dphi = 0.0;
		}

		gearB->rising_phi = gearB->rising_dphi;
		gearB->tip_phi = gearB->rising_phi + gearB->tip_dphi;
		gearB->falling_phi = gearB->tip_phi + gearB->falling_dphi;
		gearB->root_phi = gearB->falling_phi + gearB->root_dphi;



		//gear_calculator_x1x2(&x1, &x2, gearA->a, gearA->m, gearA->z, gearB->z, gearA->alpha);
		//gearA->x = x1;
		//gearB->x = x2;
		//printf("x:         %2.4f %2.4f\r\n", 	(gearA->x), (gearB->x));
		//printf("x:         %2.4f %2.4f\r\n", 	(x1), (x2));


	}

	//{
	//	unsigned int x = 0;
	//	for ( x = 0; x < 180; x+= 10) {
	//		float angle = DEG2RAD(x);
	//		float rb = 10.0;
	//		float resultx = rb * (cos(angle) + (angle * sin(angle)) );
	//		float resulty = rb * (sin(angle) + (angle * cos(angle)) );
	//		printf("result:        %2.4f %2.4f\r\n", resultx, resulty);
	//	}
	//}

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
