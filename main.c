//
// Created by user on 3/8/2018.
//
#include "SPConsoleMain.h"
#include "SPGUIStartScreen.h"
#include "SPGUISettingsScreen.h"
#include "SPGUIGameScreen.h"

int settings[3] = {1,2,1};
SPChessGame* game = NULL ;


int main(int argc, char** argv){

    SP_GUI_MESSAGE ret ;
    // If console mode is chosen, run console mode and return
    if (argc==1||!strcmp(argv[1], "-c")){
        return consoleMain();
    }

    // Else, if GUI mode is selected, run from here
    if (!strcmp(argv[1], "-g")) {
        // initialize SDL2 for video
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
            return 1;
        }

        // Draw start window
        ret = drawStartWindow() ;
        while (ret!=QUIT){
            ret = launchScreen(ret) ;
        }
        // free everything and finish
        SDL_Quit();
    }
    return 0 ;
}


SP_GUI_MESSAGE launchScreen(SP_GUI_MESSAGE screen){
    switch(screen){
        case START_NEW_GAME:
            return drawSettingsWindow(settings) ;
        case MAIN_MENU:
            return drawStartWindow() ;
        case BACK:
            return drawStartWindow() ;
        case START_GAME:
            game = spChessCreate(settings) ;
            return drawGameWindow(game) ;
        case RESTART_GAME :
            game = spChessCreate(settings) ;
            return drawGameWindow(game) ;
        case LOAD_GAME:
            game = spChessCreate(settings) ;
            return drawSaveLoadWindow(game, false) ;
        case RELOAD_GAME:
            return drawGameWindow(game) ;
        case ERROR:
            return drawStartWindow() ;

    }
}


void displayError(){

}