//
// Created by user on 3/10/2018.
//

#ifndef SPFINALPROJECT_SPGUIGAMESCREEN_H
#define SPFINALPROJECT_SPGUIGAMESCREEN_H

#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include "SPGUITypes.h"
#include "SPGUISaveLoadScreen.h"
#include "SPCHESSGame.h"
#include "SPMiniMax.h"


#define BOARD_Y 50
#define BOARD_X 506

#define PIECE_HEIGHT 200
#define PIECE_WIDTH 200
#define PIECE_GAP 23

#define SQUARE_SIDE 75

#define UNDO_Y 50
#define GAME_X 100
#define GAME_Y 315

#define GAME_BUTTON_WIDTH 200
#define GAME_BUTTON_HEIGHT 50
#define GAME_VERTICAL_DIFF 70

#define INDICATOR_X 406
#define WHITE_INDICATOR_Y 540
#define BLACK_INDICATOR_Y 80
#define INDICATOR_SIDE 70

#define GAME_NUM_BUTTONS 6



SP_GUI_MESSAGE drawGameWindow(SPChessGame* game) ;


/**
 * create an instance of a ChessBoard struct.
 * @param rend SDL_Renderer to create textures
 * @param brightSquareImg path to image of a dark chess square
 * @param darkSquareImg path to image of a bright chess square
 * @param game a pointer to an SPChessGame instance to be associated with this board
 * @return a ChessBoard pointer on sucess. NULL on SDL or allocation error
 */
ChessBoard* createGUIChessGame(SDL_Renderer* rend, char* brightSquareImg, char* darkSquareImg, SPChessGame* game) ;


/**
 *  Create game screen buttons
 * @param rend an SDL_Renderer for game window
 * @return an array of GAME_NUM_BUTTONS Widgets containing the buttons.  NULL on allocation error
 */
Widget** createGameButtons(SDL_Renderer* rend) ;


/**
 * draw a game board to screen
 * @param rend an SDL_Renderer pointer associated with this window
 * @param board a ChessBoard pointer for the board to draw
 */
void drawBoard(SDL_Renderer* rend, ChessBoard* board) ;


/**
 * reassign pieces textures to squares according to a given game state
 * @param board the board on which to reassign the pieces
 * @param game the game state to follow
 */
void redrawBoard(ChessBoard* board, SPChessGame* game) ;


/**
 * a function to handle all mouse events inside the chess board perimeter, i.e - highlight possible moves
 * on right click, selecting a piece, or executing a move.
 * @param board The board in which to handle the event
 * @param e the event
 * @return SP_GUI_MESSAGE NONE on success or ERROR on allocation error
 */
SP_GUI_MESSAGE handleBoardEvent(ChessBoard* board, SDL_Event* e) ;


/**
 * create game piece textures from image
 * @param piecesImg a path to the pieces sprite
 * @param rend
 * @return SP_GUI_MESSAGE NONE on success, ERROR on SDL or allocation error
 */
SP_GUI_MESSAGE createPieceTextures(char* piecesImg, SDL_Renderer* rend) ;


/**
 * returns the texture of the given piece
 * @param piece a char representing the piece
 * @return SDL_Texture* to the requested texture
 */
SDL_Texture* getPieceTex(char piece) ;


/**
 * clears all extra graphics (highlighting etc.) from the game squares
 * @param board
 */
void clearBoard(ChessBoard* board) ;


/**
 * create check indicators texture
 * @param rend an SDL_Renderer pointer associated with this window
 * @return a pointer to the created texture. NULL on SDL or allocation error
 */
SDL_Texture* createCheckIndicators(SDL_Renderer* rend) ;


/**
 * draw check indicators texture
 * @param rend an SDL_Renderer pointer associated with this window
 * @param tex the texture to draw
 * @param game an SPChessGame pointer to check for check
 */
void drawIndicators(SDL_Renderer* rend, SDL_Texture* tex, SPChessGame* game) ;


/**
 * execute a given move on the board and the game state
 * @param board a board instance on which to execute the move
 * @param move the move to execute
 * @return SP_GUI_MESSAGE NONE on success. ERROR in case of illegal move
 */
SP_GUI_MESSAGE executeGUIMove(ChessBoard* board, SPMove* move) ;


/**
 * undo last two moves (if possible). uses 'spChessUndoPrevMove' from 'SPCHESSGame.c'
 * @param game a game state to update
 * @return SP_GUI_MESSAGE NONE if no history, RELOAD_GAME on successful undo
 */
SP_GUI_MESSAGE undoGUIMove(SPChessGame* game) ;


/**
 * color possible moves according to suggested scheme: legal - green
 *                                                     capture - blue
 *                                                     threatend - red
 * @param board
 * @param row row of the position for which to check possible moves
 * @param col
 * @return ERROR on allocation error, NONE on success
 */
SP_GUI_MESSAGE colorPossibleMoves(ChessBoard* board, int row, int col) ;


/**
 * in case of mate/draw, present a dialog box for the user to determine next step
 * @param window
 * @param game
 * @return ERROR on SDL error. QUIT/RESTART_GAME/MAIN_MENU according to user selection otherwise
 */
SP_GUI_MESSAGE finishGUIGame(SDL_Window* window, SPChessGame* game);



void destroyGUIGame(ChessBoard* board);

void destroyPieceTextures();

void destroyButtons(Widget** buttons, int n);

SP_GUI_MESSAGE undoAction();

SP_GUI_MESSAGE saveAction();

SP_GUI_MESSAGE loadAction();

SP_GUI_MESSAGE restartAction();

SP_GUI_MESSAGE mainAction();

SP_GUI_MESSAGE gameQuitAction();

#endif //SPFINALPROJECT_SPGUIGAMESCREEN_H
