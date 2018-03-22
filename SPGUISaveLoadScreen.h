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


/**
 *  Create Save/Load screen buttons
 * @param rend an SDL_Renderer for Save/Load window
 * @param save - a flag to indicate weather it is a save or load screen. buttons are generated accordingly
 * @return an array of SAVE_NUM_BUTTONS Widgets containing the buttons. NULL on allocation error
 */
Widget** createSaveLoadButtons(SDL_Renderer* rend, bool save) ;


/**
 * creates textures for the different possible save slots. in case of load screen,
 * unused slots will be disabled (greyed out)
 * @param rend an SDL_Renderer for Save/Load window
 * @param save a flag to indicate weather it is a save or load screen. slots are generated accordingly
 * @return SP_GUI_MESSAGE NONE on success, ERROR on SDL error.
 */
SP_GUI_MESSAGE createSlotTextures(SDL_Renderer* rend, bool save);


/**
 * loads a game state from a file, uses 'spChessLoad' from 'SPCHESSGame.c'
 * @param game a pointer to a game instance into which to load the state. assumed not NULL.
 * @param currentSlot which slot to load from.
 * @return SP_GUI_MESSAGE NONE if slot is disabled, ERROR on 'fread()' fail, RELOAD_GAME on success.
 */
SP_GUI_MESSAGE loadGUIGame(SPChessGame* game, int currentSlot) ;


/**
 * saves a game state to a file, uses 'spChessSave' from 'SPCHESSGame.c'
 * @param game a pointer to a game instance to save. assumed not NULL.
 * @param currentSlot which slot to save to.
 * @return SP_GUI_MESSAGE ERROR on 'fwrite()' fail, BACK on success.
 */
SP_GUI_MESSAGE saveGUIGame(SPChessGame* game, int currentSlot) ;


/**
 * auxiliary function to draw slot graphics
 */
void drawSlot(SDL_Renderer* rend, int currentSlot) ;


/**
 * auxiliary function to destroy slot graphics
 */
void destroySlotTextures();

void destroyButtons(Widget** buttons, int n);

SP_GUI_MESSAGE upAction();

SP_GUI_MESSAGE downAction();

SP_GUI_MESSAGE saveAction();

SP_GUI_MESSAGE loadAction();

SP_GUI_MESSAGE backAction();


#endif //SPFINALPROJECT_SPGUISAVELOADSCREEN_H
