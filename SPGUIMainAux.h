//
// Created by user on 3/9/2018.
//

#ifndef SPFINALPROJECT_SPGUIMAINAUX_H
#define SPFINALPROJECT_SPGUIMAINAUX_H

#endif //SPFINALPROJECT_SPGUIMAINAUX_H

#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include "SPGUITypes.h"
#include "SPCHESSGame.h"

#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 40
#define BUTTON_VERTICAL_DIFF 60
#define START_SCREEN_TOP_BUTTON_Y 330
#define START_SCREEN_BUTTON_X 125








SP_GUI_MESSAGE drawStartWindow() ;

SP_GUI_MESSAGE newGameAction() ;

SP_GUI_MESSAGE loadAction() ;

SP_GUI_MESSAGE quitAction() ;

int drawMainWindow(SDL_Window* window, SDL_Renderer* rend);