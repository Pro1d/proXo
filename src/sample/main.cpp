#include <cstdlib>

#include "core/math/basics.h"
#include "core/model/Scene.h"
#include "core/raytracer/RayTracer.h"
#include "core/realtime/Buffer.h"
#include "core/realtime/Engine.h"
#include "parser/SceneParser.h"
#include "sdl/CameraController.h"
#include "sdl/wrapper.h"
#include <SDL/SDL.h>

#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768
#define SAMPLE_SIZE 1

using namespace proxo;

int main(int argc, char** argv)
{
	// initialize SDL video
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}
	// make sure SDL cleans up before exit
	atexit(SDL_Quit);


	// Load scene from file
	SceneParser parser;
	Scene scene;
	const char* sceneFileName = argc >= 2 ? argv[1] : "parc.scene";
	if(!parser.readScene("media/", sceneFileName, scene)) {
		printf("readScene failed!\n");
		return 2;
	}
	scene.printSize();

	positive screen_width = scene.camera.targetWidth;
	positive screen_height = scene.camera.targetHeight;
	positive sample_size = 1 << scene.camera.supersampling;

	// create a new window
	SDL_Surface* screen = SDL_SetVideoMode(screen_width, screen_height, 24,
	    SDL_HWACCEL | SDL_HWSURFACE | SDL_DOUBLEBUF);
	if(!screen) {
		printf("Unable to set %dx%d video: %s\n", screen_width, screen_height,
		    SDL_GetError());
		return 4;
	}
	
	Buffer buf(screen_width * sample_size, screen_height * sample_size);
	
	Engine realTimeEngine(&buf, &scene);
	realTimeEngine.createMatchingPool();

	RayTracer rayTracer(&buf, &scene);
	rayTracer.createMatchingPool();

	CameraController controller;
	controller.setCamera(&scene.camera);
	controller.setTranslateSpeed(1.5);
	controller.setRotateSpeed(30);

	controller.assignKey(MOVE_FORWARD, SDLK_w);
	controller.assignKey(MOVE_BACKWARD, SDLK_s);
	controller.assignKey(MOVE_LEFT, SDLK_a);
	controller.assignKey(MOVE_RIGHT, SDLK_d);
	controller.assignKey(MOVE_UP, SDLK_SPACE);
	controller.assignKey(MOVE_DOWN, SDLK_v);
	controller.assignKey(YAW_LEFT, SDLK_q);
	controller.assignKey(YAW_RIGHT, SDLK_e);
	controller.assignKey(ROLL_LEFT, SDLK_z);
	controller.assignKey(ROLL_RIGHT, SDLK_x);
	controller.assignKey(PITCH_DOWN, SDLK_r);
	controller.assignKey(PITCH_UP, SDLK_f);
	controller.setModifiersForStepByStep(KMOD_LCTRL);
	controller.setModifiersHighSpeed(KMOD_LALT);
	controller.assignMouse(FOV_INCREASE, SDL_BUTTON_WHEELDOWN);
	controller.assignMouse(FOV_DECREASE, SDL_BUTTON_WHEELUP);
	controller.assignMouse(GRAB_MOUSE, SDL_BUTTON_RIGHT);
	controller.assignMouse(PITCH_UP, MOUSE_Y);
	controller.assignMouse(YAW_LEFT, MOUSE_X);

	SDL_Surface* buffer = SDL_CreateRGBSurface(
	    SDL_HWSURFACE, screen_width, screen_height, 24, 0, 0, 0, 0);
	Uint32 time               = SDL_GetTicks();
	bool rayTracingRenderView = false;

	BufferToBitmap btb(buf, buffer, sample_size);

	// program main loop
	bool done = false;
	while(!done) {
		real dT = (real)(SDL_GetTicks() - time) / 1000;
		time    = SDL_GetTicks();
		// message processing loop
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			controller.handleEvent(event);
			// check for messages
			switch(event.type) {
				// exit if the window is closed
				case SDL_QUIT:
					done = true;
					break;

				// check for keypresses
				case SDL_KEYDOWN: {
					// exit if ESCAPE is pressed
					switch(event.key.keysym.sym) {
						case SDLK_ESCAPE:
							done = true;
							break;
						case SDLK_F4:
							if((SDL_GetModState() & KMOD_ALT) != KMOD_NONE)
								done = true;
							break;
						case SDLK_F10:
							if(!rayTracingRenderView) {
								Uint32 t = SDL_GetTicks();
								rayTracer.render();
								printf("Ray tracer - render time: %f seconds\n", (float)(SDL_GetTicks() - t)/1000);
								rayTracingRenderView = true;
							}
							else {
								rayTracingRenderView = false;
							}
							break;
						default:
							break;
					}
					break;
				}
			} // end switch
		} // end of message processing

		controller.updateCamera(dT);

		// DRAWING STARTS HERE

		if(!rayTracingRenderView) {
			Uint32 t = SDL_GetTicks();
			realTimeEngine.render();
			printf("t:%dms ", SDL_GetTicks() - t);
			t = SDL_GetTicks();
			btb.convert();
			printf("%dms \n", SDL_GetTicks() - t);
		}
		else {
			bufferToBitmap24bpp(buf, buffer, sample_size);
		}

		// clear screen
		SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));

		// draw bitmap
		SDL_BlitSurface(buffer, 0, screen, NULL);

		// DRAWING ENDS HERE

		// finally, update the screen :)
		SDL_Flip(screen);
	} // end main loop

	// free loaded bitmap
	SDL_FreeSurface(buffer);

	// all is well ;)
	printf("Exited cleanly\n");
	return 0;
}
