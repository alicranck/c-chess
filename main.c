//
// Created by user on 3/8/2018.
//
#include "SPConsoleMain.h"
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

        // create an SDL window
        SDL_Window *window = SDL_CreateWindow(
                "Title",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                600,
                600,
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

        SDL_Rect rect = {.x = 250, .y = 250, .w = 100, .h = 100};

        // clear window to color red (r,g,b,a)
        SDL_SetRenderDrawColor(rend, 255, 0, 0, 0);
        SDL_RenderClear(rend);

        // draw a blue rectangle
        SDL_SetRenderDrawColor(rend, 0, 0, 255, 0);
        SDL_RenderFillRect(rend, &rect);

        // present changes to user
        SDL_RenderPresent(rend);

        SDL_Delay(10000);

        // free everything and finish
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    return ret ;
}
