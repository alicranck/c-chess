//
// Created by user on 3/9/2018.
//

#ifndef SPFINALPROJECT_SPGUIBUTTON_H
#define SPFINALPROJECT_SPGUIBUTTON_H

#endif //SPFINALPROJECT_SPGUIBUTTON_H

#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include "SPCHESSGame.h"

typedef enum sp_gui_message_t{
    START_NEW_GAME,
    LOAD_GAME,
    QUIT
}SP_GUI_MESSAGE;


typedef struct widget_t Widget ;
struct widget_t{
    void (*draw)(Widget*, SDL_Renderer*) ;
    SP_GUI_MESSAGE (*handleEvent)(Widget*, SDL_Event*) ;
    void (*destroy)(Widget*) ;
    void* data ;
};

typedef struct button_t{
    SDL_Renderer* rend;
    SDL_Texture* texture ;
    SDL_Texture* highlightedTexture ;
    SDL_Texture* pressedTexture ;
    SDL_Rect* location ;
    bool highlighted ;
    bool pressed ;
    SP_GUI_MESSAGE (*action)(void) ;
}Button;


Widget* createButton(SDL_Renderer* rend, char* img, char* highlightImg,
                     char* pressedImg, SDL_Rect* location, SP_GUI_MESSAGE (*action)(void)) ;

void destroyButton(Widget* src) ;

SP_GUI_MESSAGE handleButtonEvent(Widget* src, SDL_Event* e) ;

void drawButton(Widget* src, SDL_Renderer* rend) ;