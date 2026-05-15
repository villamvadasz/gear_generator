# Involute gear generator project
This is a simple C project to generate a profile shifter involute gear. It can draw the gear into a window using SDL2, and also it will emitt DXF files.
However the project is in early shape, so dont expect any user friendly GUI. And also the file naming and architecture is strange.

## Dependencies
gcc-14.2.0-no-debug
SDL2 (included)
cJSON (included)
Windows

## How to use it
Install gcc-14.2.0-no-debug
Build the project and run with 
```
build.bat
``` 
this will emitt a gear.json file.
Edit the json file as you want.
```
{
	"name":	"Gearbox",
	"z1":	12, //Tooth number of gear 1
	"z2":	24, //Tooth number of gear 2
	"m":	3, //Modulo number
	"p":	20, //Pressure angle
	"x1":	0.36, //Modification of gear 1
	"x2":	0.6 //Modification of gear 1
}
```
run the project with 
```
build.bat
```
This will generate two DXF files respective for gear 1 and gear 2. And also open a windows and show you gear 1.

### Keyboard commands
In the GUI you can use some keys to change parameters:
```
w, a, s, d - Moving view
r and t - Zoom
5 and 6 - module
7 and 8 - Tooth count
m and n - pressure angle
k and l - Modification
q for exit
```

## Used sites
Beside chatgpt which told me a lot for formulas just by asking it for:

https://khkgears.net/new/gear_knowledge/gear_technical_reference/calculation_gear_dimensions.html

https://www.fictiv.com/articles/creating-involute-gears-in-cad

https://khkgears.net/new/gear_knowledge/gear_technical_reference/involute_gear_profile.html

http://www.gearseds.com/files/Approx_method_draw_involute_teeth_rev2.pdf

# Gears
I am not a mechanical engineer, so this stuff represents what I understood during implementing the project. So basically, mechanical engineers developed the gears. For me it was just something with lot of protractions on the edge around a round stuff. Usually when they break, stuffs does not work any more. And ofthen they are not replacable. There are small ones and big ones. Some of them are from plastics and some of them are from metall. But how on earth do they draw one? Do they have a golden sample and only the oldest engineers are allowed to copy it? Why are there so many differnt types of it?
So it turned out, they dont have a golden sample, but insted some golden mathematics. And I also found out, that they are different type of gears, however now I will just focus on the involute gear. I dont even know all the other ones that exists. 
## Involute gear
Take a round object (circle), turn a string around it tight. Grab the end of the string, which is right on the surface of the round object. Unwrap it. Hold it tight. Imagine the end have a pen on it and draws a line. This line would be the involute line. Mirroring this line at the right position and chopping of the excess. The tooth of a gear is born! Do this several times and you have the involute gear read. More or less.
## Implementing the involute line
What is easy on the paper, it is not so easy on in notepad++. First we have a bunch of numbers to calculate. Mostly diameters and angles.
Based on this numbers, you can now take something to draw where you can input your x and y coordinates. Here you will really fast notice, that drawing the involute line
```
void involute_curve(float *x, float *y, float rb, float tetha) {
	if ((x != NULL) && (y != NULL)) {
		*x = rb * (cos(tetha) + (tetha * sin(tetha)) );
		*y = rb * (sin(tetha) - (tetha * cos(tetha)) );
	}
}
```
is the easyiest part. However a small problem is here, that tetha is the angle where the string is unwrapped and touching the circle. And it is not the angle that points to the end of the string. Finding out that it is not possible to calculate it out, just by iteration makes it not trivial to implement. Unless you can ask chatgpt and it tells you instantly that
```
float solve_theta(float phi) {
    float t = phi + 0.5f; // initial guess
    for(int i=0; i<10; i++) {
        float f  = t - atanf(t) - phi;
        float df = 1.0f - 1.0f/(1.0f + t*t);
        t = t - f/df;
    }
    return t;
}
```
you can do it. Thanks to Newton. Only thig is you have to guess a single number. No not the initial guess. The step. Either you guess how many steps you want, or you define a minimum change to stop iteration. So guess a number. Another funny thing is if you want to have the angle that points again to the end of the string, but also the distance should be a given one. Like asking the question, when does this curve will reach the desired tip diameter?
```
static float involute_function_d_to_phi(float d, float db) {
	float result = 0.0;
	float r = d / 2.0;
	float rb = db / 2.0;
	float a = sqrt( pow((r/rb), 2.0) - 1.0 );
	float b = atan(a);
	result = a - b;
	return result;
}
```
Thankfully chatpgt delivered the mathematical background, and implementing it was easy. Not really sure where this info can be found on the internet. I was not able to find any site that discussed this part of a gear generating.
Lets see what parameters are the most important ones. Actually every one is important, because is just one is missing there is no gear.
```
	typedef struct _GearStructure {
  //This are just general parameters that define the gear.
		float m;			//module
		float alpha;  //Reference Pressure Angle [rad]
		float z;			//Number of Teeth
		float x;			//Profile Shift Coefficient

//This are important ones
		float db;			//Base Diameter: Here the involute curve starts. (no it does not start at the root)
		float da;			//Tip Diameter: This high the involute curve is allowed to go.
		float df;			//Root Diameter: This is lower thatn the base diameter. But it is fine so.

//This is are angles for a single tooth
		float rising_dphi;//When the involute line reaches the tip diameter
		float tip_dphi; //From starting of the tip to the end.
                    //Basically to calculate this one, you must know that where a tooth rises out from the material, till it comes back is defined by dividing the circle with the tooth number
		float falling_dphi;//When the involute line reaches the base diameter
		float root_dphi;//From starting of the root to the end

	} GearStructure;
```
Basically generating the lines is now easy. We can calculate how to pixelise an involute curve. We can also calculate at witch angle will the involute curve reach the tip. The tip is just a round arc. And the falling part of the tooth is again the involute curve. Just it needs a proper placement so that it falls from the tip to the root. Then the root is again just an arc. Also the angles are know, where to draw involutes, where to draw arcs.

Most of the calculation is done here:
```
void gear_profile_shifter_calculator(float m, float alpha, float z1, float z2, float x1, float x2, GearStructure *gearA, GearStructure *gearB)
```
Drawing is done here:
```
static void gear_draw_profile_shifter_calculator(unsigned int z1, unsigned int z2, float module, float pressureAngleDeg, float x1, float x2, float rotation)
```
The drawer loops around a circle by small angle steps. This is then checked if it is in a range of involute rising, tip, involute falling or root angle:
```
	for (t = 0; t < z1; t++) {
		for (x = 0; x < max_step; x++) {
			float angleInsideTooth = (anglePerTooth * ((float)x / (float)max_step));
			angle = t * anglePerTooth;

			if (angleInsideTooth < gearA.rising_phi) {
			} else if (angleInsideTooth < gearA.tip_phi) {
			} else if (angleInsideTooth < gearA.falling_phi) {
			} else if (angleInsideTooth < gearA.root_phi) {
			}
		}
	}
```
