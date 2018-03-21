//
// Created by user on 6/23/2017.
//

#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "SPChessGame.h"

/**
 * Creates a new game with a specified settings.
 *
 * @settings - game mode, difficulty and user color used in the game
 * @return
 * NULL if either a memory allocation failure occurs or historySize <= 0.
 * Otherwise, a new game instant is returned.
 */
SPChessGame* spChessCreate(int *settings){

    SPChessGame* game = (SPChessGame*)malloc(sizeof(SPChessGame)) ;
    if (game==NULL)
        return NULL ;

    game->gameMode = settings[0] ;
    game->difficulty = settings[1] ;
    game->userColor = settings[2] ;

    game->whiteKing = 4 ;
    game->blackKing = 60 ;

    memcpy(game->gameBoard[0], "rnbqkbnr", 8);
    memcpy(game->gameBoard[1], "mmmmmmmm", 8);
    for (int i=2; i<6; i++){
        for (int j=0;j<SP_CHESS_GAME_N_COLUMNS; j++){
            game->gameBoard[i][j] = SP_CHESS_GAME_EMPTY_ENTRY ;
        }
    }
    memcpy(game->gameBoard[6], "MMMMMMMM", 8);
    memcpy(game->gameBoard[7], "RNBQKBNR", 8);

    game->currentPlayer = SP_CHESS_GAME_WHITE_SYMBOL ;
    game->history = spArrayListCreate(HISTORY_SIZE) ;
    if (game->history==NULL)
        return NULL ;

    return game ;
}


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
SPChessGame* spChessCopy(SPChessGame* src){

    if (src==NULL)
        return NULL ;
    int settings[3] = {src->gameMode, src->difficulty, src->userColor};
    SPChessGame* cpy = spChessCreate(settings) ;
    if (cpy==NULL)
        return NULL ;

    for (int i=0; i<SP_CHESS_GAME_N_ROWS; i++){
        for (int j=0;j<SP_CHESS_GAME_N_COLUMNS; j++){
            cpy->gameBoard[i][j] = src->gameBoard[i][j] ;
        }
    }
    
    cpy->currentPlayer = src->currentPlayer ;
    cpy->whiteKing = src->whiteKing ;
    cpy->blackKing = src->blackKing ;
    
    for (int i=0; i<src->history->actualSize;i++){
	    cpy->history->elements[i] = src->history->elements[i] ;
    }
    if (cpy->history==NULL)
        return NULL ;

    return cpy ;
}

/**
 * Frees all memory allocation associated with a given game. If src==NULL
 * the function does nothing.
 *
 * @param src - the source game
 */
void spChessDestroy(SPChessGame* src){

    if (src==NULL)
        return;

    spArrayListDestroy(src->history) ;

    free(src) ;
    return;
}


/**
 * a helper function that checks if a given move is possible in a given game. This function only checks if
 * the move is legal without checking for check.
 * @param game 
 * @param move 
 * @return true if a move is legal. false otherwise.
 */
