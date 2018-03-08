#ifndef SPCHESSGAME_H_
#define SPCHESSGAME_H_
#include <stdbool.h>
#include <limits.h>
#include "SPArrayList.h"


/**
 * SPCHESSGame Summary:
 *
 * A container that represents a classic connect-4 game, a two players 6 by 7
 * board game (rows X columns). The container supports the following functions.
 *
 * spChessCreate           - Creates a new game board
 * spChessCopy             - Copies a game board
 * spChessDestroy          - Frees all memory resources associated with a game
 * spChessSetMove          - Sets a move on a game board
 * spChessIsValidMove      - Checks if a move is valid
 * spChessUndoPrevMove     - Undoes previous move made by the last player
 * spChessPrintBoard       - Prints the current board
 * spChessGetCurrentPlayer - Returns the current player
 * spChessCheckWinner          - Returns the winner's char or null char if the game is not over
 * swapCurrent                - Swaps the current player in given game
 * getGameScore               - Computes the score of a given game 
 * parseBoard                 - Turns a game board into a 2d array of ints for computing the score
 * horizontalSpan             - Computes a horizontal span of 4 cells in the board starting at a given index
 * verticalSpan               - Computes a vertical span of 4 cells in the board starting at a given index
 * diagonalSpan               - Computes a diagonal span of 4 cells in the board starting at a given index
 * dotProduct                 - Computes the "dot product" of two int arrays of the same size
 *
 */

//Definitions
#define SP_CHESS_GAME_N_ROWS 8
#define SP_CHESS_GAME_N_COLUMNS 8
#define SP_CHESS_GAME_WHITE_SYMBOL 'W'
#define SP_CHESS_GAME_BLACK_SYMBOL 'B'
#define SP_CHESS_GAME_TIE_SYMBOL '-'
#define SP_CHESS_GAME_EMPTY_ENTRY '_'
#define HISTORY_SIZE 6


typedef struct sp_chess_move {
    int sourceRow ;
    int sourceColumn ;
    int destRow ;
    int destColumn ;
    char previousPiece ;
} SPMove ;

typedef struct sp_chess_game_t {
	char gameBoard[8][8];
	char currentPlayer;
	int gameMode ;
	int difficulty ;
	int userColor ;
    int whiteKing ;
    int blackKing ;
    SPArrayList* history ;
} SPChessGame;



/**
 * Type used for returning error codes from game functions
 */
typedef enum sp_chess_game_message_t {
    SP_CHESS_GAME_DRAW,
    SP_CHESS_GAME_NO_WINNER,
    SP_CHESS_GAME_CHECKMATE,
	SP_CHESS_GAME_INVALID_MOVE,
	SP_CHESS_GAME_INVALID_ARGUMENT,
	SP_CHESS_GAME_NO_HISTORY,
	SP_CHESS_GAME_SUCCESS,
    SP_CHESS_GAME_STANDART_ERROR,
    SP_CHESS_GAME_UNDER_THREAT,
    SP_CHESS_GAME_NO_THREAT,
    SP_CHESS_GAME_ILLEGAL_MOVE,
    SP_CHESS_GAME_LEGAL_MOVE,
    SP_CHESS_GAME_STILL_CHECK,
    SP_CHESS_GAME_INTO_CHECK,
} SP_CHESS_GAME_MESSAGE;

/**
 * Creates a new game with a specified settings.
 *
 * @settings - game mode, difficulty and user color used in the game
 * @return
 * NULL if either a memory allocation failure occurs or historySize <= 0.
 * Otherwise, a new game instant is returned.
 */
SPChessGame* spChessCreate(int *settings);

/**
 *	Creates a copy of a given game.
 *	The new copy has the same status as the src game.
 *
 *	@param src - the source game which will be copied
 *	@return
 *	NULL if either src is NULL or a memory allocation failure occurred.
 *	Otherwise, an new copy of the source game is returned.
 *
 */
SPChessGame* spChessCopy(SPChessGame* src);

/**
 * Frees all memory allocation associated with a given game. If src==NULL
 * the function does nothing.
 *
 * @param src - the source game
 */
void spChessDestroy(SPChessGame* src);


/**
 * a helper function that checks if a given move is possible in a given game. This function only checks if
 * the move is legal without checking for check.
 * @param game
 * @param move
 * @return true if a move is legal. false otherwise.
 */
bool spChessCheckLegalPieceMove(SPChessGame* game, SPMove* move, char player) ;


/**
 * checks if a given move is legal on a given game instance. checks if the move is
 * legal, and also if the king is or becomes threatend.
 * @param game
 * @param move
 * @return SP_CHESS_GAME_ILLEGAL_MOVE - if the move is illegal
 *          SP_CHESS_GAME_STILL_CHECK - if the king is threatend before and after the move
 *          SP_CHESS_GAME_INTO_CHECK - if the king becomes threatend after the move
 *          SP_CHESS_GAME_LEGAL_MOVE - if the move is legal and the king is not threatend after
 */
SP_CHESS_GAME_MESSAGE spChessCheckLegalMove(SPChessGame* game, SPMove* move) ;


