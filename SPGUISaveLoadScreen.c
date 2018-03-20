//
// Created by user on 3/19/2018.
//

#include "SPGUISaveLoadScreen.h"


SDL_Texture* slots[NUM_SLOTS] ;


SP_GUI_MESSAGE drawSaveLoadWindow(SPChessGame* game, bool save){

    SP_GUI_MESSAGE ret = NONE ;
    Widget** buttons ;
    int currentSlot = 0 ;

    // create main SDL window
    SDL_Window *window = SDL_CreateWindow(
            "SPChessSettings",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            450,
            600,
            SDL_WINDOW_OPENGL);

    // make sure window was created successfully
    if (window == NULL) {
        printf("ERROR: unable to create window: %s\n", SDL_GetError());
        return ERROR;
    }

    // create a renderer for the window
    SDL_Renderer *rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (rend == NULL) {
        printf("ERROR: unable to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        return ERROR;
    }

    if (createSlotTextures(rend, save)!=NONE){
        printf("ERROR: unable to create slots: %s\n", SDL_GetError());
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        return ERROR ;
    }

    // set background
    SDL_Surface* surface = SDL_LoadBMP("bmp/save/saveBg.bmp") ;
    if (surface==NULL){
        printf("ERROR: unable to load image: %s\n", SDL_GetError());
        destroySlotTextures() ;
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        return ERROR ;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(rend, surface) ;
    if (texture==NULL){
        destroySlotTextures() ;
        SDL_FreeSurface(surface) ;
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        printf("ERROR: unable to create texture from image: %s\n", SDL_GetError());
        return ERROR ;
    }
    SDL_FreeSurface(surface) ;
    SDL_RenderCopy(rend, texture, NULL, NULL);

    // Create buttons
    buttons = createSaveLoadButtons(rend, save) ;
    if (buttons==NULL){
        destroySlotTextures() ;
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        printf("ERROR: unable to create buttons: %s\n", SDL_GetError());
        return ERROR ;
    }

    // Draw buttons
    for (int i=0; i<SAVE_NUM_BUTTONS; i++){
        buttons[i]->draw(buttons[i], rend) ;
    }

    // Event loop
    while(1){
        drawSlot(rend, currentSlot) ;
        SDL_RenderPresent(rend);
        SDL_Event e ;
        Button* button ;
        SDL_WaitEvent(&e) ;
        if (e.type==SDL_QUIT||e.key.keysym.sym==SDLK_ESCAPE){
            ret = QUIT ;
            break ;
        }
        for (int i=0; i<SAVE_NUM_BUTTONS; i++){
            ret = buttons[i]->handleEvent(buttons[i], &e) ;
            if (ret==UP)
                currentSlot = (currentSlot-1)%NUM_SLOTS ;
            if (ret==DOWN)
                currentSlot = (currentSlot+1)%NUM_SLOTS ;
            if (ret==LOAD_GAME)
                ret = loadGUIGame(game, currentSlot) ;
            if (ret==SAVE_GAME)
                ret =  saveGUIGame(game, currentSlot) ;
            if (ret==QUIT||ret==ERROR||ret==BACK||ret==RELOAD_GAME)
                break ;
        }
        if (ret==QUIT||ret==ERROR||ret==BACK||ret==RELOAD_GAME)
            break ;
    }

    // Destroy buttons
    for (int i=0; i<SAVE_NUM_BUTTONS; i++){
        buttons[i]->destroy(buttons[i]) ;
        free(buttons[i]) ;
    }
    destroySlotTextures() ;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);

    return ret ;
}


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


void destroySlotTextures() {
    for (int i=0;i<NUM_SLOTS;i++){
        SDL_DestroyTexture(slots[i]) ;
    }
}


void drawSlot(SDL_Renderer* rend, int currentSlot){
    SDL_Rect location = {.x = SLOT_X, .y = SLOT_Y, .h = SLOT_HEIGHT, .w = SLOT_WIDTH};
    SDL_RenderCopy(rend, slots[currentSlot], NULL, &location) ;
}


SP_GUI_MESSAGE loadGUIGame(SPChessGame* game, int currentSlot){
    char filePath[64];
    sprintf(filePath, "saves/slot%d", currentSlot+1);

    if (spChessLoad(game, filePath)!=SP_CHESS_GAME_SUCCESS)
        return ERROR ;

    return RELOAD_GAME ;
}


SP_GUI_MESSAGE saveGUIGame(SPChessGame* game, int currentSlot){
    char filePath[64];
    sprintf(filePath, "saves/slot%d", currentSlot+1);

    if (spChessSave(game, filePath)!=SP_CHESS_GAME_SUCCESS)
        return ERROR ;

    return BACK ;
}


SP_GUI_MESSAGE upAction(){
    return UP ;
}

SP_GUI_MESSAGE downAction(){
    return DOWN ;
}