bool spChessCheckLegalPieceMove(SPChessGame* game, SPMove* move, char player){
    char piece = game->gameBoard[move->sourceRow][move->sourceColumn] ;
    piece = (char)tolower(piece) ;
    bool ret ;
    if (move->sourceRow==move->destRow&&move->sourceColumn==move->destColumn)
        return false ;   // remaining in place is not allowed
    switch(piece) {
        case 'm':
            ret = checkPawn(game, move, player) ;
            break ;
        case 'r':
            ret = checkRook(game, move, player) ;
            break ;
        case 'n':
            ret = checkKnight(game, move, player) ;
            break ;
        case 'b':
            ret = checkBishop(game, move, player) ;
            break ;
        case 'k':
            ret = checkKing(game, move, player) ;
            break ;
        case 'q':
            if (checkRook(game, move, player)||checkBishop(game, move, player))
                ret = true ;
            else
                ret = false;
            break ;
        default:
            ret = false ;
    }
    return ret ;
}


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
SP_CHESS_GAME_MESSAGE spChessCheckLegalMove(SPChessGame* game, SPMove* move){

    char player = game->currentPlayer ;
    SP_CHESS_GAME_MESSAGE checkBefore ;
    SP_CHESS_GAME_MESSAGE checkAfter ;

    if (!spChessCheckLegalPieceMove(game, move, player))
        return SP_CHESS_GAME_ILLEGAL_MOVE ;

    checkBefore = spChessIsCheck(game, player) ;
    testMove(game, move) ;
    checkAfter = spChessIsCheck(game, player) ;
    undoTestMove(game, move) ;

    if (checkBefore==SP_CHESS_GAME_STANDART_ERROR||checkAfter==SP_CHESS_GAME_STANDART_ERROR)
        return SP_CHESS_GAME_STANDART_ERROR ;

    if (checkBefore==SP_CHESS_GAME_UNDER_THREAT&&checkAfter==SP_CHESS_GAME_UNDER_THREAT)
        return SP_CHESS_GAME_STILL_CHECK ;

    if (checkAfter==SP_CHESS_GAME_UNDER_THREAT)
        return SP_CHESS_GAME_INTO_CHECK ;

    return SP_CHESS_GAME_LEGAL_MOVE ;
}


/**
 * Checks the current gameboard to see if a certain player is under check from the opposition
 * @param game
 * @param player
 * @return SP_CHESS_GAME_UNDER_THREAT -  if player is under check
 *          SP_CHESS_GAME_NO_THREAT -  if player is not under check
 *          NULL on allocation error
 */
SP_CHESS_GAME_MESSAGE spChessIsCheck(SPChessGame* game, char player){
    int king = (player==SP_CHESS_GAME_WHITE_SYMBOL) ? game->whiteKing : game->blackKing ;
    int col = king%8 ;
    int row = (king-col)/8 ;
    return spChessIsThreatend(game, row, col, player) ;
}


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
SP_CHESS_GAME_MESSAGE spChessIsThreatend(SPChessGame* game, int row, int column, char player){
    char opposition = (player==SP_CHESS_GAME_WHITE_SYMBOL) ? (char)SP_CHESS_GAME_BLACK_SYMBOL : (char)SP_CHESS_GAME_WHITE_SYMBOL ;
    SPMove* move = (SPMove*)malloc(sizeof(SPMove)) ;
    if (move==NULL)
        return SP_CHESS_GAME_STANDART_ERROR ;
    move->destColumn = column ;
    move->destRow = row ;

    for (int i=0;i<SP_CHESS_GAME_N_ROWS;i++){
        for (int j=0;j<SP_CHESS_GAME_N_COLUMNS;j++){
            if ((player==SP_CHESS_GAME_WHITE_SYMBOL&&isupper(game->gameBoard[i][j]))||
                    (player==SP_CHESS_GAME_BLACK_SYMBOL&&islower(game->gameBoard[i][j]))){
                move->sourceRow = i ;
                move->sourceColumn = j ;
                if(spChessCheckLegalPieceMove(game, move, opposition)) {
                    //printf("piece %c move from <%d,%d> to <%d,%d> is threatning king\n",game->gameBoard[i][j], i, j, row, column) ;
                    free(move);
                    return SP_CHESS_GAME_UNDER_THREAT;
                }
            }

        }
    }
    free(move) ;
    return SP_CHESS_GAME_NO_THREAT ;
}


/**
 * simulates a move without actually executing it. used for testing if a move is legal,
 * and for suggesting possible moves of a certain piece, without losing history.
 * @param game
 * @param move
 */
void testMove(SPChessGame* game, SPMove* move) {
    char piece = game->gameBoard[move->sourceRow][move->sourceColumn];
    char dest = game->gameBoard[move->destRow][move->destColumn];
    move->previousPiece = dest;

    // Simulate move
    game->gameBoard[move->destRow][move->destColumn] = piece;
    game->gameBoard[move->sourceRow][move->sourceColumn] = SP_CHESS_GAME_EMPTY_ENTRY;
    if (piece=='k')
        game->whiteKing = move->destRow*8 + move->destColumn ;
    if (piece=='K')
        game->blackKing = move->destRow*8 + move->destColumn ;
    swapCurrent(game) ;
}


