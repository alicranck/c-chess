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
#define SETTINGS_VERTICAL_DIFF 60

#define SETTINGS_NUM_BUTTONS 11




SP_GUI_MESSAGE drawSettingsWindow(int* settings) ;

Widget** createSettingsButtons(SDL_Renderer* rend) ;

void changeSettings(int* settings, SP_GUI_MESSAGE msg) ;

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
