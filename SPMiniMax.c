//
// Created by user on 6/23/2017.
//

#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>
#include "SPMiniMax.h"





/**
 * Given a game state, this function evaluates the best move according to
 * the current player. The function initiates a MiniMax algorithm up to a
 * specified length given by maxDepth. The current game state doesn't change
 * by this function including the history of previous moves.
 *
 * @param currentGame - The current game state
 * @param maxDepth - The maximum depth of the miniMax algorithm
 * @return
 * NULL if either currentGame is NULL or maxDepth <= 0 or an allocation error occured.
 * On success the function returns the best move for the current player. In case there is no legal
 * move possible, the function returns a '0 move' (all positions are 0)
 */
SPMove* spMinimaxSuggestMove(SPChessGame* game, int maxDepth){

    if (game == NULL||maxDepth<=0)
        return NULL ;

    SPMove* move ;
    SPMove* bestMove = (SPMove*)malloc(sizeof(SPMove)) ;
    int score ;
    int bestScore = (game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL) ? INT_MIN : INT_MAX ;
    int count = 0 ;
    int alpha = INT_MIN ;
    int beta = INT_MAX ;
    SPArrayList* possibleMoves ;

    if (bestMove==NULL)
        return NULL ;

    for (int j=0;j<SP_CHESS_GAME_N_COLUMNS;j++){
        for (int i=0;i<SP_CHESS_GAME_N_ROWS;i++){
            if (game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL&&islower(game->gameBoard[i][j])||
                    game->currentPlayer==SP_CHESS_GAME_BLACK_SYMBOL&&isupper(game->gameBoard[i][j])){
                possibleMoves = spChessGetMoves(game, i, j) ;
                if (possibleMoves==NULL)
                    return NULL ;

                for (int k=0; k<possibleMoves->actualSize; k++){
                    move = spArrayListGetAt(possibleMoves, k) ;
                    testMove(game, move) ;
                    score = spGetMinimaxScore(game, maxDepth-1, alpha, beta) ;
                    if (score==ALLOC_ERROR)
                        return NULL;
                    undoTestMove(game, move) ;

                    if ((game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL&&score>bestScore)||
                        (game->currentPlayer==SP_CHESS_GAME_BLACK_SYMBOL&&score<bestScore)){
                        bestScore = score ;
                        SPMoveCopy(bestMove, move) ;
                    }

                    if (game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL&&bestScore>alpha)
                        alpha = bestScore ;

                    if (game->currentPlayer==SP_CHESS_GAME_BLACK_SYMBOL&&bestScore<beta)
                        beta = bestScore ;

                    count++ ;
                }
                spArrayListDestroy(possibleMoves) ;
            }
        }
    }
    if (count==0){
        bestMove->destColumn = 0 ;
        bestMove->destRow = 0 ;
        bestMove->sourceColumn = 0 ;
        bestMove->sourceRow = 0 ;
    }
    return bestMove ;
}




/**
 * a recursive function that computes the minimal/maximal score of all possible game states stemming from current
 * game, up to depth recursion level.
 * @param game - the current game instance. assumes a valid game instance
 * @param depth - recursion level of the program. assumes >=0
 * @return  - an int representing the minimum/maximum score. ALLOC_ERROR on allocation error.
 */
int spGetMinimaxScore(SPChessGame* game, int depth, int alpha, int beta){

    if (depth==0)
        return getGameScore(game) ;

    int bestScore = (game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL) ? INT_MIN : INT_MAX ;
    SPArrayList* possibleMoves ;
    SPMove* move ;
    int score ;
    int count = 0 ;
    bool prune = false ;

    // Iterate over the board, column by column
    for (int j=0;j<SP_CHESS_GAME_N_COLUMNS;j++){
        for (int i=0;i<SP_CHESS_GAME_N_ROWS;i++){
            // If there is a piece of the current player on the current position, get all possible moves
            if (game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL&&islower(game->gameBoard[i][j])||
                game->currentPlayer==SP_CHESS_GAME_BLACK_SYMBOL&&isupper(game->gameBoard[i][j])){
                possibleMoves = spChessGetMoves(game, i, j) ;
                if (possibleMoves==NULL)
                    return ALLOC_ERROR ;

                // Iterate over all possible moves from the current position, for each, execute the move and get
                // the score recursively. this is like creating a child node in the minimax tree
                for (int k=0; k<possibleMoves->actualSize; k++){
                    move = spArrayListGetAt(possibleMoves, k) ;
                    testMove(game, move) ;
                    score = spGetMinimaxScore(game, depth-1, alpha, beta) ;
                    if (score==ALLOC_ERROR)
                        return ALLOC_ERROR ;
                    undoTestMove(game, move) ;

                    // update min/max score if neccesary
                    if ((game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL&&score>bestScore)||
                        (game->currentPlayer==SP_CHESS_GAME_BLACK_SYMBOL&&score<bestScore)){
                        bestScore = score ;
                    }

                    // alpha-beta pruning
                    if (game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL){
                        if (bestScore>alpha)
                            alpha = bestScore ;
                        if (beta<alpha)
                            prune = true ;
                    }

                    if (game->currentPlayer==SP_CHESS_GAME_BLACK_SYMBOL){
                        if (bestScore<beta)
                            beta = bestScore ;
                        if (beta<alpha)
                            prune = true ;
                    }
                    count ++ ;
                    if (prune)
                        break ;
                }
                spArrayListDestroy(possibleMoves) ;
            }
            if (prune)
                break;
        }
        if (prune)
            break ;
    }

    if (count==0)
        return getGameScore(game) ;
    return bestScore ;
}