/**
 * undo a test move and revert board to previous state
 * @param game
 * @param move
 */
void undoTestMove(SPChessGame* game, SPMove* move){
    char dest = game->gameBoard[move->destRow][move->destColumn];

    // Restore board
    game->gameBoard[move->destRow][move->destColumn] = move->previousPiece ;
    game->gameBoard[move->sourceRow][move->sourceColumn] = dest ;
    if (dest=='k')
        game->whiteKing = move->sourceRow*8 + move->sourceColumn ;
    if (dest=='K')
        game->blackKing = move->sourceRow*8 + move->sourceColumn ;
    swapCurrent(game) ;
}


/**
 * executes a move on a given game board. assumes the move is legal
 * @param game - game instance
 * @param move - the move to execute
 * @return -1 on allocation error, 0 on success
 */
int executeMove(SPChessGame* game, SPMove* move){

    SPMove* history_move = (SPMove*)malloc(sizeof(SPMove)) ;
    if (history_move==NULL)
        return -1 ;

    SPMoveCopy(history_move, move) ;
    swapCurrent(game) ;  // Update current player
    move->previousPiece = game->gameBoard[move->destRow][move->destColumn] ;

    // Update board
    game->gameBoard[move->destRow][move->destColumn] = game->gameBoard[move->sourceRow][move->sourceColumn] ;
    game->gameBoard[move->sourceRow][move->sourceColumn] = SP_CHESS_GAME_EMPTY_ENTRY ;

    if (game->gameBoard[move->destRow][move->destColumn]=='k')
        game->whiteKing = move->destRow*8 + move->destColumn ;
    if (game->gameBoard[move->destRow][move->destColumn]=='K')
        game->blackKing = move->destRow*8 + move->destColumn ;

    if (spArrayListIsFull(game->history))
        spArrayListRemoveFirst(game->history) ;
    spArrayListAddLast(game->history, (void*)history_move) ;
    return 0 ;
}


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
SP_CHESS_GAME_MESSAGE spChessUndoPrevMove(SPChessGame* game){
    if (spArrayListIsEmpty(game->history))
        return SP_CHESS_GAME_NO_HISTORY ;

    char* player = (game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL) ? "black" : "white" ;
    SPMove* move = (SPMove*)spArrayListGetLast(game->history) ;
    if (move==NULL){
        return SP_CHESS_GAME_STANDART_ERROR ;
    }

    game->gameBoard[move->sourceRow][move->sourceColumn] = game->gameBoard[move->destRow][move->destColumn]  ;
    game->gameBoard[move->destRow][move->destColumn] = move->previousPiece ;

    if (move->previousPiece=='k')
        game->whiteKing = move->destRow*8 + move->destColumn ;
    if (move->previousPiece=='K')
        game->blackKing = move->destRow*8 + move->destColumn ;

    if (game->gameBoard[move->sourceRow][move->sourceColumn]=='k')
        game->whiteKing = move->sourceRow*8 + move->sourceColumn ;
    if (game->gameBoard[move->sourceRow][move->sourceColumn]=='K')
        game->blackKing = move->sourceRow*8 + move->sourceColumn ;

    printf("Undo move for %s player: <%d,%c> -> <%d,%c>\n", player, move->destRow+1, (char)move->destColumn+'A',
                                                            move->sourceRow+1, (char)move->sourceColumn+'A') ;

    swapCurrent(game) ;
    spArrayListRemoveLast(game->history) ;

    return SP_CHESS_GAME_SUCCESS ;
}


/**
 * get all possible legal moves for the piece in the given position.
 * @param game
 * @param row
 * @param col
 * @return SPArrayList* to a list of all possible moves.
 *          NULL on allocation error
 */
