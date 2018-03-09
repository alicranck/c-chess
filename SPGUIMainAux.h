//
// Created by user on 3/9/2018.
//

#ifndef SPFINALPROJECT_SPGUIMAINAUX_H
#define SPFINALPROJECT_SPGUIMAINAUX_H

#endif //SPFINALPROJECT_SPGUIMAINAUX_H

#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>

typedef struct widget_t Widget ;
struct widget_t{
    void (*draw)(Widget*, SDL_Renderer*) ;
    void (*handleEvent)(Widget*, SDL_Event*) ;
    void (*destroy)(Widget*) ;
    void* data ;
};

typedef struct button_t{
    SDL_Renderer* rend;
    SDL_Texture* texture ;
    SDL_Rect* location ;
    void (*action)(void) ;
}Button;


Widget* createButton(SDL_Renderer* rend, char* img, SDL_Rect* location, void (*action)(void)) ;

void destroyButton(Widget* src) ;

void handleButtonEvent(Widget* src, SDL_Event* e) ;

void drawButton(Widget* src, SDL_Renderer* rend) ;

int drawMainWindow(SDL_Window* window, SDL_Renderer* rend);