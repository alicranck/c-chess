#ifndef SPMAINAUX_H_
#define SPMAINAUX_H_

#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "SPCHESSGame.h"
#include "SPCHESSParser.h"
#include "SPMiniMax.h"

#define LINE_SIZE 1024
#define HISTORY_SIZE 6


/**
 * a container for all possible errors in a run of SP_CHESS
 */
typedef enum {
    SAVE_ERROR,
    ILLEGAL_PATH,
    STANDART_ERROR ,
    INVALID_DIFFICULTY,
    INVALID_USER_COLOR,
    INVALID_GAME_MODE,
    NO_EXPERT_MODE,
    LOAD_ERROR,
    INVALID_COMMAND ,
    ILLEGAL_MOVE ,
    UNDER_CHECK,
    INTO_CHECK,
    INVALID_LOCATION ,
    NO_PIECE,
    EMPTY_PIECE,
    UNDO_ERROR ,
    GAME_OVER ,
    NO_ERROR ,
}ERROR;




/**
 * gets game settings from user and creates a new game with those settings
 * @return -  a pointer to a game instance with the specified settings
 */
SPChessGame* startNewGame() ;

/**
 * prints game current settings
 * @param settings - an integer array representing the game current setting
 */
void printSettings(int* settings) ;

/**
 * prints one of the ERROR type errors to the user
 * @param err - the ERROR to print
 * @param arg  - an additional argument if neccesary (for example the standart function that failed)
 */
void printError(ERROR err, char* arg) ;


/**
 * gets a command from the user and parses it
 * @return - a valid SPCommand, or NULL if fgets() failed
 */
SPCommand* getCommandFromUser(int game_mode) ;


/**
 * gets a command from the user and parses it. This function is used in
 * game mode.
 * @return - a valid SPCommand, or NULL if fgets() failed
 */
SPCommand* getMoveFromUser(SPChessGame* game) ;


/**
 * Checks if a 'move' or 'get_moves' command is legal. i.e. checks if the source and destination locations
 * are in the board and if there is an appropriate piece in the source location.
 * @param game a game instance. assumed not NULL
 * @param cmd the command to check. assumed not NULL
 * @return appropriate error if there is one, or NO_ERROR if the command is valid.
 */
ERROR checkLegalCommand(SPChessGame* game, SPCommand* cmd) ;


/**
 * an auxiliary function to execute user commands on given game instance
 * assumes neither the command nor the game are NULL, and that cmd.arg is a valid int in range(1-7)
 * prints an appropriate error if one occurs.
 * @param cmd - the command to execute
 * @param game - the game instance on which to execute it
 * @return - an integer number to indicate the correct continuation of the game loop
 *              (1=quit, 2=restart, 3=print status and repeat user turn, 4=execute computer turn
 *                      5=wait for further commands from user)
 */
int executeUserCommand(SPCommand* cmd, SPChessGame* game) ;


/**
 * executs undo_move command. assumes game is not NULL
 * @param game - game on which to execute move
 * @return UNDO_ERROR Error in case there is no history. NO_ERROR on success
 */
ERROR undoMove(SPChessGame* game) ;


/**
 * suggests (prints) possible moves to the user
 * @param game - game instance to suggest a move for
 * @param maxDepth - recursion depth for minimax algorithm
 * @return STANDART_ERROR Error in case of allocation error during minimax run. NO_ERROR on success.
 */
ERROR getMoves(SPChessGame* game,int row, int col) ;


/**
 * executes the computer's turn of the game
 * @param game - the game on which to play the turn
 * @return 0 on success, -1 on malloc() failure
 */
int executeComputerTurn(SPChessGame* game) ;

/**
 * prints the appropriate message when game is over (tie or there is a winner)
 * @param winner - the char of the winning player
 */
void finishGame(SPChessGame* game, SP_CHESS_GAME_MESSAGE result) ;


/**
 * returns the name of the difficulty level given by the parameter
 * @param difficulty an int representing a legal level
 * @return a string with the name of the level
 */
char* getDifficultyString(int difficulty) ;


/**
 * gets a char that represents a piece and returns it's name
 * @param piece a char representing a legal piece
 * @return a string representing the name of the piece
 */
char* getPieceString(char piece) ;


/**
 * terminates the program
 */
void quit() ;

#endif
