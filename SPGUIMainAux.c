//
// Created by user on 3/9/2018.
//

#include "SPGUIMainAux.h"


Widget* createButton(SDL_Renderer* rend, char* img, SDL_Rect* location, void (*action)(void)){
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

    button->rend = rend ;
    button->texture = texture ;
    button->location = location ;
    button->action = action ;

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

void destroyButton(Widget* src){
    Button* button = (Button*)src->data ;
    SDL_DestroyTexture(button->texture) ;
    free(button) ;
    free(src) ;
}

void handleButtonEvent(Widget* src, SDL_Event* e) {
    printf("1\n") ;
    Button* button= (Button*)src->data ;
    if (e->type==SDL_MOUSEMOTION){
        int x ;
        int y ;
        SDL_GetMouseState(&x, &y) ;
        if (x>button->location->x&&x<button->location->x+button->location->w
                &&y>button->location->y&&y<button->location->y+button->location->h)
            printf("sddd\n") ;
    }
    if(e->type==SDL_MOUSEBUTTONDOWN)
        (*button->action)() ;
}

void drawButton(Widget* src, SDL_Renderer* rend){
    Button* button= (Button*)src->data ;
    SDL_RenderCopy(rend, button->texture, NULL, button->location);
}

void emptyFunc(){printf("hey111ldsd\n");}

int drawMainWindow(SDL_Window* window, SDL_Renderer* rend){

    // clear window to color red (r,g,b,a)
    SDL_SetRenderDrawColor(rend, 51, 102, 255, 0);
    SDL_RenderClear(rend);

    SDL_Rect rect = { .x = 150, .y = 150, .w = 300, .h = 300 };
    Widget* button = createButton(rend, "icn.bmp", &rect, &emptyFunc) ;
    button->draw(button, rend);
    while(1){
        SDL_RenderPresent(rend);
        SDL_Event e ;
        SDL_WaitEvent(&e) ;
        if (e.key.keysym.sym==SDLK_ESCAPE){
            break ;
        }
        (*button->handleEvent)(button, &e) ;
    }

    /**
    for (int i=0; i<8; i++){
        for (int j=0; j<8;j++){
            SDL_Rect rect = {.x = 300+75*i, .y = 75+75*j, .w = 75, .h = 75};
            // draw a blue rectangle
            SDL_SetRenderDrawColor(rend, 0, 0, 255*((i+j)%2), 0);
            SDL_RenderFillRect(rend, &rect);
        }
    }
    **/


    return 0 ;
}