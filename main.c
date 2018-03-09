//
// Created by user on 3/8/2018.
//
#include "SPConsoleMain.h"
#include "SPGUIMainAux.h"
#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>

int main(int argc, char** argv){

    int ret = 0 ;

    // If console mode is chosen, run console mode and return
    if (argc==1||!strcmp(argv[1], "-c")){
        ret = consoleMain();
        return ret ;
    }

    // Else, if GUI mode is selected, run from here
    if (!strcmp(argv[1], "-g")) {
        // initialize SDL2 for video
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
            return 1;
        }

        // create main SDL window
        SDL_Window *window = SDL_CreateWindow(
                "SPChess",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                1024,
                768,
                SDL_WINDOW_OPENGL);

        // make sure window was created successfully
        if (window == NULL) {
            printf("ERROR: unable to create window: %s\n", SDL_GetError());
            SDL_Quit();
            return 0;
        }

        // create a renderer for the window
        SDL_Renderer *rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (rend == NULL) {
            printf("ERROR: unable to create renderer: %s\n", SDL_GetError());
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 0;
        }

        // Draw main window
        drawMainWindow(window, rend) ;

        SDL_Delay(1000000000);

        // free everything and finish
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    return ret ;
}