SPArrayList* spChessGetMoves(SPChessGame* game, int row, int col){
    SPArrayList* list = spArrayListCreate(28) ;  // Max possible moves for any piece is 28
    if (list==NULL)
        return NULL ;
    SPMove* move ;

    for (int j=0;j<SP_CHESS_GAME_N_COLUMNS;j++){
        for (int i=0;i<SP_CHESS_GAME_N_ROWS;i++){
            move = (SPMove*)malloc(sizeof(SPMove)) ;
            if (move==NULL)
                return NULL ;
            move->sourceRow = row ;
            move->sourceColumn = col ;
            move->destRow = i ;
            move->destColumn = j ;
            if(spChessCheckLegalMove(game, move)==SP_CHESS_GAME_LEGAL_MOVE)
                spArrayListAddLast(list, move) ;
            else
                free(move) ;
        }
    }
    return list ;
}


/**
 * On success, the function prints the board game. If an error occurs, then the
 * function does nothing.
 *
 * @param src - the target game
 * @return
 * SP_CHESS_GAME_INVALID_ARGUMENT - if src==NULL
 * SP_CHESS_GAME_SUCCESS - otherwise
 *
 */
SP_CHESS_GAME_MESSAGE spChessPrintBoard(SPChessGame* src){

    if (src==NULL)
        return SP_CHESS_GAME_INVALID_ARGUMENT ;

    for (int i=SP_CHESS_GAME_N_ROWS-1; i>=0; i--){
        printf("%d| ", i+1) ;
        for (int j=0; j<SP_CHESS_GAME_N_COLUMNS; j++){
            printf("%c ", src->gameBoard[i][j])  ;
        }
        printf("|\n") ;
    }

    printf("  -----------------\n") ;
    printf("   A B C D E F G H  \n") ;

    return SP_CHESS_GAME_SUCCESS ;
}


/**
* Checks if there's a winner in the specified game status.
* @param src - the source game
* @return
* SP_CHESS_GAME_CHECKMATE if current player is under checkmate
* SP_CHESS_GAME_DRAW - If the game is over and there's a tie
* SP_CHESS_GAME_STANDART_ERROR- on allocation error
* SP_CHESS_GAME_NO_WINNER - otherwise
*/
SP_CHESS_GAME_MESSAGE spChessCheckWinner(SPChessGame* game){

    SPArrayList* possibleMoves ;
    bool isCheck = (spChessIsCheck(game, game->currentPlayer)==SP_CHESS_GAME_UNDER_THREAT) ? true : false ;

    for (int j=0;j<SP_CHESS_GAME_N_COLUMNS;j++){
        for (int i=0;i<SP_CHESS_GAME_N_ROWS;i++){
            if (game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL&&islower(game->gameBoard[i][j])||
                game->currentPlayer==SP_CHESS_GAME_BLACK_SYMBOL&&isupper(game->gameBoard[i][j])){
                possibleMoves = spChessGetMoves(game, i, j) ;
                if (possibleMoves==NULL)
                    return SP_CHESS_GAME_STANDART_ERROR ;
                if (possibleMoves->actualSize>0)
                    return SP_CHESS_GAME_NO_WINNER ;
                spArrayListDestroy(possibleMoves) ;
            }
        }
    }

    if (!isCheck)
        return SP_CHESS_GAME_DRAW ;
    return SP_CHESS_GAME_CHECKMATE ;
}


/**
 * loads a game from a given filepath
 * @param game a pointer to the game into which to load the game state. assumed not NULL
 * @param filepath a string containing the path to the saved game
 * @return ILLEGAL_PATH if 'fopen' fails
 *          STANDART_ERROR if 'fwrite' fails
 *          NO_ERROR on success
 */
