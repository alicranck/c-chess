//
// Created by user on 3/19/2018.
//

#include "SPGUISaveLoadScreen.h"


SDL_Texture* slots[NUM_SLOTS] ;


SP_GUI_MESSAGE drawSaveLoadWindow(SPChessGame* game, bool save){

    SP_GUI_MESSAGE ret = NONE ;
    Widget** buttons ;
    int currentSlot = 0 ;

    Screen* screen = createScreen(450, 600, "bmp/save/saveBg.bmp", "save/load") ;
    if (screen==NULL)
        return ERROR ;

    if (createSlotTextures(screen->rend, save)!=NONE){
        printf("ERROR: unable to create save/load slots: %s\n", SDL_GetError());
        destroyScreen(screen) ;
        return ERROR ;
    }

    // Create buttons
    buttons = createSaveLoadButtons(screen->rend, save) ;
    if (buttons==NULL){
        destroySlotTextures() ;
        destroyScreen(screen) ;
        printf("ERROR: unable to create save/load buttons: %s\n", SDL_GetError());
        return ERROR ;
    }

    // Draw buttons
    for (int i=0; i<SAVE_NUM_BUTTONS; i++){
        buttons[i]->draw(buttons[i], screen->rend, NULL) ;
    }

    // Event loop
    while(1){
        drawSlot(screen->rend, currentSlot) ;
        SDL_RenderPresent(screen->rend);
        SDL_Event e ;
        SDL_WaitEvent(&e) ;
        if (e.type==SDL_QUIT||e.key.keysym.sym==SDLK_ESCAPE){
            ret = QUIT ;
            break ;
        }
        for (int i=0; i<SAVE_NUM_BUTTONS; i++){
            ret = buttons[i]->handleEvent(buttons[i], &e) ;
            if (ret==UP){
                currentSlot = (currentSlot-1)%NUM_SLOTS ;
                if (currentSlot<0)
                    currentSlot = 5+currentSlot ;
            }
            if (ret==DOWN)
                currentSlot = (currentSlot+1)%NUM_SLOTS ;
            if (ret==LOAD_GAME)
                ret = loadGUIGame(game, currentSlot) ;
            if (ret==SAVE_GAME)
                ret = saveGUIGame(game, currentSlot) ;
            if (ret==QUIT||ret==ERROR||ret==BACK||ret==RELOAD_GAME)
                break ;
        }
        if (ret==QUIT||ret==ERROR||ret==BACK||ret==RELOAD_GAME)
            break ;
    }

    destroyButtons(buttons, SAVE_NUM_BUTTONS) ;
    destroySlotTextures() ;
    destroyScreen(screen) ;

    return ret ;
}


/**
 *  Create Save/Load screen buttons
 * @param rend an SDL_Renderer for Save/Load window
 * @param save - a flag to indicate weather it is a save or load screen. buttons are generated accordingly
 * @return an array of SAVE_NUM_BUTTONS Widgets containing the buttons. NULL on allocation error
 */
Widget** createSaveLoadButtons(SDL_Renderer* rend, bool save){

    Widget** buttons = (Widget**)malloc(sizeof(Widget*)*SAVE_NUM_BUTTONS) ;
    if (buttons==NULL)
        return NULL ;

    SDL_Rect* upRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (upRect==NULL)
        return NULL ;
    upRect->x = ARROW_X ;
    upRect->y = ARROW_Y  ;
    upRect->w = ARROW_BUTTON_WIDTH ;
    upRect->h = ARROW_BUTTON_HEIGHT ;
    buttons[0] = createButton(rend, "bmp/save/up.bmp","bmp/save/upHL.bmp",
                              "bmp/save/upP.bmp", upRect, &upAction) ;

    SDL_Rect* downRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (downRect==NULL)
        return NULL ;
    downRect->x = ARROW_X + ARROW_DIFF ;
    downRect->y = ARROW_Y ;
    downRect->w = ARROW_BUTTON_WIDTH ;
    downRect->h = ARROW_BUTTON_HEIGHT ;
    buttons[1] = createButton(rend, "bmp/save/down.bmp","bmp/save/downHL.bmp",
                              "bmp/save/downP.bmp", downRect, &downAction) ;

    SDL_Rect* backRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (backRect==NULL)
        return NULL ;
    backRect->x = 0 ;
    backRect->y = 560 ;
    backRect->w = SAVE_BUTTON_WIDTH ;
    backRect->h = SAVE_BUTTON_HEIGHT ;
    buttons[2] = createButton(rend, "bmp/save/back.bmp","bmp/save/backHL.bmp",
                              "bmp/save/backP.bmp", backRect, &backAction) ;

    SDL_Rect* startRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (startRect==NULL)
        return NULL ;
    startRect->x = 300;
    startRect->y = 560 ;
    startRect->w = SAVE_BUTTON_WIDTH ;
    startRect->h = SAVE_BUTTON_HEIGHT ;
    if (save)
        buttons[3] = createButton(rend, "bmp/save/save.bmp","bmp/save/saveHL.bmp",
                               "bmp/save/saveP.bmp", startRect, &saveAction) ;
    else
        buttons[3] = createButton(rend, "bmp/save/load.bmp","bmp/save/loadHL.bmp",
                                   "bmp/save/loadP.bmp", startRect, &loadAction) ;

    // Check that all buttons were created succesfuly
    for (int i=0;i<SAVE_NUM_BUTTONS;i++){
        if (buttons[i]==NULL)
            return NULL ;
    }

    return buttons ;
}


