#include <cstdlib>

#include "parser/SceneParser.h"
#include "core/model/Scene.h"
#include "core/realtime/Buffer.h"
#include "core/realtime/Engine.h"
#include "sdl/wrapper.h"
#include "sdl/CameraController.h"
#include <SDL/SDL.h>
#include "core/math/basics.h"

int main(int argc, char** argv)
{
    // initialize SDL video
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Unable to init SDL: %s\n", SDL_GetError());
        return 1;
    }
    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    // create a new window
    SDL_Surface* screen = SDL_SetVideoMode(640, 480, 24,
                                           SDL_HWACCEL|SDL_HWSURFACE|SDL_DOUBLEBUF);
    if(!screen ) {
        printf("Unable to set 640x480 video: %s\n", SDL_GetError());
        return 4;
    }

    SceneParser parser;
    Scene scene;
    Buffer buf(640, 480);
    if(!parser.readScene("media/","parc.scene", scene)) {
        printf("readScene failed!\n");
        return 2;
    }
    real fov = 70 * PI / 180;
    real zNear = 0.001;
    real size = tan(fov/2) * zNear * 2;
    real ratio = 480.0 / 640.0;
    scene.camera.setFrustrum(zNear, 100, size, size*ratio);
    //scene.camera.setOrthographics(0.001, 10, 2,2);
    scene.camera.setScreenSize(640, 480);
    scene.print();
    Engine realTimeEngine(&buf, &scene);
    realTimeEngine.createMatchingPool();

    CameraController controller;
    controller.setCamera(&scene.camera);
    controller.setTranslateSpeed(0.6);
    controller.setRotateSpeed(30);


    SDL_Surface * buffer = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_HWSURFACE, 640, 480, 24, 0,0,0,0);
    Uint32 time = SDL_GetTicks();

    // program main loop
    bool done = false;
    while (!done)
    {
        real dT = (real) (SDL_GetTicks()-time) / 1000;
        time = SDL_GetTicks();
        // message processing loop
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            controller.handleEvent(event);
            // check for messages
            switch (event.type)
            {
                // exit if the window is closed
            case SDL_QUIT:
                done = true;
                break;

                // check for keypresses
            case SDL_KEYDOWN:
                {
                    // exit if ESCAPE is pressed
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        done = true;
                    break;
                }
            } // end switch
        } // end of message processing

        controller.updateCamera(dT);

        // DRAWING STARTS HERE

        Uint32 t = SDL_GetTicks();
        realTimeEngine.render();
        printf("t:%dms\n", SDL_GetTicks() - t);
        bufferToBitmap24bpp(buf, buffer, 1);

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