SP_CHESS_GAME_MESSAGE spChessLoad(SPChessGame* game, char* filepath){

    size_t ret ;

    FILE* f = fopen(filepath, "rb") ;
    if (f==NULL)
        return  SP_CHESS_GAME_ILLEGAL_PATH ;

    ret = fread(game->gameBoard, sizeof(char), 64, f) ;
    if (ret!=64)
        return SP_CHESS_GAME_STANDART_ERROR ;

    ret = fread(&(game->currentPlayer), sizeof(char), 1, f) ;
    if (ret!=1)
        return SP_CHESS_GAME_STANDART_ERROR ;

    ret = fread(&(game->gameMode), sizeof(int), 1, f) ;
    if (ret!=1)
        return SP_CHESS_GAME_STANDART_ERROR ;

    ret = fread(&(game->userColor), sizeof(int), 1, f) ;
    if (ret!=1)
        return SP_CHESS_GAME_STANDART_ERROR ;

    ret = fread(&(game->difficulty), sizeof(int), 1, f) ;
    if (ret!=1)
        return SP_CHESS_GAME_STANDART_ERROR ;

    ret = fread(&(game->blackKing), sizeof(int), 1, f) ;
    if (ret!=1)
        return SP_CHESS_GAME_STANDART_ERROR ;

    ret = fread(&(game->whiteKing), sizeof(int), 1, f) ;
    if (ret!=1)
        return SP_CHESS_GAME_STANDART_ERROR ;

    fclose(f) ;
    return SP_CHESS_GAME_SUCCESS ;
}


/**
 * saves a given game to a given filepath
 * @param game a pointer to the game to be saved
 * @param filepath a string containing the path to the saved game
 * @return ILLEGAL_PATH if 'fopen' fails
 *          STANDART_ERROR if 'fread' fails
 *          NO_ERROR on success
 */
SP_CHESS_GAME_MESSAGE spChessSave(SPChessGame* game, char* filepath){
    size_t ret ;

    FILE* f = fopen(filepath, "wb") ;
    if (f==NULL)
        return  SP_CHESS_GAME_ILLEGAL_PATH ;

    ret = fwrite(game->gameBoard, sizeof(char), 64, f) ;
    if (ret!=64)
        return SP_CHESS_GAME_STANDART_ERROR ;

    ret = fwrite(&game->currentPlayer, sizeof(char), 1, f) ;
    if (ret!=1)
        return SP_CHESS_GAME_STANDART_ERROR ;

    ret = fwrite(&game->gameMode, sizeof(int), 1, f) ;
    if (ret!=1)
        return SP_CHESS_GAME_STANDART_ERROR ;

    ret = fwrite(&game->userColor, sizeof(int), 1, f) ;
    if (ret!=1)
        return SP_CHESS_GAME_STANDART_ERROR ;

    ret = fwrite(&game->difficulty, sizeof(int), 1, f) ;
    if (ret!=1)
        return SP_CHESS_GAME_STANDART_ERROR ;

    ret = fwrite(&game->blackKing, sizeof(int), 1, f) ;
    if (ret!=1)
        return SP_CHESS_GAME_STANDART_ERROR ;

    ret = fwrite(&game->whiteKing, sizeof(int), 1, f) ;
    if (ret!=1)
        return SP_CHESS_GAME_STANDART_ERROR ;

    fclose(f) ;
    return SP_CHESS_GAME_SUCCESS ;
}

//---------------------------------------------------Auxiliary functions------------------------------------------------
/**
 * calculates the score of a given game.
 * @param game
 * @return
 */
int getGameScore(SPChessGame* game){

    int score = 0 ;

    SP_CHESS_GAME_MESSAGE ret = spChessCheckWinner(game) ;

    if (ret==SP_CHESS_GAME_CHECKMATE)
        return (game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL) ? 1000 : -1000 ;
    if (ret==SP_CHESS_GAME_DRAW)
        return 0;

    for (int i=0;i<SP_CHESS_GAME_N_ROWS;i++){
        for (int j=0;j<SP_CHESS_GAME_N_COLUMNS;j++){
            score += getPieceScore(game->gameBoard[i][j]);
        }
    }
    return score;
}


/**
 * calculates the score of a given piece
 * @param piece - the piece to test
 * @return the score
 */
int getPieceScore(char piece){
    int score = 0 ;
    if (piece==SP_CHESS_GAME_EMPTY_ENTRY)
        return 0;
    if (piece=='m'||piece=='M')
        score = 1 ;
    if (piece=='n'||piece=='N'||piece=='b'||piece=='B')
        score = 3 ;
    if (piece=='r'||piece=='R')
        score = 5 ;
    if (piece=='q'||piece=='Q')
        score = 9 ;
    if (piece=='k'||piece=='K')
        score = 100 ;

    if (islower(piece))
        return score ;
    else
        return -score ;
}