/**
 * Checks the current gameboard to see if a certain player is under check from the opposition
 * @param game
 * @param player
 * @return SP_CHESS_GAME_UNDER_THREAT -  if player is under check
 *          SP_CHESS_GAME_NO_THREAT -  if player is not under check
 *          NULL on allocation error
 */
SP_CHESS_GAME_MESSAGE spChessIsCheck(SPChessGame* game, char player) ;


/**
 * Checks if a given position (row, column) is threatend by the opposing player.
 * this is done by checking for every opposing piece if it can move to the given position.
 * @param game
 * @param row
 * @param column
 * @param player - the player who is threatend (or not). i.e. we check if the opposition of 'player' is threatening
 *                      the given position
 * @return SP_CHESS_GAME_UNDER_THREAT if the position is under threat
 *          SP_CHESS_GAME_NO_THREAT - if no opposing piece is threatening the position.
 */
SP_CHESS_GAME_MESSAGE spChessIsThreatend(SPChessGame* game, int row, int column, char player) ;


/**
 * simulates a move without actually executing it. used for testing if a move is legal,
 * and for suggesting possible moves of a certain piece, without losing history.
 * @param game
 * @param move
 */
void testMove(SPChessGame* game, SPMove* move) ;


/**
 * undo a test move and revert board to previous state
 * @param game
 * @param move
 */
void undoTestMove(SPChessGame* game, SPMove* move) ;


/**
 * executes a move on a given game board. assumes the move is legal
 * @param game - game instance
 * @param move - the move to execute
 * @return -1 on allocation error. 0 on success
 */
int executeMove(SPChessGame* game, SPMove* move) ;


/**
 * Undo a previous move and changes the current player's turn.
 * If the user invoked this command more than historySize times in a row then an error occurs.
 *
 * @param game - The source game
 * @return
 * SP_CHESS_GAME_NO_HISTORY       - if the user invoked this function more then
 *                                 historySize in a row.
 * SP_CHESS_GAME_SUCCESS          - on success. The last disc that was put on the
 *                                 board is removed and the current player is changed
 */
SP_CHESS_GAME_MESSAGE spChessUndoPrevMove(SPChessGame* game);


/**
 * On success, the function prints the board game. If an error occurs, then the
 * function does nothing. The characters 'X' and 'O' are used to represent
 * the discs of player 1 and player 2, respectively.
 *
 * @param src - the target game
 * @return
 * SP_CHESS_GAME_INVALID_ARGUMENT - if src==NULL
 * SP_CHESS_GAME_SUCCESS - otherwise
 *
 */
SP_CHESS_GAME_MESSAGE spChessPrintBoard(SPChessGame* src);


/**
 * get all possible legal moves for the piece in the given position.
 * @param game
 * @param row
 * @param col
 * @return SPArrayList* to a list of all possible moves.
 *          NULL on allocation error
 */
SPArrayList* spChessGetMoves(SPChessGame* game, int row, int col);


/**
* Checks if there's a winner in the specified game status.
* @param src - the source game
* @return
* SP_CHESS_GAME_CHECKMATE if current player is under checkmate
* SP_CHESS_GAME_DRAW - If the game is over and there's a tie
* SP_CHESS_GAME_STANDART_ERROR- on allocation error
* SP_CHESS_GAME_NO_WINNER - otherwise
*/
SP_CHESS_GAME_MESSAGE spChessCheckWinner(SPChessGame* game);


//--------------------------------------------------auxiliary functions----------------------------------------
/**
 * swaps current player in input SPCHESSGame
 * assumes src is not NULL
 * @src - the source game whose current player will be swapped
 */
void swapCurrent(SPChessGame* src) ;


/**
 * calculates the score of a given game.
 * @param game
 * @return
 */
int getGameScore(SPChessGame* game) ;


/**
 * calculates the score of a given piece
 * @param piece - the piece to test
 * @return the score
 */
int getPieceScore(char piece) ;

/**
 * checks if a move is legal for a pawn
 * @param game
 * @param move
 * @return true for a legal move. false otherwise.
 */
bool checkPawn(SPChessGame* game, SPMove* move, char player) ;


/**
 * checks if a move is legal for a rook
 * @param game
 * @param move
 * @return true for a legal move. false otherwise.
 */
bool checkRook(SPChessGame* game, SPMove* move, char player) ;

/**
 * checks if a move is legal for a Knight
 * @param game
 * @param move
 * @return true for a legal move. false otherwise.
 */
bool checkKnight(SPChessGame* game, SPMove* move, char player) ;


/**
 * checks if a move is legal for a bishop
 * @param game
 * @param move
 * @return true for a legal move. false otherwise.
 */
bool checkBishop(SPChessGame* game, SPMove* move, char player) ;


/**
 * checks if a move is legal for a king
 * @param game
 * @param move
 * @return true for a legal move. false otherwise.
 */
bool checkKing(SPChessGame* game, SPMove* move, char player) ;


/**
 * copies the content of a move into another move (no allocation)
 * @param cpy a pointer to the destination SPMove
 * @param move a pointer to the source SPMove
 */
void SPMoveCopy(SPMove* cpy, SPMove* move) ;

#endif
