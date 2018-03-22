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

#define START_NUM_BUTTONS 3


/**
 *  Create Main Menu buttons
 * @param rend an SDL_Renderer for main window
 * @return an array of START_NUM_BUTTONS Widgets containing the buttons.  NULL on allocation error
 */
Widget** createStartButtons(SDL_Renderer* rend) ;


/**
 * destroy all buttons in an array
 * @param buttons - the array of buttons (Widget*) to destroy
 * @param n - number of buttons in the array
 */
void destroyButtons(Widget** buttons, int n);








SP_GUI_MESSAGE drawStartWindow() ;

SP_GUI_MESSAGE newGameAction() ;

SP_GUI_MESSAGE loadAction() ;

SP_GUI_MESSAGE quitAction() ;