/**
 * checks if a move is legal for a pawn
 * @param game
 * @param move
 * @return true for a legal move. false otherwise.
 */
bool checkPawn(SPChessGame* game, SPMove* move,char player){
    char destSquare = game->gameBoard[move->destRow][move->destColumn] ;

    if (player==SP_CHESS_GAME_WHITE_SYMBOL) {
        if (move->destRow==move->sourceRow+1){
            if (move->destColumn==move->sourceColumn&&destSquare==SP_CHESS_GAME_EMPTY_ENTRY)
                return true ;
            if ((move->destColumn==move->sourceColumn-1||move->destColumn==move->sourceColumn+1)
                                                         &&isupper(destSquare))
                return true ;
        }
        if (move->sourceRow==1&&move->destRow==3&&move->sourceColumn==move->destColumn&&
            game->gameBoard[move->sourceRow+1][move->destColumn]==SP_CHESS_GAME_EMPTY_ENTRY&&
            destSquare==SP_CHESS_GAME_EMPTY_ENTRY)
            return true;
    }
    else if (player==SP_CHESS_GAME_BLACK_SYMBOL){
        if (move->destRow==move->sourceRow-1){
            if (move->destColumn==move->sourceColumn&&destSquare==SP_CHESS_GAME_EMPTY_ENTRY)
                return true ;
            if ((move->destColumn==move->sourceColumn-1||move->destColumn==move->sourceColumn+1)
                                                         &&islower(destSquare))
                return true ;
        }
        if (move->sourceRow==6&&move->destRow==4&&move->sourceColumn==move->destColumn&&
            game->gameBoard[move->sourceRow-1][move->destColumn]==SP_CHESS_GAME_EMPTY_ENTRY&&
            destSquare==SP_CHESS_GAME_EMPTY_ENTRY)
            return true;
    }
    return false ;
}


/**
 * checks if a move is legal for a Rook
 * @param game
 * @param move
 * @return true for a legal move. false otherwise.
 */
bool checkRook(SPChessGame* game, SPMove* move, char player) {
    char destSquare = game->gameBoard[move->destRow][move->destColumn];

    if(move->sourceColumn!=move->destColumn&&move->sourceRow!=move->destRow)
        return false ;

    if (move->sourceColumn==move->destColumn){
        int min = (move->sourceRow<move->destRow) ? move->sourceRow+1 : move->destRow+1 ;
        int max = (move->sourceRow>move->destRow) ? move->sourceRow : move->destRow ;
        while (min<max){
            if (game->gameBoard[min][move->sourceColumn]!=SP_CHESS_GAME_EMPTY_ENTRY)
                return false ;
            min++ ;
        }
    }
    if (move->sourceRow==move->destRow){
        int min = (move->sourceColumn<move->destColumn) ? move->sourceColumn+1 : move->destColumn+1 ;
        int max = (move->sourceColumn>move->destColumn) ? move->sourceColumn : move->destColumn ;
        while (min<max){
            if (game->gameBoard[move->sourceRow][min]!=SP_CHESS_GAME_EMPTY_ENTRY)
                return false ;
            min++ ;
        }
    }
    if (destSquare!=SP_CHESS_GAME_EMPTY_ENTRY){
        if (player==SP_CHESS_GAME_WHITE_SYMBOL&&islower(destSquare))
            return false ;
        if (player==SP_CHESS_GAME_BLACK_SYMBOL&&isupper(destSquare))
            return false ;

    }
    return true ;
}


/**
 * checks if a move is legal for a Knight
 * @param game
 * @param move
 * @return true for a legal move. false otherwise.
 */
