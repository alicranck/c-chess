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

        // Draw start window
        drawStartWindow() ;

        // free everything and finish
        SDL_Quit();
    }
    return ret ;
}
