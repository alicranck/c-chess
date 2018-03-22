//
// Created by user on 3/9/2018.
//

#ifndef SPFINALPROJECT_SPGUISETTINGSSCREEN_H
#define SPFINALPROJECT_SPGUISETTINGSSCREEN_H

#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include "SPGUITypes.h"
#include "SPCHESSGame.h"


#define SETTINGS_TOP_Y 210
#define SETTINGS_X 60
#define SETTINGS_HORIZONTAL_DIFF 180
#define SETTINGS_BUTTON_WIDTH 150
#define SETTINGS_BUTTON_HEIGHT 40

#define SETTINGS_NUM_BUTTONS 11




SP_GUI_MESSAGE drawSettingsWindow(int* settings) ;


/**
 *  Create Settings screen buttons
 * @param rend an SDL_Renderer for settings window
 * @return an array of SETTINGS_NUM_BUTTONS Widgets containing the buttons. NULL on allocation error
 */
Widget** createSettingsButtons(SDL_Renderer* rend) ;


/**
 * a function to change the settings based on the pressed buttons
 * @param settings a pointer to an int[3] array with the game settings
 * @param msg an SP_GUI_MESSAGE containing info about setting to change
 */
void changeSettings(int* settings, SP_GUI_MESSAGE msg) ;



void destroyButtons(Widget** buttons, int n);

SP_GUI_MESSAGE onePlayerAction();

SP_GUI_MESSAGE twoPlayerAction();

SP_GUI_MESSAGE whiteAction();

SP_GUI_MESSAGE blackAction();

SP_GUI_MESSAGE amateurAction();

SP_GUI_MESSAGE easyAction();

SP_GUI_MESSAGE moderateAction();

SP_GUI_MESSAGE hardAction();

SP_GUI_MESSAGE expertAction();

SP_GUI_MESSAGE backAction();

SP_GUI_MESSAGE startAction();


#endif //SPFINALPROJECT_SPGUISETTINGSSCREEN_H
