#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif

#include "parser/SceneParser.h"
#include "core/model/Scene.h"
#include "core/realtime/Buffer.h"
#include "core/realtime/Rasterizer.h"
#include "core/common/Pool.h"
#include "core/common/SceneToPool.h"
#include "sdl/wrapper.h"
#include <SDL/SDL.h>

int main ( int argc, char** argv )
{
    // initialize SDL video
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    SceneParser parser;
    Scene scene;
    Buffer buf(640, 480);
    SceneToPool sceneToPool;
    Pool pool(16384, 8192, 16);
    if(!parser.readScene("media/","test.scene", scene)) {
        printf("readScene failed!\n");
        return 2;
    }
    scene.print();

    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    // create a new window
    SDL_Surface* screen = SDL_SetVideoMode(640, 480, 24,
                                           SDL_HWACCEL|SDL_HWSURFACE|SDL_DOUBLEBUF);
    if ( !screen )
    {
        printf("Unable to set 640x480 video: %s\n", SDL_GetError());
        return 4;
    }

    SDL_Surface * buffer = SDL_CreateRGBSurface(SDL_HWSURFACE|SDL_HWSURFACE, 640, 480, 24, 0,0,0,0);

    // program main loop
    bool done = false;
    while (!done)
    {
        // message processing loop
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
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

        // DRAWING STARTS HERE

        printf("%s:%d\n", __FILE__, __LINE__);
        pool.reset();
        printf("%s:%d\n", __FILE__, __LINE__);
        sceneToPool.run(scene, pool);
        printf("%s:%d\n", __FILE__, __LINE__);
        for(positive i = 0; i < pool.currentFacesCount; i++) {
            vec3 A = &pool.vertexPool[pool.facePool[i*3+0]*VEC4_SCALARS_COUNT];
            vec3 B = &pool.vertexPool[pool.facePool[i*3+1]*VEC4_SCALARS_COUNT];
            vec3 C = &pool.vertexPool[pool.facePool[i*3+2]*VEC4_SCALARS_COUNT];
            real fakeColor[3] = {1.0, 1.0, 1.0};

            triangle(buf, A,B,C, fakeColor, fakeColor, fakeColor);
        }
        printf("%s:%d\n", __FILE__, __LINE__);
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
