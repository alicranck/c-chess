//
// Created by user on 3/19/2018.
//

#ifndef SPFINALPROJECT_SPGUISAVELOADSCREEN_H
#define SPFINALPROJECT_SPGUISAVELOADSCREEN_H

#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include <unistd.h>
#include "SPGUITypes.h"
#include "SPCHESSGame.h"


#define NUM_SLOTS 5
#define SAVE_NUM_BUTTONS 4

#define SLOT_X 125
#define SLOT_Y 250
#define SLOT_HEIGHT 50
#define SLOT_WIDTH 200

#define SAVE_BUTTON_WIDTH 150
#define SAVE_BUTTON_HEIGHT 40

#define ARROW_X 180
#define ARROW_Y 310
#define ARROW_BUTTON_WIDTH 40
#define ARROW_BUTTON_HEIGHT 40
#define ARROW_DIFF 50


SP_GUI_MESSAGE drawSaveLoadWindow(SPChessGame* game, bool save) ;

Widget** createSaveLoadButtons(SDL_Renderer* rend, bool save) ;

SP_GUI_MESSAGE createSlotTextures(SDL_Renderer* rend, bool save);

void drawSlot(SDL_Renderer* rend, int currentSlot) ;

SP_GUI_MESSAGE loadGUIGame(SPChessGame* game, int currentSlot) ;

SP_GUI_MESSAGE saveGUIGame(SPChessGame* game, int currentSlot) ;

void destroySlotTextures();

SP_GUI_MESSAGE upAction();

SP_GUI_MESSAGE downAction();

SP_GUI_MESSAGE saveAction();

SP_GUI_MESSAGE loadAction();

SP_GUI_MESSAGE backAction();


#endif //SPFINALPROJECT_SPGUISAVELOADSCREEN_H
