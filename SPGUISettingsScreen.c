//
// Created by user on 3/9/2018.
//

#include "SPGUISettingsScreen.h"



SP_GUI_MESSAGE drawSettingsWindow(int* settings){

    SP_GUI_MESSAGE ret = NONE ;
    Widget** buttons ;

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
        SDL_Quit();
        return ERROR;
    }

    // create a renderer for the window
    SDL_Renderer *rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (rend == NULL) {
        printf("ERROR: unable to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return ERROR;
    }

    // set background
    SDL_Surface* surface = SDL_LoadBMP("bmp/start/startBg.bmp") ;
    if (surface==NULL){
        printf("ERROR: unable to load image: %s\n", SDL_GetError());
        return ERROR ;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(rend, surface) ;
    if (texture==NULL){
        SDL_FreeSurface(surface) ;
        printf("ERROR: unable to create texture from image: %s\n", SDL_GetError());
        return ERROR ;
    }
    SDL_FreeSurface(surface) ;
    SDL_RenderCopy(rend, texture, NULL, NULL);

    // Create buttons
    buttons = createSettingsButtons(rend) ;

    // Draw buttons
    for (int i=0; i<SETTINGS_NUM_BUTTONS; i++){
        if (buttons[i]==NULL) {
            printf("ERROR\n") ;
            return ERROR;
        }
        buttons[i]->draw(buttons[i], rend) ;
    }

    // Event loop
    while(1){
        SDL_RenderPresent(rend);
        SDL_Event e ;
        Button* button ;
        SDL_WaitEvent(&e) ;
        if (e.type==SDL_QUIT||e.key.keysym.sym==SDLK_ESCAPE){
            break ;
        }
        for (int i=0; i<SETTINGS_NUM_BUTTONS; i++){
            // Disable settings in 2-player mode
            if (settings[0]==2&&i!=0){
                button = (Button*)buttons[i]->data ;
                button->pressed = true ;
                button->highlighted = true ;
            }
            ret = buttons[i]->handleEvent(buttons[i], &e) ;
            if (ret==QUIT||ret==ERROR||ret==MAIN_MENU||ret==START_GAME)
                break ;
            if (ret!=NONE)
                changeSettings(settings, ret) ;
        }
        if (ret==QUIT||ret==ERROR||ret==MAIN_MENU||ret==START_GAME)
            break ;
    }

    // Destroy buttons
    for (int i=0; i<SETTINGS_NUM_BUTTONS; i++){
        buttons[i]->destroy(buttons[i]) ;
        free(buttons[i]) ;
    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);

    return ret ;
}




Widget** createSettingsButtons(SDL_Renderer* rend){

    Widget** buttons = (Widget**)malloc(sizeof(Widget*)*SETTINGS_NUM_BUTTONS) ;
    
    // Game mode buttons
    SDL_Rect* onePlayerRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (onePlayerRect==NULL)
        return NULL ;
    onePlayerRect->x = SETTINGS_X ;
    onePlayerRect->y = SETTINGS_TOP_Y ;
    onePlayerRect->w = SETTINGS_BUTTON_WIDTH ;
    onePlayerRect->h = SETTINGS_BUTTON_HEIGHT ;
    buttons[0] = createButton(rend, "bmp/settings/onePlayer.bmp", "bmp/settings/onePlayerHL.bmp",
                                         "bmp/settings/onePlayerP.bmp", onePlayerRect, &onePlayerAction) ;

    SDL_Rect* twoPlayerRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (twoPlayerRect==NULL)
        return NULL ;
    twoPlayerRect->x = SETTINGS_X + SETTINGS_HORIZONTAL_DIFF ;
    twoPlayerRect->y = SETTINGS_TOP_Y ;
    twoPlayerRect->w = SETTINGS_BUTTON_WIDTH ;
    twoPlayerRect->h = SETTINGS_BUTTON_HEIGHT ;
    buttons[1] = createButton(rend, "bmp/settings/twoPlayer.bmp","bmp/settings/twoPlayerHL.bmp",
                                      "bmp/settings/twoPlayerP.bmp", twoPlayerRect, &twoPlayerAction) ;

    // Player color buttons
    SDL_Rect* whiteRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (whiteRect==NULL)
        return NULL ;
    whiteRect->x = SETTINGS_X ;
    whiteRect->y = SETTINGS_TOP_Y + 80 ;
    whiteRect->w = SETTINGS_BUTTON_WIDTH ;
    whiteRect->h = SETTINGS_BUTTON_HEIGHT ;
    buttons[2] = createButton(rend, "bmp/settings/white.bmp", "bmp/settings/whiteHL.bmp",
                              "bmp/settings/whiteP.bmp", whiteRect, &whiteAction) ;

    SDL_Rect* blackRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (blackRect==NULL)
        return NULL ;
    blackRect->x = SETTINGS_X + SETTINGS_HORIZONTAL_DIFF;
    blackRect->y = SETTINGS_TOP_Y + 80;
    blackRect->w = SETTINGS_BUTTON_WIDTH ;
    blackRect->h = SETTINGS_BUTTON_HEIGHT ;
    buttons[3] = createButton(rend, "bmp/settings/black.bmp","bmp/settings/blackHL.bmp",
                              "bmp/settings/blackP.bmp", blackRect, &blackAction) ;

    // Level buttons

    SDL_Rect* amateurRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (amateurRect==NULL)
        return NULL ;
    amateurRect->x = SETTINGS_X ;
    amateurRect->y = SETTINGS_TOP_Y + 160;
    amateurRect->w = SETTINGS_BUTTON_WIDTH ;
    amateurRect->h = SETTINGS_BUTTON_HEIGHT ;
    buttons[4] = createButton(rend, "bmp/settings/amateur.bmp", "bmp/settings/amateurHL.bmp",
                              "bmp/settings/amateurP.bmp", amateurRect, &amateurAction) ;

    SDL_Rect* easyRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (easyRect==NULL)
        return NULL ;
    easyRect->x = SETTINGS_X + SETTINGS_HORIZONTAL_DIFF;
    easyRect->y = SETTINGS_TOP_Y + 160;
    easyRect->w = SETTINGS_BUTTON_WIDTH ;
    easyRect->h = SETTINGS_BUTTON_HEIGHT ;
    buttons[5] = createButton(rend, "bmp/settings/easy.bmp","bmp/settings/easyHL.bmp",
                              "bmp/settings/easyP.bmp", easyRect, &easyAction) ;

    SDL_Rect* moderateRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (moderateRect==NULL)
        return NULL ;
    moderateRect->x = SETTINGS_X ;
    moderateRect->y = SETTINGS_TOP_Y + 210 ;
    moderateRect->w = SETTINGS_BUTTON_WIDTH ;
    moderateRect->h = SETTINGS_BUTTON_HEIGHT ;
    buttons[6] = createButton(rend, "bmp/settings/moderate.bmp", "bmp/settings/moderateHL.bmp",
                              "bmp/settings/moderateP.bmp", moderateRect, &moderateAction) ;

    SDL_Rect* hardRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (hardRect==NULL)
        return NULL ;
    hardRect->x = SETTINGS_X + SETTINGS_HORIZONTAL_DIFF ;
    hardRect->y = SETTINGS_TOP_Y + 210 ;
    hardRect->w = SETTINGS_BUTTON_WIDTH ;
    hardRect->h = SETTINGS_BUTTON_HEIGHT ;
    buttons[7] = createButton(rend, "bmp/settings/hard.bmp","bmp/settings/hardHL.bmp",
                              "bmp/settings/hardP.bmp", hardRect, &hardAction) ;

    SDL_Rect* expertRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (expertRect==NULL)
        return NULL ;
    expertRect->x = SETTINGS_X + 90;
    expertRect->y = SETTINGS_TOP_Y + 260 ;
    expertRect->w = SETTINGS_BUTTON_WIDTH ;
    expertRect->h = SETTINGS_BUTTON_HEIGHT ;
    buttons[8] = createButton(rend, "bmp/settings/expert.bmp","bmp/settings/expertHL.bmp",
                              "bmp/settings/expertP.bmp", expertRect, &expertAction) ;

    SDL_Rect* backRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (backRect==NULL)
        return NULL ;
    backRect->x = 0 ;
    backRect->y = 560 ;
    backRect->w = SETTINGS_BUTTON_WIDTH ;
    backRect->h = SETTINGS_BUTTON_HEIGHT ;
    buttons[9] = createButton(rend, "bmp/settings/back.bmp","bmp/settings/backHL.bmp",
                              "bmp/settings/backP.bmp", backRect, &backAction) ;

    SDL_Rect* startRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (startRect==NULL)
        return NULL ;
    startRect->x = 300;
    startRect->y = 560 ;
    startRect->w = SETTINGS_BUTTON_WIDTH ;
    startRect->h = SETTINGS_BUTTON_HEIGHT ;
    buttons[10] = createButton(rend, "bmp/settings/start.bmp","bmp/settings/startHL.bmp",
                              "bmp/settings/startP.bmp", startRect, &startAction) ;
    
    return buttons ;
}



void changeSettings(int* settings, SP_GUI_MESSAGE msg){
    switch (msg){
        case MODE_ONE_PLAYER:
            settings[0] = 1 ;
            break ;
        case MODE_TWO_PLAYER:
            settings[0] = 2;
            break ;
        case COLOR_WHITE:
            settings[2] = 1 ;
            break;
        case COLOR_BLACK:
            settings[2] = 0 ;
            break ;
        case LEVEL_AMATEUR:
            settings[1] = 1 ;
            break ;
        case LEVEL_EASY:
            settings[1] = 2 ;
            break ;
        case LEVEL_MODERATE:
            settings[1] = 3 ;
            break ;
        case LEVEL_HARD:
            settings[1] = 4 ;
            break ;
        case LEVEL_EXPERT:
            settings[1] = 5 ;
            break ;
        default:
            break ;
    }
}

SP_GUI_MESSAGE onePlayerAction(){
    return MODE_ONE_PLAYER ;
}

SP_GUI_MESSAGE twoPlayerAction(){
    return MODE_TWO_PLAYER ;
}

SP_GUI_MESSAGE whiteAction(){
    return COLOR_WHITE ;
}

SP_GUI_MESSAGE blackAction(){
    return COLOR_BLACK ;
}

SP_GUI_MESSAGE amateurAction(){
    return LEVEL_AMATEUR ;
}

SP_GUI_MESSAGE easyAction(){
    return LEVEL_EASY ;
}

SP_GUI_MESSAGE moderateAction(){
    return LEVEL_MODERATE ;
}

SP_GUI_MESSAGE hardAction(){
    return LEVEL_HARD ;
}

SP_GUI_MESSAGE expertAction(){
    return LEVEL_EXPERT ;
}

SP_GUI_MESSAGE backAction(){
    return MAIN_MENU ;
}

SP_GUI_MESSAGE startAction(){
    return START_GAME ;
}