/**
 * creates textures for the different possible save slots. in case of load screen,
 * unused slots will be disabled (greyed out)
 * @param rend an SDL_Renderer for Save/Load window
 * @param save a flag to indicate weather it is a save or load screen. slots are generated accordingly
 * @return SP_GUI_MESSAGE NONE on success, ERROR on SDL error.
 */
SP_GUI_MESSAGE createSlotTextures(SDL_Renderer* rend, bool save){

    for (int i=1; i<NUM_SLOTS+1;i++) {
        char imagePath[64];
        sprintf(imagePath, "bmp/save/slot%d.bmp", i);

        SDL_Surface *surface = SDL_LoadBMP(imagePath);
        if (surface == NULL)
            return ERROR;

        SDL_Texture *texture = SDL_CreateTextureFromSurface(rend, surface);
        if (texture == NULL) {
            SDL_FreeSurface(surface);
            return ERROR;
        }
        SDL_FreeSurface(surface);

        char filePath[64];
        sprintf(filePath, "saves/slot%d", i);

        if (!save && access(filePath, F_OK) == -1) {
            SDL_SetTextureColorMod(texture, 50, 50, 50) ;
        }
        slots[i-1] = texture ;
    }
    return NONE ;
}


/**
 * loads a game state from a file, uses 'spChessLoad' from 'SPCHESSGame.c'
 * @param game a pointer to a game instance into which to load the state. assumed not NULL.
 * @param currentSlot which slot to load from.
 * @return SP_GUI_MESSAGE NONE if slot is disabled, ERROR on 'fread()' fail, RELOAD_GAME on success.
 */
SP_GUI_MESSAGE loadGUIGame(SPChessGame* game, int currentSlot){
    char filePath[64];
    SP_CHESS_GAME_MESSAGE ret ;
    sprintf(filePath, "saves/slot%d", currentSlot+1);

    if (access(filePath, F_OK) == -1) {
        return NONE ;
    }

    ret = spChessLoad(game, filePath) ;
    if (ret==SP_CHESS_GAME_STANDART_ERROR){
        printf("ERROR: could not open save file %s, 'fread()' failed.\n", filePath) ;
        return ERROR ;
    }
    if (ret==SP_CHESS_GAME_ILLEGAL_PATH){
        printf("ERROR: could not open save file %s, Illegal path.\n", filePath) ;
        return ERROR ;
    }

    return RELOAD_GAME ;
}


/**
 * saves a game state to a file, uses 'spChessSave' from 'SPCHESSGame.c'
 * @param game a pointer to a game instance to save. assumed not NULL.
 * @param currentSlot which slot to save to.
 * @return SP_GUI_MESSAGE ERROR on 'fwrite()' fail, BACK on success.
 */
SP_GUI_MESSAGE saveGUIGame(SPChessGame* game, int currentSlot){
    char filePath[64];
    sprintf(filePath, "saves/slot%d", currentSlot+1);

    if (spChessSave(game, filePath)==SP_CHESS_GAME_STANDART_ERROR){
        printf("ERROR: could not open save file %s, 'fwrite()' failed.\n", filePath) ;
        return ERROR ;
    }
    if (spChessSave(game, filePath)==SP_CHESS_GAME_ILLEGAL_PATH){
        printf("ERROR: could not open save file %s, Illegal path.\n", filePath) ;
        return ERROR ;
    }
    return BACK ;
}


/**
 * auxiliary function to destroy slot graphics
 */
void destroySlotTextures() {
    for (int i=0;i<NUM_SLOTS;i++){
        SDL_DestroyTexture(slots[i]) ;
    }
}


/**
 * auxiliary function to draw slot graphics
 */
void drawSlot(SDL_Renderer* rend, int currentSlot){
    SDL_Rect location = {.x = SLOT_X, .y = SLOT_Y, .h = SLOT_HEIGHT, .w = SLOT_WIDTH};
    SDL_RenderCopy(rend, slots[currentSlot], NULL, &location) ;
}


SP_GUI_MESSAGE upAction(){
    return UP ;
}

SP_GUI_MESSAGE downAction(){
    return DOWN ;
}