bool checkKnight(SPChessGame* game, SPMove* move, char player) {
    char destSquare = game->gameBoard[move->destRow][move->destColumn];

    if ((move->destColumn==move->sourceColumn+1||move->destColumn==move->sourceColumn-1)&&
        (move->destRow==move->sourceRow+2||move->destRow==move->sourceRow-2)){
        if (destSquare==SP_CHESS_GAME_EMPTY_ENTRY)
            return true;
        if (player==SP_CHESS_GAME_WHITE_SYMBOL&&isupper(destSquare))
            return true ;
        if (player==SP_CHESS_GAME_BLACK_SYMBOL&&islower(destSquare))
            return true ;
    }
    if ((move->destColumn==move->sourceColumn+2||move->destColumn==move->sourceColumn-2)&&
        (move->destRow==move->sourceRow+1||move->destRow==move->sourceRow-1)){
        if (destSquare==SP_CHESS_GAME_EMPTY_ENTRY)
            return true;
        if (player==SP_CHESS_GAME_WHITE_SYMBOL&&isupper(destSquare))
            return true ;
        if (player==SP_CHESS_GAME_BLACK_SYMBOL&&islower(destSquare))
            return true ;
    }
    return false ;
}


/**
 * checks if a move is legal for a Bishop
 * @param game
 * @param move
 * @return true for a legal move. false otherwise.
 */
bool checkBishop(SPChessGame* game, SPMove* move, char player) {
    char destSquare = game->gameBoard[move->destRow][move->destColumn];

    int i = (move->sourceRow<move->destRow) ? 1 : -1 ;
    int j = (move->sourceColumn<move->destColumn) ? 1 : -1 ;
    int count = 1 ;

    if (move->sourceRow-move->destRow!=move->sourceColumn-move->destColumn&&
        move->sourceRow-move->destRow!=move->destColumn-move->sourceColumn)
        return false ;

    while (move->sourceRow+i*count!=move->destRow){
        if (game->gameBoard[move->sourceRow+i*count][move->sourceColumn+j*count]!=SP_CHESS_GAME_EMPTY_ENTRY)
            return false ;
        count++ ;
    }
    if (destSquare==SP_CHESS_GAME_EMPTY_ENTRY)
        return true;
    if (player==SP_CHESS_GAME_WHITE_SYMBOL&&isupper(destSquare))
        return true ;
    if (player==SP_CHESS_GAME_BLACK_SYMBOL&&islower(destSquare))
        return true ;
    return false ;
}


/**
 * checks if a move is legal for a King
 * @param game
 * @param move
 * @return true for a legal move. false otherwise.
 */
bool checkKing(SPChessGame* game, SPMove* move, char player) {
    char destSquare = game->gameBoard[move->destRow][move->destColumn];

    if (move->destRow>move->sourceRow+1||move->destRow<move->sourceRow-1||
        move->destColumn>move->sourceColumn+1||move->destColumn<move->sourceColumn-1)
        return false ;
    if (destSquare==SP_CHESS_GAME_EMPTY_ENTRY)
        return true;
    if (player==SP_CHESS_GAME_WHITE_SYMBOL&&isupper(destSquare))
        return true ;
    if (player==SP_CHESS_GAME_BLACK_SYMBOL&&islower(destSquare))
        return true ;
    return false ;
}


/**
 * copies the content of a move into another move (no allocation)
 * @param cpy a pointer to the destination SPMove
 * @param move a pointer to the source SPMove
 */
void SPMoveCopy(SPMove* cpy, SPMove* move){
    cpy->sourceRow = move->sourceRow ;
    cpy->sourceColumn = move->sourceColumn ;
    cpy->destRow = move->destRow ;
    cpy->destColumn = move->destColumn ;
    cpy->previousPiece = move->previousPiece ;
}


/**
 * swaps current player in input SPChessGame. Assumes src!=NULL
 *
 * @src - the source game whose current player will be swapped
 */
void swapCurrent(SPChessGame* game) {
    if (game->currentPlayer == SP_CHESS_GAME_WHITE_SYMBOL)
        game->currentPlayer = SP_CHESS_GAME_BLACK_SYMBOL ;
    else
        game->currentPlayer = SP_CHESS_GAME_WHITE_SYMBOL ;
}