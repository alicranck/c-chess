//
// Created by user on 3/9/2018.
//

#include "SPGUITypes.h"


//--------------------------------------Button Methods-------------------------------------

/**
 * creates a button widget
 * @param rend
 * @param img - path to button image
 * @param highlightImg - path to highlighted button image - for hover graphics
 * @param pressedImg - path to pressed button image - for pressed graphics
 * @param location - SDL_rect* with the location of the button
 * @param action - a (void)() function which will be called on button click
 * @return a Widget* with the created button on success. NULL on allocation or SDL error.
 */
Widget* createButton(SDL_Renderer* rend, char* img, char* highlightImg, char* pressedImg,
                     SDL_Rect* location, SP_GUI_MESSAGE (*action)(void)){

    Button* button = (Button*)malloc(sizeof(Button)) ;
    if (button==NULL)
        return NULL ;

    SDL_Surface* surface = SDL_LoadBMP(img) ;
    if (surface==NULL){
        printf("ERROR: unable to load image: %s\n", SDL_GetError());
        return NULL ;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(rend, surface) ;
    if (texture==NULL){
        SDL_FreeSurface(surface) ;
        printf("ERROR: unable to create texture from image: %s\n", SDL_GetError());
        return NULL ;
    }
    SDL_FreeSurface(surface) ;

    surface = SDL_LoadBMP(highlightImg) ;
    if (surface==NULL){
        printf("ERROR: unable to load image: %s\n", SDL_GetError());
        return NULL ;
    }
    SDL_Texture* HLtexture = SDL_CreateTextureFromSurface(rend, surface) ;
    if (HLtexture==NULL){
        SDL_FreeSurface(surface) ;
        printf("ERROR: unable to create texture from image: %s\n", SDL_GetError());
        return NULL ;
    }
    SDL_FreeSurface(surface) ;

    surface = SDL_LoadBMP(pressedImg) ;
    if (surface==NULL){
        printf("ERROR: unable to load image: %s\n", SDL_GetError());
        return NULL ;
    }
    SDL_Texture* Ptexture = SDL_CreateTextureFromSurface(rend, surface) ;
    if (Ptexture==NULL){
        SDL_FreeSurface(surface) ;
        printf("ERROR: unable to create texture from image: %s\n", SDL_GetError());
        return NULL ;
    }
    SDL_FreeSurface(surface) ;

    button->rend = rend ;
    button->texture = texture ;
    button->highlightedTexture = HLtexture ;
    button->pressedTexture = Ptexture ;
    button->location = location ;
    button->action = action ;
    button->hover = false ;
    button->pressed = false ;
    button->disabled = false ;

    Widget* widget = (Widget*)malloc(sizeof(Widget)) ;
    if (widget==NULL){
        printf("ERROR: allocation error\n") ;
        return NULL ;
    }

    widget->draw = &drawButton ;
    widget->destroy = &destroyButton ;
    widget->handleEvent = &handleButtonEvent ;
    widget->data = button ;

    return widget ;
}


/**
 * free all memory of a given button
 * @param src
 */
void destroyButton(Widget* src){
    Button* button = (Button*)src->data ;
    SDL_DestroyTexture(button->texture) ;
    SDL_DestroyTexture(button->highlightedTexture) ;
    SDL_DestroyTexture(button->pressedTexture) ;
    free(button->location) ;
    free(button) ;
}


/**
 * handle button press event. activate hover/pressed graphics and call button action
 * if needed.
 * @param src
 * @param e
 * @return SP_GUI_MESSAGE NONE on success, or according to button action if clicked.
 */
SP_GUI_MESSAGE handleButtonEvent(Widget* src, SDL_Event* e) {
    Button* button= (Button*)src->data ;
    if (button->disabled)
        return NONE ;

    if (e->type==SDL_MOUSEMOTION){
        SDL_Point p = {0, 0};
        SDL_GetMouseState(&p.x, &p.y) ;
        if (SDL_PointInRect(&p, button->location)&&!button->hover){
            SDL_RenderCopy(button->rend, button->highlightedTexture, NULL, button->location);
            button->hover = true ;
        }
        if (!SDL_PointInRect(&p, button->location)&&button->hover){
            SDL_RenderCopy(button->rend, button->texture, NULL, button->location);
            button->hover = false ;
        }
    }
    if(e->type==SDL_MOUSEBUTTONDOWN){
        SDL_Point p = {0, 0};
        SDL_GetMouseState(&p.x, &p.y) ;
        if (SDL_PointInRect(&p, button->location)&&!button->pressed&&!button->disabled){
            button->pressed = true ;
        }
    }
    if(e->type==SDL_MOUSEBUTTONUP){
        SDL_Point p = {0, 0};
        SDL_GetMouseState(&p.x, &p.y) ;
        button->pressed = false ;
        if (SDL_PointInRect(&p, button->location)){
            return (*button->action)() ;
        }
    }
    return NONE ;
}


/**
 * draw the given button according to the flags (disabled, pressed, etc.)
 * @param src
 * @param rend
 */
void drawButton(Widget* src, SDL_Renderer* rend){

    Button* button= (Button*)src->data ;
    SDL_SetTextureColorMod(button->texture, 255, 255, 255);

    // if the button is disabled apply grey color mod
    if (button->disabled){
        SDL_SetTextureColorMod(button->texture, 50, 50, 50);
        SDL_RenderCopy(rend, button->texture, NULL, button->location);
        return ;
    }
    if (button->pressed)
        SDL_RenderCopy(rend, button->pressedTexture, NULL, button->location);
    else if(button->hover)
        SDL_RenderCopy(rend, button->highlightedTexture, NULL, button->location);
    else
        SDL_RenderCopy(rend, button->texture, NULL, button->location);

}




//--------------------------------------ChessSquare Methods-----------------------------

/**
 * create a chessSquare widget.
 * @param rend
 * @param img path to bright or dark square images
 * @param location SDL_Rect* with square location
 * @param piece the texture of the piece to be located on the square (NULL if no piece)
 * @return Widget* to the square on success. NULL on allocation or SDL error
 */
Widget* createChessSquare(SDL_Renderer* rend, char* img, SDL_Rect* location, SDL_Texture* piece){
    ChessSquare* square = (ChessSquare*)malloc(sizeof(ChessSquare)) ;
    if (square==NULL)
        return NULL ;

    SDL_Surface* surface = SDL_LoadBMP(img) ;
    if (surface==NULL){
        printf("ERROR: unable to load image: %s\n", SDL_GetError());
        return NULL ;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(rend, surface) ;
    if (texture==NULL){
        SDL_FreeSurface(surface) ;
        printf("ERROR: unable to create texture from image: %s\n", SDL_GetError());
        return NULL ;
    }
    SDL_FreeSurface(surface) ;


    square->texture = texture ;
    square->location = location ;
    square->hover = false ;
    square->pressed = false ;
    square->capture = false ;
    square->threatend = false ;
    square->highlighted = false ;
    square->piece = piece ;

    Widget* widget = (Widget*)malloc(sizeof(Widget)) ;
    if (widget==NULL){
        printf("ERROR: allocation error while creating chessSquare\n") ;
        return NULL ;
    }

    widget->draw = &drawChessSquare ;
    widget->destroy = &destroyChessSquare ;
    widget->handleEvent = NULL ;
    widget->data = square ;

    return widget ;
}


/**
 * free all memory of given chess square
 * @param src
 */
void destroyChessSquare(Widget* src){
    ChessSquare* square = (ChessSquare*)src->data ;
    SDL_DestroyTexture(square->texture) ;
    free(square->location) ;
    free(square) ;
}



/**
 * draw a chess square according to the flags (threatend, capture etc.) in the correct color
 * uses 'SDL_SetTextureColorMod()' to change colors
 * @param src
 * @param rend
 */
void drawChessSquare(Widget* src, SDL_Renderer* rend){
    ChessSquare* square = (ChessSquare*)src->data ;
    SDL_SetTextureColorMod(square->texture, 255, 255, 255);
    if (square->hover||square->pressed)
        SDL_SetTextureColorMod(square->texture, 100, 100, 100);
    if (square->highlighted)
        SDL_SetTextureColorMod(square->texture, 0, 255, 0);
    if (square->capture)
        SDL_SetTextureColorMod(square->texture, 0, 50, 255);
    if (square->threatend)
        SDL_SetTextureColorMod(square->texture, 255, 0, 0);

    SDL_RenderCopy(rend, square->texture, NULL, square->location);
    SDL_RenderCopy(rend, square->piece, NULL, square->location);
}

//------------------------------------------------Window methods-----------------------------------------------

/**
 * a function to create a screen that ties together a window, renderer and background texture
 * @param width window width
 * @param height window height
 * @param backgroundImage path to background image
 * @param name name of the window
 * @return a pointer to the Screen screen on success. NULL on SDL or allocation error
 */
Screen* createScreen(int width, int height, char* backgroundImage, char* name){

    Screen* screen = (Screen*)malloc(sizeof(Screen)) ;
    if (screen==NULL)
        return NULL ;

    SDL_Window *window = SDL_CreateWindow(
            name,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width,
            height,
            SDL_WINDOW_OPENGL);

    // make sure window was created successfully
    if (window == NULL) {
        printf("ERROR: unable to create %s window: %s\n", name, SDL_GetError());
        return NULL;
    }

    // create a renderer for the window
    SDL_Renderer *rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (rend == NULL) {
        printf("ERROR: unable to create %s renderer: %s\n", name, SDL_GetError());
        SDL_DestroyWindow(window);
        return NULL;
    }

    // ensure renderer supports transparency
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

    // set background
    SDL_Surface* surface = SDL_LoadBMP(backgroundImage) ;
    if (surface==NULL){
        printf("ERROR: unable to load %s background image: %s\n", name, SDL_GetError());
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        return NULL ;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(rend, surface) ;
    if (texture==NULL){
        printf("ERROR: unable to create %s texture from image: %s\n", name, SDL_GetError());
        SDL_FreeSurface(surface) ;
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        return NULL ;
    }
    SDL_FreeSurface(surface) ;
    SDL_RenderCopy(rend, texture, NULL, NULL);

    screen->window = window ;
    screen->rend = rend ;
    screen->background = texture ;
    return screen ;
}


/**
 * an auxiliary function to destroy a Screen
 * @param screen a pointer to the Screen to destroy
 */
void destroyScreen(Screen* screen){
    SDL_DestroyTexture(screen->background) ;
    SDL_DestroyRenderer(screen->rend) ;
    SDL_DestroyWindow(screen->window) ;
}

