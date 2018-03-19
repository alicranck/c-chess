//
// Created by user on 3/10/2018.
//

#ifndef SPFINALPROJECT_SPGUIGAMESCREEN_H
#define SPFINALPROJECT_SPGUIGAMESCREEN_H

#endif //SPFINALPROJECT_SPGUIGAMESCREEN_H
#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include "SPGUITypes.h"
#include "SPCHESSGame.h"

#define BOARD_Y 50
#define BOARD_X 506
#define BOARD_SIDE 668

#define PIECE_HEIGHT 64
#define PIECE_WIDTH 64

#define SQUARE_SIDE 75


SP_GUI_MESSAGE drawGameWindow(int* settings) ;

ChessBoard* createGUIChessGame(SDL_Renderer* rend, char* brightSquareImg, char* darkSquareImg, int* settings) ;

void drawBoard(SDL_Renderer* rend, ChessBoard* board) ;

SP_GUI_MESSAGE handleBoardEvent(ChessBoard* board, SDL_Event* e) ;

SP_GUI_MESSAGE createPieceTextures(char* piecesImg, SDL_Renderer* rend) ;

SDL_Texture* getPieceTex(char piece) ;

void clearBoard(ChessBoard* board) ;

SP_GUI_MESSAGE executeGUIMove(ChessBoard* board, int row, int col) ;

SP_GUI_MESSAGE colorPossibleMoves(ChessBoard* board) ;
