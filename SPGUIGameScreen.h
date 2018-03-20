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
#define BOARD_SIDE 668

#define PIECE_HEIGHT 64
#define PIECE_WIDTH 64

#define SQUARE_SIDE 75

#define UNDO_Y 50
#define GAME_X 100
#define GAME_Y 315

#define GAME_BUTTON_WIDTH 200
#define GAME_BUTTON_HEIGHT 50
#define GAME_VERTICAL_DIFF 70

#define GAME_NUM_BUTTONS 6


SP_GUI_MESSAGE drawGameWindow(SPChessGame* game) ;

ChessBoard* createGUIChessGame(SDL_Renderer* rend, char* brightSquareImg, char* darkSquareImg, SPChessGame* game) ;

void redrawBoard(ChessBoard* board, SPChessGame* game) ;

Widget** createGameButtons(SDL_Renderer* rend) ;

void drawBoard(SDL_Renderer* rend, ChessBoard* board) ;

SP_GUI_MESSAGE handleBoardEvent(ChessBoard* board, SDL_Event* e) ;

SP_GUI_MESSAGE createPieceTextures(char* piecesImg, SDL_Renderer* rend) ;

SDL_Texture* getPieceTex(char piece) ;

void clearBoard(ChessBoard* board) ;

SP_GUI_MESSAGE executeGUIMove(ChessBoard* board, SPMove* move) ;

SP_GUI_MESSAGE undoGUIMove(SPChessGame* game) ;

SP_GUI_MESSAGE colorPossibleMoves(ChessBoard* board, int row, int col) ;

void destroyGUIGame(ChessBoard* board);

void destroyPieceTextures();

void destroyButtons(Widget** buttons);

SP_GUI_MESSAGE undoAction();

SP_GUI_MESSAGE saveAction();

SP_GUI_MESSAGE loadAction();

SP_GUI_MESSAGE restartAction();

SP_GUI_MESSAGE mainAction();

SP_GUI_MESSAGE gameQuitAction();

#endif //SPFINALPROJECT_SPGUIGAMESCREEN_H
