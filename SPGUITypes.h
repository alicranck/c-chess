//
// Created by user on 3/9/2018.
//

#ifndef SPGUITYPES_H
#define SPGUITYPES_H


#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include "SPCHESSGame.h"

typedef enum sp_gui_message_t{
    START_NEW_GAME,
    SAVE_GAME,
    LOAD_GAME,
    MAIN_MENU,
    START_GAME,
    MODE_ONE_PLAYER,
    MODE_TWO_PLAYER,
    COLOR_WHITE,
    COLOR_BLACK,
    LEVEL_AMATEUR,
    LEVEL_EASY,
    LEVEL_MODERATE,
    LEVEL_HARD,
    LEVEL_EXPERT,
    UNDO_MOVE,
    UP,
    DOWN,
    BACK,
    QUIT,
    ERROR,
    NONE
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
    bool hover ;
    bool pressed ;
    bool disabled ;
    SP_GUI_MESSAGE (*action)(void) ;
}Button;


typedef struct chess_square_t{
    SDL_Texture* piece ;
    SDL_Texture* texture ;
    SDL_Rect* location ;
    bool hover ;
    bool highlighted ;
    bool pressed ;
    bool threatend ;
    bool capture ;
}ChessSquare;


typedef struct chess_board_t{
    Widget* squares[SP_CHESS_GAME_N_ROWS][SP_CHESS_GAME_N_COLUMNS] ;
    SPChessGame* game ;
    SDL_Rect* location ;
    bool pressed ;
    int pressedLocation[2] ;
}ChessBoard;



Widget* createButton(SDL_Renderer* rend, char* img, char* highlightImg,
                     char* pressedImg, SDL_Rect* location, SP_GUI_MESSAGE (*action)(void)) ;

void destroyButton(Widget* src) ;

SP_GUI_MESSAGE handleButtonEvent(Widget* src, SDL_Event* e) ;

void drawButton(Widget* src, SDL_Renderer* rend) ;

Widget* createChessSquare(SDL_Renderer* rend, char* img, SDL_Rect* location, SDL_Texture* piece) ;

void destroyChessSquare(Widget* src) ;

SP_GUI_MESSAGE handleChessSquareEvent(Widget* src, SDL_Event* e) ;

void drawChessSquare(Widget* src, SDL_Renderer* rend) ;

SP_GUI_MESSAGE launchScreen(SP_GUI_MESSAGE screen) ;


#endif //SPGUITYPES_H
