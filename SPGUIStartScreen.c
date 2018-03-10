//
// Created by user on 3/9/2018.
//

#include "SPGUIStartScreen.h"


SP_GUI_MESSAGE drawStartWindow(){

    SP_GUI_MESSAGE ret = NONE ;

    // create main SDL window
    SDL_Window *window = SDL_CreateWindow(
            "SPChess",
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

    SDL_Rect newGameRect = { .x = START_SCREEN_BUTTON_X, .y = START_SCREEN_TOP_BUTTON_Y,
            .w = BUTTON_WIDTH, .h = BUTTON_HEIGHT };
    Widget* newGameButton = createButton(rend, "bmp/start/newGame.bmp", "bmp/start/newGameHL.bmp",
                                         "bmp/start/newGameP.bmp", &newGameRect, &newGameAction) ;
    if (newGameButton==NULL)
        return ERROR ;
    newGameButton->draw(newGameButton, rend);

    SDL_Rect loadRect = { .x = START_SCREEN_BUTTON_X, .y = START_SCREEN_TOP_BUTTON_Y + BUTTON_VERTICAL_DIFF,
            .w = BUTTON_WIDTH, .h = BUTTON_HEIGHT };
    Widget* loadButton = createButton(rend, "bmp/start/loadGame.bmp","bmp/start/loadGameHL.bmp",
                                      "bmp/start/loadGameP.bmp", &loadRect, &loadAction) ;
    if (loadButton==NULL)
        return ERROR ;
    loadButton->draw(loadButton, rend);

    SDL_Rect quitRect = { .x = START_SCREEN_BUTTON_X, .y = START_SCREEN_TOP_BUTTON_Y + 2*BUTTON_VERTICAL_DIFF,
            .w = BUTTON_WIDTH, .h = BUTTON_HEIGHT };
    Widget* quitButton = createButton(rend, "bmp/start/quit.bmp", "bmp/start/quitHL.bmp",
                                      "bmp/start/quitP.bmp", &quitRect, &quitAction) ;
    if (quitButton==NULL)
        return ERROR ;
    quitButton->draw(quitButton, rend);

    while(1){
        SDL_RenderPresent(rend);
        SDL_Event e ;
        SDL_WaitEvent(&e) ;
        if (e.type==SDL_QUIT||e.key.keysym.sym==SDLK_ESCAPE){
            break ;
        }
        ret = (*newGameButton->handleEvent)(newGameButton, &e) ;
        if (ret!=NONE)
            break;
        ret = (*loadButton->handleEvent)(loadButton, &e) ;
        if (ret!=NONE)
            break;
        ret = (*quitButton->handleEvent)(quitButton, &e) ;
        if (ret!=NONE)
            break;
    }
    newGameButton->destroy(newGameButton) ;
    free(newGameButton) ;
    loadButton->destroy(loadButton) ;
    free(loadButton);
    quitButton->destroy(quitButton) ;
    free(quitButton) ;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);
    return ret ;
}



SP_GUI_MESSAGE newGameAction(){
    return START_NEW_GAME ;
}

SP_GUI_MESSAGE loadAction(){
    return LOAD_GAME ;
}

SP_GUI_MESSAGE quitAction(){
    return QUIT ;
}