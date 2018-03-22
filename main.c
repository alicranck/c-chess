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


/**
 * an auxiliary method to main used to control game flow
 * @param msg an SP_GUI_MESSAGE to indicate the next screen to draw
 * @return a message that represents the next screen, program ends only in case msg==QUIT 
 */
SP_GUI_MESSAGE launchScreen(SP_GUI_MESSAGE msg){

    if(msg==START_NEW_GAME)
        return drawSettingsWindow(settings) ;

    if(msg==MAIN_MENU||msg==BACK||msg==ERROR)
        return drawStartWindow() ;

    if (msg==START_GAME||msg==RESTART_GAME) {
        game = spChessCreate(settings);
        return drawGameWindow(game);
    }

    if(msg==LOAD_GAME) {
        game = spChessCreate(settings);
        return drawSaveLoadWindow(game, false);
    }

    if(msg==RELOAD_GAME)
        return drawGameWindow(game) ;

    return ERROR ;
}