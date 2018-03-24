#ifndef SPMINIMAX_H_
#define SPMINIMAX_H_
#include "SPCHESSGame.h"


#define ALLOC_ERROR 7777777

/**
 * Given a game state, this function evaluates the best move according to
 * the current player. The function initiates a MiniMax algorithm up to a
 * specified length given by maxDepth. The current game state doesn't change
 * by this function including the history of previous moves.
 *
 * @param currentGame - The current game state
 * @param maxDepth - The maximum depth of the miniMax algorithm
 * @return
 * -1 if either currentGame is NULL or maxDepth <= 0.
 * On success the function returns a number between [0,SP_CHESS_GAME_N_COLUMNS -1]
 * which is the best move for the current player.
 */
SPMove* spMinimaxSuggestMove(SPChessGame* currentGame,
		int maxDepth);



/**
 * a recursive function that computes the minimal/maximal score of all possible game states stemming from current
 * game, up to depth recursion level.
 * @param game - the current game instance. assumes a valid game instance
 * @param depth - recursion level of the program. assumes >=0
 * @return  - an int representing the minimum/maximum score. ALLOC_ERROR on allocation error.
 */
int spGetMinimaxScore(SPChessGame* game, int depth, int alpha, int beta) ;






#endif
