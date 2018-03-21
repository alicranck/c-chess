//
// Created by user on 3/9/2018.
//

#include "SPGUIStartScreen.h"


SP_GUI_MESSAGE drawStartWindow(){

    SP_GUI_MESSAGE ret = NONE ;
    Widget** buttons ;

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
        printf("ERROR: unable to create main window: %s\n", SDL_GetError());
        return ERROR;
    }

    // create a renderer for the window
    SDL_Renderer *rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (rend == NULL) {
        printf("ERROR: unable to create main window renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        return ERROR;
    }

    // set background
    SDL_Surface* surface = SDL_LoadBMP("bmp/start/startBg.bmp") ;
    if (surface==NULL){
        printf("ERROR: unable to load main window background image: %s\n", SDL_GetError());
        SDL_DestroyRenderer(rend) ;
        SDL_DestroyWindow(window);
        return ERROR ;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(rend, surface) ;
    if (texture==NULL){
        SDL_FreeSurface(surface) ;
        SDL_DestroyRenderer(rend) ;
        SDL_DestroyWindow(window);
        printf("ERROR: unable to create main window texture from image: %s\n", SDL_GetError());
        return ERROR ;
    }
    SDL_FreeSurface(surface) ;
    SDL_RenderCopy(rend, texture, NULL, NULL);

    // Create buttons
    buttons = createStartButtons(rend) ;
    if (buttons==NULL){
        SDL_DestroyTexture(texture) ;
        SDL_DestroyRenderer(rend) ;
        SDL_DestroyWindow(window);
        printf("ERROR: unable to create main window buttons: %s\n", SDL_GetError());
        return ERROR ;
    }

    // Draw buttons
    for (int i=0;i<START_NUM_BUTTONS;i++){
        buttons[i]->draw(buttons[i], rend) ;
    }

    // Event loop
    while(1){
        SDL_RenderPresent(rend);
        SDL_Event e ;
        SDL_WaitEvent(&e) ;
        if (e.type==SDL_QUIT||e.key.keysym.sym==SDLK_ESCAPE){
            ret = QUIT ;
            break ;
        }
        for (int i=0;i<START_NUM_BUTTONS;i++){
            ret = buttons[i]->handleEvent(buttons[i], &e) ;
            if (ret!=NONE)
                break ;
        }
        if (ret!=NONE)
            break ;
    }

    destroyButtons(buttons, START_NUM_BUTTONS) ;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);

    return ret ;
}


/**
 *  Create Main Menu buttons
 * @param rend an SDL_Renderer for main window
 * @return an array of START_NUM_BUTTONS Widgets containing the buttons
 */
Widget** createStartButtons(SDL_Renderer* rend){
    Widget** buttons = (Widget**)malloc(sizeof(Widget*)*START_NUM_BUTTONS) ;
    if (buttons==NULL)
        return NULL ;

    SDL_Rect* newGameRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (newGameRect==NULL)
        return NULL ;
    newGameRect->x = START_SCREEN_BUTTON_X ;
    newGameRect->y = START_SCREEN_TOP_BUTTON_Y ;
    newGameRect->w = BUTTON_WIDTH ;
    newGameRect->h = BUTTON_HEIGHT;
    buttons[0] = createButton(rend, "bmp/start/newGame.bmp", "bmp/start/newGameHL.bmp",
                                         "bmp/start/newGameP.bmp", newGameRect, &newGameAction) ;

    SDL_Rect* loadRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (loadRect==NULL)
        return NULL ;
    loadRect->x = START_SCREEN_BUTTON_X ;
    loadRect->y = START_SCREEN_TOP_BUTTON_Y + BUTTON_VERTICAL_DIFF ;
    loadRect->w = BUTTON_WIDTH ;
    loadRect->h = BUTTON_HEIGHT;
    buttons[1] = createButton(rend, "bmp/start/loadGame.bmp","bmp/start/loadGameHL.bmp",
                                      "bmp/start/loadGameP.bmp", loadRect, &loadAction) ;

    SDL_Rect* quitRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (quitRect==NULL)
        return NULL ;
    quitRect->x = START_SCREEN_BUTTON_X ;
    quitRect->y = START_SCREEN_TOP_BUTTON_Y + 2*BUTTON_VERTICAL_DIFF ;
    quitRect->w = BUTTON_WIDTH ;
    quitRect->h = BUTTON_HEIGHT;
    buttons[2] = createButton(rend, "bmp/start/quit.bmp", "bmp/start/quitHL.bmp",
                                      "bmp/start/quitP.bmp", quitRect, &quitAction) ;

    // Check that all buttons were created successfully
    for (int i=0;i<START_NUM_BUTTONS;i++){
        if (buttons[i]==NULL)
            return NULL ;
    }

    return buttons ;
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