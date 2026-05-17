#define NOMINMAX
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "gear.h"
#include "geometry.h"
#include "cJSON.h"

// Window dimensions
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
unsigned char gameRunning = 0;
unsigned int z1 = 12;
unsigned int z2 = 12;
float module = 3.0;
float pressureAngleDeg = 20.0;
float x1 = 0.0;
float x2 = 0.0;
GearStructure gearA;
GearStructure gearB;
float distance = 0.0;
unsigned int visible_diameters = 0;


int file_exists(const char *fname);

//int main(int argc, char* argv[]) {
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	static unsigned int singleShoot = 1;
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
	TTF_Init();

	int displays = SDL_GetNumVideoDisplays();
	SDL_Rect displayBounds[displays];
	for( int i = 0; i < displays; i++ ) {
		SDL_GetDisplayBounds( i, &displayBounds[i] );
	}
	int x = displayBounds[ 0 ].x + 100;
	int y = displayBounds[ 0 ].y + 100;
	int w = WINDOW_WIDTH;
	int h = WINDOW_HEIGHT;

    // Create window
    //SDL_Window* window = SDL_CreateWindow("SDL Pixel Drawing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Window* window = SDL_CreateWindow("SDL Pixel Drawing", x, y, w, h, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // Create renderer for the window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Load font
    font = TTF_OpenFont(".\\Steiner-1.ttf", 24);  // Make sure arial.ttf exists in your working directory
    if (!font) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Set the draw color (e.g., red)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Clear the screen with the current color
    SDL_RenderClear(renderer);

	
    // Present the rendered content to the screen
    SDL_RenderPresent(renderer);
	
	{
		if (file_exists("gear.json")) {
			FILE *json_file = fopen("gear.json", "r");
			if (json_file != NULL) {
				fseek(json_file, 0, SEEK_END); 
				size_t size = ftell(json_file);
				fseek(json_file, 0, SEEK_SET); 
				void *fcontent = malloc(size);
				fread(fcontent, 1, size, json_file);
				
				
				cJSON *root = cJSON_Parse(fcontent);
				cJSON *value = NULL;
				
				value = cJSON_GetObjectItem(root, "z1");
				z1 = value->valueint;
				
				value = cJSON_GetObjectItem(root, "z2");
				z2 = value->valueint;
				
				value = cJSON_GetObjectItem(root, "m");
				module = value->valuedouble;
				
				value = cJSON_GetObjectItem(root, "p");
				pressureAngleDeg = value->valuedouble;
				
				value = cJSON_GetObjectItem(root, "x1");
				x1 = value->valuedouble;
				
				value = cJSON_GetObjectItem(root, "x2");
				x2 = value->valuedouble;
			
				free(value);
				free(root);
				free(fcontent);
				fclose(json_file);
			}
			
		} else {
			FILE *json_file = fopen("gear.json", "w");

			if (json_file != NULL) {
				cJSON *root = cJSON_CreateObject();

				cJSON_AddStringToObject(root, "name", "Gearbox");
				cJSON_AddNumberToObject(root, "z1", 12);
				cJSON_AddNumberToObject(root, "z2", 24);
				cJSON_AddNumberToObject(root, "m", 3.0);
				cJSON_AddNumberToObject(root, "p", 20.0);
				cJSON_AddNumberToObject(root, "x1", 0.6);
				cJSON_AddNumberToObject(root, "x2", 0.36);

				char *json = cJSON_Print(root);

				printf("%s\n", json);
				fprintf(json_file, "%s\n", json);
				
				cJSON_Delete(root);
				free(json);
				
				fclose(json_file);
			}
		}
	}
	
		
    // Wait for 5 seconds
	gameRunning = 1;
	unsigned int frames = 0;
	while (gameRunning) {
		SDL_Delay(1);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		frames++;
		float rotation = frames * 0.001;

		gear_profile_shifter_calculator(module, DEG2RAD(pressureAngleDeg), z1, z2, x1, x2, &gearA);
		gear_profile_shifter_calculator(module, DEG2RAD(pressureAngleDeg), z2, z1, x2, x1, &gearB);
		distance = gearA.a;


		if (singleShoot) {
			singleShoot = 0;
			gear_dxf(0.0, 0.0, 0.0, visible_diameters, &gearA, "gearA.dxf");
			gear_dxf(distance, 0.0, 0.0, visible_diameters, &gearB, "gearB.dxf");

			printf("Gear A\r\n");
			gear_print(&gearA);
			printf("Gear B\r\n");
			gear_print(&gearB);

		}
		
	
		//rotation = 0.0;
		float rotationA = rotation;
		float rotationB = rotation * (gearA.z / gearB.z) + DEG2RAD(180.0) + (0.7 * DEG2RAD(360.0) / gearB.z);
		
		gear_sdl(0.0, 0.0, rotationA, visible_diameters, &gearA);
		gear_sdl(distance, 0.0, -rotationB, visible_diameters, &gearB);
		SDL_RenderPresent(renderer);
		{
			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
					case SDL_APP_TERMINATING : 
					case SDL_QUIT : {
						gameRunning = 0;
						break;
					}
					case SDL_KEYDOWN :
					case SDL_KEYUP : {
						if (event.key.state == SDL_PRESSED) {
							extern float OFFSET_X;
							extern float OFFSET_Y;
							extern float ZOOM_FACTOR;

							if (event.key.keysym.sym == SDLK_a) {
								OFFSET_X += 3 * ZOOM_FACTOR;
							}
							if (event.key.keysym.sym == SDLK_d) {
								OFFSET_X -= 3 * ZOOM_FACTOR;
							}
							if (event.key.keysym.sym == SDLK_s) {
								OFFSET_Y -= 3 * ZOOM_FACTOR;
							}
							if (event.key.keysym.sym == SDLK_w) {
								OFFSET_Y += 3 * ZOOM_FACTOR;
							}

							if (event.key.keysym.sym == SDLK_r) {
								OFFSET_Y /= ZOOM_FACTOR;
								ZOOM_FACTOR += 1.0;
								OFFSET_Y *= ZOOM_FACTOR;
							}
							if (event.key.keysym.sym == SDLK_t) {
								OFFSET_Y /= ZOOM_FACTOR;
								ZOOM_FACTOR -= 1.0;
								OFFSET_Y *= ZOOM_FACTOR;
							}

							if (event.key.keysym.sym == SDLK_5) {
								module += 1.0;
							}
							if (event.key.keysym.sym == SDLK_6) {
								module -= 1.0;
							}
							if (event.key.keysym.sym == SDLK_7) {
								z1 += 1;
							}
							if (event.key.keysym.sym == SDLK_8) {
								if (z1 > 1) {
									z1 -= 1;
								}
							}
							if (event.key.keysym.sym == SDLK_1) {
								z2 += 1;
							}
							if (event.key.keysym.sym == SDLK_2) {
								if (z2 > 1) {
									z2 -= 1;
								}
							}

							if (event.key.keysym.sym == SDLK_3) {
								if (visible_diameters) {
									visible_diameters = 0;
								} else {
									visible_diameters = 1;
								}
							}


							if (event.key.keysym.sym == SDLK_m) {
								if (pressureAngleDeg < 90.0) {
									pressureAngleDeg += 1.0;
								}
							}
							if (event.key.keysym.sym == SDLK_n) {
								if (pressureAngleDeg > 1.0) {
									pressureAngleDeg -= 1.0;
								}
							}

							if (event.key.keysym.sym == SDLK_k) {
								if (x1 < 10.0) {
									x1 += 0.01;
								}
							}
							if (event.key.keysym.sym == SDLK_l) {
								if (x1 > 0.01) {
									x1 -= 0.01;
								}
							}


							if (event.key.keysym.sym == SDLK_q) {
								gameRunning = 0;
							}
						}
						break;
					}
					default : {
						break;
					}
				}
			}
		}
		SDL_Delay(16);
		//SDL_Delay(1500);
		//break;
	}
	//_getch();

    // Clean up and close SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

int file_exists(const char *fname)
{
    FILE *file;
    if ((file = fopen(fname, "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
}
