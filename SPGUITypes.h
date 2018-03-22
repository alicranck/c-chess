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
    RELOAD_GAME,
    RESTART_GAME,
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


/**
 * a container for a graphical widget (button, chess square) with functions to draw, handle events,
 * and destroy the widget.
 */
typedef struct widget_t Widget ;

struct widget_t{
    void (*draw)(Widget*, SDL_Renderer*) ;
    SP_GUI_MESSAGE (*handleEvent)(Widget*, SDL_Event*) ;
    void (*destroy)(Widget*) ;
    void* data ;
};


/**
 * a container for buttons. has textures for normal, hover and pressed modes, location, action function
 * that is called on click, and flags to establish the current state (pressed, disabled, etc.)
 */
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


/**
 * a container for chess square. has texture for the square and the piece that sits on it, a location Rect,
 * and flags to establish it's state
 */
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


/**
 * a container for a complete GUI game instance. has a 2D array of squares, an SPChessGame game to
 * hold the game state, board location, and also an indication of the currently selected piece on the
 * board (to determine of there is a move to execute)
 */
typedef struct chess_board_t{
    Widget* squares[SP_CHESS_GAME_N_ROWS][SP_CHESS_GAME_N_COLUMNS] ;
    SPChessGame* game ;
    SDL_Rect* location ;
    bool pressed ;
    int pressedLocation[2] ;
}ChessBoard;


/**
 * a screen that holds together an SDL_window*, SDL_Renderer* associated with the
 * window, and a background texture.
 */
typedef struct chess_screen_t{
    SDL_Window* window ;
    SDL_Renderer* rend ;
    SDL_Texture* background ;
}Screen;



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
Widget* createButton(SDL_Renderer* rend, char* img, char* highlightImg,
                     char* pressedImg, SDL_Rect* location, SP_GUI_MESSAGE (*action)(void)) ;


/**
 * free all memory of a given button
 * @param src
 */
void destroyButton(Widget* src) ;


/**
 * handle button press event. activate hover/pressed graphics and call button action
 * if needed.
 * @param src
 * @param e
 * @return SP_GUI_MESSAGE NONE on success, or according to button action if clicked.
 */
SP_GUI_MESSAGE handleButtonEvent(Widget* src, SDL_Event* e) ;


/**
 * draw the given button according to the flags (disabled, pressed, etc.)
 * @param src
 * @param rend
 */
void drawButton(Widget* src, SDL_Renderer* rend) ;


//---------------------------------------ChessSquare methods---------------------------------

/**
 * create a chessSquare widget.
 * @param rend
 * @param img path to bright or dark square images
 * @param location SDL_Rect* with square location
 * @param piece the texture of the piece to be located on the square (NULL if no piece)
 * @return Widget* to the square on success. NULL on allocation or SDL error
 */
Widget* createChessSquare(SDL_Renderer* rend, char* img, SDL_Rect* location, SDL_Texture* piece) ;


/**
 * free all memory of given chess square
 * @param src
 */
void destroyChessSquare(Widget* src) ;


/**
 * a dummy function to handle chess square events. In practice all events are handled by the board
 * in 'SPGUIGameScreen.c'.
 * @param src
 * @param e
 * @return SP_GUI_MESSAGE NONE
 */
SP_GUI_MESSAGE handleChessSquareEvent(Widget* src, SDL_Event* e) ;


/**
 * draw a chess square according to the flags (threatend, capture etc.) in the correct color
 * uses 'SDL_SetTextureColorMod()' to change colors
 * @param src
 * @param rend
 */
void drawChessSquare(Widget* src, SDL_Renderer* rend) ;


/**
 * a function to create a screen that ties together a window, renderer and background texture
 * @param width window width
 * @param height window height
 * @param backgroundImage path to background image
 * @param name name of the window
 * @return a pointer to the Screen screen on success. NULL on SDL or allocation error
 */
Screen* createScreen(int width, int height, char* backgroundImage, char* name) ;


/**
 * an auxiliary function to destroy a Screen
 * @param screen a pointer to the Screen to destroy
 */
void destroyScreen(Screen* screen) ;


/**
 * an auxiliary method to main used to control game flow
 * @param msg an SP_GUI_MESSAGE to indicate the next screen to draw
 * @return a message that represents the next screen, program ends only in case msg==QUIT
 */
SP_GUI_MESSAGE launchScreen(SP_GUI_MESSAGE msg) ;


#endif //SPGUITYPES_H
