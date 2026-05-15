#ifndef _GEAR_CALCULATOR_H_
#define _GEAR_CALCULATOR_H_

	typedef struct _GearStructure {
		float m;			//module
		float alpha;		//Reference Pressure Angle [rad]
		float z;			//Number of Teeth
		float x;			//Profile Shift Coefficient

		float inv_alphaw;	//Involute αw
		float alphaw;		//Working Pressure Angle
		float y;			//Center Distance Modification Coefficient
		float a;			//Center Distance
		float d;			//Reference Diameter
		float db;			//Base Diameter
		float dw;			//Working Pitch Diameter
		float ha;			//Addendum
		float h;			//Tooth Depth
		float da;			//Tip Diameter
		float df;			//Root Diameter
		
		float tooth_angle;
		float tip_tetha;	//Single Tip Angle Width On Pitch Circle

		float rising_wp_dphi;
		float rising_dphi;
		float tip_dphi;
		float falling_dphi;
		float falling_wp_dphi;
		float root_dphi;

		float rising_phi;
		float tip_phi;
		float falling_phi;
		float root_phi;
	} GearStructure;

	extern void gear_calculator_x1x2(float *x1, float *x2, float a, float m, float z1, float z2, float alpha);
	extern void gear_teeth_calculator(float m, float a, float i, float *z1, float *z2);
	extern void gear_calculator_x1x2(float *x1, float *x2, float a, float m, float z1, float z2, float alpha);
	extern void gear_standard_calculator(float m, float alpha, float z1, float z2, GearStructure *gearA, GearStructure *gearB);
	extern void gear_profile_shifter_calculator(float m, float alpha, float z1, float z2, float x1, float x2, GearStructure *gearA, GearStructure *gearB);

#endif
