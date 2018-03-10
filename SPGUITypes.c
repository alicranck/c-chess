//
// Created by user on 3/9/2018.
//

#include "SPGUITypes.h"



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
    button->highlighted = false ;
    button->pressed = false ;

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
    SDL_DestroyTexture(button->highlightedTexture) ;
    SDL_DestroyTexture(button->pressedTexture) ;
    free(button) ;
}

SP_GUI_MESSAGE handleButtonEvent(Widget* src, SDL_Event* e) {
    Button* button= (Button*)src->data ;
    if (e->type==SDL_MOUSEMOTION){
        SDL_Point p = {0, 0};
        SDL_GetMouseState(&p.x, &p.y) ;
        if (SDL_PointInRect(&p, button->location)&&!button->highlighted){
            SDL_RenderCopy(button->rend, button->highlightedTexture, NULL, button->location);
            button->highlighted = true ;
        }
        if (!SDL_PointInRect(&p, button->location)&&button->highlighted){
            SDL_RenderCopy(button->rend, button->texture, NULL, button->location);
            button->highlighted = false ;
        }
    }
    if(e->type==SDL_MOUSEBUTTONDOWN){
        SDL_Point p = {0, 0};
        SDL_GetMouseState(&p.x, &p.y) ;
        if (SDL_PointInRect(&p, button->location)&&!button->pressed){
            SDL_RenderCopy(button->rend, button->pressedTexture, NULL, button->location);
            button->pressed = true ;
        }
    }
    if(e->type==SDL_MOUSEBUTTONUP){
        SDL_Point p = {0, 0};
        SDL_GetMouseState(&p.x, &p.y) ;
        if (SDL_PointInRect(&p, button->location)&&button->pressed){
            SDL_RenderCopy(button->rend, button->texture, NULL, button->location);
            button->pressed = false ;
            return (*button->action)() ;
        }
    }
    return NONE ;
}

void drawButton(Widget* src, SDL_Renderer* rend){
    Button* button= (Button*)src->data ;
    SDL_RenderCopy(rend, button->texture, NULL, button->location);
}