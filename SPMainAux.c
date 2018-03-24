//
// Created by user on 6/24/2017.
//
#include "SPMainAux.h"




/**
 * gets game settings from user and creates a new game with those settings
 * @return -  a pointer to a game instance with the specified settings
 */
SPChessGame* startNewGame(){

    SPCommand* cmd ;
    SPChessGame* game = NULL ;
    SP_CHESS_GAME_MESSAGE msg ;
    bool loaded = false ;
    bool start = false ;
    int settings[3] = {1,2,1};
    printf("Specify game settings or type 'start' to begin a game with the current settings:\n");

    while (true) {
        cmd = getCommandFromUser(settings[0]) ;
        switch(cmd->cmd){
            case SP_GAME_MODE:
                settings[0] = cmd->arg ;
                if (loaded)
                    game->gameMode = cmd->arg ;
                char* str = (cmd->arg == 1) ? "1-player" : "2-player" ;
                printf("Game mode is set to %s\n", str) ;
                break ;
            case SP_DIFFICULTY:
                settings[1] = cmd->arg ;
                if (loaded)
                    game->difficulty = cmd->arg ;
                char* level = getDifficultyString(settings[1]) ;
                printf("Difficulty level is set to %s\n", level) ;
                break ;
            case SP_USER_COLOR:
                settings[2] = cmd->arg ;
                if (loaded)
                    game->userColor = cmd->arg ;
                char* color = (cmd->arg==0) ? "black" : "white" ;
                printf("User color is set to %s\n", color) ;
                break ;
            case SP_LOAD:
                game = spChessCreate(settings) ;
                if (game==NULL){
                    printError(STANDART_ERROR, "malloc()") ;
                    break ;
                }
                msg = spChessLoad(game, cmd->filePath) ;
                if (msg==SP_CHESS_GAME_STANDART_ERROR){
                    printError(STANDART_ERROR, "fread()") ;
                    spChessDestroy(game) ;
                    break ;
                }
                if (msg==SP_CHESS_GAME_ILLEGAL_PATH){
                    printError(ILLEGAL_PATH, NULL) ;
                    spChessDestroy(game) ;
                    break ;
                }
                loaded = true ;
                settings[0] = game->gameMode;
                settings[1] = game->difficulty ;
                settings[2] = game->userColor ;
                break ;
            case SP_DEFAULT:
                settings[0] = 1 ;
                settings[1] = 2 ;
                settings[2] = 1 ;
                if (loaded){
                    game->gameMode = 1 ;
                    game->difficulty = 2;
                    game->userColor = 1 ;
                }
                printf("All settings reset to default\n") ;
                break ;
            case SP_PRINT_SETTING:
                printSettings(settings) ;
                break ;
            case SP_QUIT:
                if (loaded)
                    spChessDestroy(game) ;
                quit() ;
                break ;
            case SP_START:
                if (loaded){
                    start = true ;
                    break ;
                }
                game = spChessCreate(settings) ;
                if (game == NULL){
                    printError(STANDART_ERROR, "malloc()") ;
                    break ;
                }
                start = true ;
                break ;
            default:
                break ;
        }
        if (start)
            break ;
    }
    return game ;
}


/**
 * prints game current settings
 * @param settings - an integer array representing the game current setting
 */
void printSettings(int* settings){
    if (settings[0]==2){
        printf("SETTINGS:\nGAME_MODE: 2-player\n");
        return;
    }
    char* color = (settings[2]==0) ? "black" : "white" ;
    char* level = getDifficultyString(settings[1]) ;
    printf("SETTINGS:\nGAME_MODE: 1-player\nDIFFICULTY: %s\nUSER_COLOR: %s\n", level, color);
    return;
}


/**
 * prints one of the ERROR type errors to the user
 * @param err - the ERROR to print
 * @param arg  - an additional argument if neccesary (for example the standard function that failed)
 */
void printError(ERROR err, char* arg){

    switch (err){
        case STANDART_ERROR:
            printf("Error: %s has failed\n", arg) ;
            break ;
        case INVALID_DIFFICULTY:
            printf("Wrong difficulty level. The value should be between 1 to 5\n") ;
            break ;
        case NO_EXPERT_MODE:
            printf("Expert level not supported, please choose a value between 1 to 4\n") ;
            break ;
        case INVALID_GAME_MODE:
            printf("Wrong game mode\n") ;
            break ;
        case INVALID_USER_COLOR:
            printf("Wrong user color. The value should be 0 or 1\n") ;
            break;
        case LOAD_ERROR:
            printf("Error: File doesn’t exist or cannot be opened\n") ;
            break ;
        case ILLEGAL_PATH:
            printf("File cannot be created or modified\n") ;
        case INVALID_LOCATION:
            printf("Invalid position on the board\n") ;
            break;
        case NO_PIECE:
            printf("The specified position does not contain your piece\n") ;
            break ;
        case ILLEGAL_MOVE:
            printf("Illegal move\n") ;
            break ;
        case UNDER_CHECK:
            printf("Illegal move: king is still threatened\n") ;
            break ;
        case INTO_CHECK:
            printf("Illegal move: king will be threatened\n") ;
            break ;
        case INVALID_COMMAND :
            printf("Error: invalid command\n") ;
            break ;
        case UNDO_ERROR :
            printf("Error: cannot undo previous move!\n") ;
            break ;
        case GAME_OVER :
            printf("Error: the game is over\n") ;
            break ;
	default:
	    break ;
    }
}


/**
 * gets a command from the user and parses it. This function is used in
 * settings mode.
 * @return - a valid SPCommand
 */
SPCommand* getCommandFromUser(int game_mode){

    char line[LINE_SIZE] ;
    SPCommand* cmd ;

    while (true) {
        if (fgets(line, LINE_SIZE, stdin) == NULL) {
            printError(STANDART_ERROR, "fgets()") ;
            continue ;
        }

	    cmd = spParserPraseLine(line) ;

        if (cmd->cmd == SP_INVALID_LINE){
            printError(INVALID_COMMAND, NULL) ;
            continue ;
        }
        if (cmd->cmd == SP_GAME_MODE&&(cmd->arg>2||cmd->arg<1)){
            printError(INVALID_GAME_MODE, NULL) ;
            continue ;
        }
        if (cmd->cmd == SP_DIFFICULTY || cmd->cmd == SP_USER_COLOR){
            if (game_mode == 2) {
                printError(INVALID_COMMAND, NULL);
                continue;
            }
        }
        if (cmd->cmd == SP_DIFFICULTY&&cmd->validArg==false){
            printError(INVALID_DIFFICULTY, NULL) ;
            continue ;
        }
        if (cmd->cmd == SP_USER_COLOR&&cmd->validArg==false){
            printError(INVALID_USER_COLOR, NULL) ;
            continue ;
        }
        if (cmd->cmd==SP_GET_MOVES||cmd->cmd==SP_MOVE||cmd->cmd==SP_SAVE||
                cmd->cmd==SP_RESTART||cmd->cmd==SP_UNDO_MOVE){
            printError(INVALID_COMMAND, NULL) ;
            continue ;
        }

        return cmd ;
    }
}

/**
 * gets a command from the user and parses it. This function is used in
 * game mode.
 * @return - a valid SPCommand
 */
SPCommand* getMoveFromUser(SPChessGame* game){

    char line[LINE_SIZE] ;
    SPCommand* cmd ;
    char *player = (game->currentPlayer==SP_CHESS_GAME_BLACK_SYMBOL) ? "black" : "white" ;

    while (true) {
        printf("Enter your move (%s player):\n", player) ;
        if (fgets(line, LINE_SIZE, stdin) == NULL) {
            printError(STANDART_ERROR, "fgets()");
            continue ;
        }

        cmd = spParserPraseLine(line) ;
        if (cmd==NULL){
            printError(STANDART_ERROR, "malloc()");
            continue ;
        }

        if (cmd->cmd == SP_INVALID_LINE){
            printError(INVALID_COMMAND, NULL) ;
            continue ;
        }
        if (cmd->cmd == SP_MOVE){
            ERROR ret = checkLegalCommand(game, cmd) ;
            if (ret!=NO_ERROR) {
                printError(ret, NULL);
                continue;
            }
        }
        if (cmd->cmd == SP_GET_MOVES){
            ERROR ret = checkLegalCommand(game, cmd) ;
            if (ret==STANDART_ERROR){
                printError(STANDART_ERROR, "malloc()") ;
                continue ;
            }
            if (ret!=NO_ERROR) {
                printError(ret, NULL);
                continue;
            }
        }
        if (cmd->cmd!=SP_MOVE&&cmd->cmd!=SP_UNDO_MOVE&&cmd->cmd!=SP_SAVE&&
                cmd->cmd!=SP_GET_MOVES&&cmd->cmd!=SP_RESTART&&cmd->cmd!=SP_QUIT){
            printError(INVALID_COMMAND, NULL) ;
            continue ;
        }
        return cmd ;
    }
}


/**
 * Checks if a 'move' or 'get_moves' command is legal. i.e. checks if the source and destination locations
 * are in the board and if there is an appropriate piece in the source location.
 * @param game a game instance. assumed not NULL
 * @param cmd the command to check. assumed not NULL
 * @return appropriate error if there is one, or NO_ERROR if the command is valid.
 */
ERROR checkLegalCommand(SPChessGame* game, SPCommand* cmd){
    char player = game->currentPlayer ;
    char piece = game->gameBoard[cmd->move->sourceRow][cmd->move->sourceColumn] ;
    SP_CHESS_GAME_MESSAGE ret ;

    if (cmd->move->sourceRow < 0 || cmd->move->sourceRow > 7 || cmd->move->sourceColumn < 0 ||
        cmd->move->sourceColumn > 7)
        return INVALID_LOCATION;
    if (piece == SP_CHESS_GAME_EMPTY_ENTRY || (player == SP_CHESS_GAME_WHITE_SYMBOL && isupper(piece)) ||
        (player == SP_CHESS_GAME_BLACK_SYMBOL && islower(piece)))
        return NO_PIECE;
    if (cmd->cmd==SP_GET_MOVES)
        return NO_ERROR ;

    if(cmd->move->destRow<0||cmd->move->destRow>7||cmd->move->destColumn<0||cmd->move->destColumn>7)
        return INVALID_LOCATION ;

    ret = spChessCheckLegalMove(game, cmd->move) ;
    if (ret==SP_CHESS_GAME_STANDART_ERROR)
        return STANDART_ERROR ;
    if (ret==SP_CHESS_GAME_STILL_CHECK)
        return UNDER_CHECK ;
    if (ret==SP_CHESS_GAME_INTO_CHECK)
        return INTO_CHECK ;
    if (ret==SP_CHESS_GAME_ILLEGAL_MOVE)
        return ILLEGAL_MOVE ;

    return NO_ERROR ;
}


/**
 * an auxiliary function to execute user commands on given game instance
 * assumes neither the command nor the game are NULL, and that arguments are valid.
 * prints an appropriate error if one occurs.
 * @param cmd - the command to execute
 * @param game - the game instance on which to execute it
 * @return - an integer number to indicate the correct continuation of the game loop
 *              (1=quit, 2=restart, 3=print status and repeat user turn, 4=execute computer turn
 *                      5=wait for further commands from user)
 */
int executeUserCommand(SPCommand* cmd, SPChessGame* game){
    int ret = 3 ;
    ERROR err = NO_ERROR ;
    SP_CHESS_GAME_MESSAGE msg ;
    char* path ;
    switch(cmd->cmd){
        case SP_QUIT:
            ret = 1 ;
	        break ;
        case SP_RESTART:
            printf("Game restarted!\n") ;
            ret = 2 ;
	        break ;
        case SP_SAVE:
            path = cmd->filePath ;
            msg = spChessSave(game, cmd->filePath) ;
            if (msg==SP_CHESS_GAME_STANDART_ERROR){
                printError(STANDART_ERROR, "fwrite()") ;
                err = NO_ERROR ;
                ret = 5 ;
                break ;
            }
            if (msg==SP_CHESS_GAME_ILLEGAL_PATH){
                printError(ILLEGAL_PATH, NULL) ;
                err = NO_ERROR ;
                ret = 5 ;
                break ;

            }
            printf("Game saved to: %s\n", path) ;
            ret = 5 ;
            break ;
        case SP_UNDO_MOVE:
            err = undoMove(game) ;
            ret = 3 ;
	        break ;
        case SP_MOVE:
            if (executeMove(game, cmd->move)<0) {
                err = STANDART_ERROR ;
                break ;
            }
            free(cmd->move) ;
            ret = (game->gameMode==1) ? 4 : 3 ;
	        break ;
        case SP_GET_MOVES:
            err = getMoves(game, cmd->move->sourceRow, cmd->move->sourceColumn) ;
            free(cmd->move) ;
            ret = 5 ;
	        break ;
        default:
            break ;
    }
    if (err==STANDART_ERROR) {
        printError(err, "malloc()");
        ret = 5;
    }
    else if(err!=NO_ERROR){
        printError(err, NULL) ;
        ret = 5 ;
    }
    return ret ;
}


/**
 * executs undo_move command. assumes game is not NULL
 * @param game - game on which to execute move
 * @return UNDO_ERROR Error in case there is no history. NO_ERROR on success
 */
ERROR undoMove(SPChessGame* game){

    SP_CHESS_GAME_MESSAGE ret ;

    ret = spChessUndoPrevMove(game) ;
    if (ret==SP_CHESS_GAME_NO_HISTORY)
        return UNDO_ERROR ;

    spChessUndoPrevMove(game) ;

    return NO_ERROR ;
}


/**
 * suggests (prints) possible moves to the user
 * @param game - game instance to suggest a move for
 * @param maxDepth - recursion depth for minimax algorithm
 * @return STANDART_ERROR Error in case of allocation error during minimax run. NO_ERROR on success.
 */
ERROR getMoves(SPChessGame* game,int row, int col){

    char player = game->currentPlayer ;
    SPArrayList* moves = spChessGetMoves(game, row, col) ;
    if (moves==NULL)
        return STANDART_ERROR ;

    SPMove* move ;
    int listSize = spArrayListSize(moves) ;
    char* threatend ;
    char* eating ;

    for (int i=0;i<listSize;i++){
        move = spArrayListGetAt(moves, i) ;
        eating = (game->gameBoard[move->destRow][move->destColumn]!=SP_CHESS_GAME_EMPTY_ENTRY) ? "^" : "\0" ;
        testMove(game, move) ;
        threatend = (spChessIsThreatend(game, move->destRow, move->destColumn, player)==SP_CHESS_GAME_UNDER_THREAT) ? "*" : "\0" ;
        undoTestMove(game, move) ;
        printf("<%d,%c>%s%s\n", move->destRow+1, (char)move->destColumn+'A', threatend, eating) ;
    }

    spArrayListDestroy(moves) ;
    return NO_ERROR ;
}


/**
 * executes the computer's turn of the game
 * @param game - the game on which to play the turn
 * @param maxDepth - maximum recursion depth for minimax tree
 * @return 0 on success, -1 on malloc() failure
 */
int executeComputerTurn(SPChessGame* game){
    int maxDepth = game->difficulty ;
    SPMove* move = spMinimaxSuggestMove(game, maxDepth) ;
    if (move==NULL)
        return -1 ;
    char piece = game->gameBoard[move->sourceRow][move->sourceColumn] ;
    char* pieceName = getPieceString(piece) ;
    executeMove(game, move) ;
    printf("Computer: move %s at <%d,%c> to <%d,%c>\n",
           pieceName, move->sourceRow+1, (char)move->sourceColumn+'A', move->destRow+1, (char)move->destColumn+'A') ;

    return 0 ;
}

/**
 * prints the appropriate message when game is over (tie or there is a winner)
 * @param winner - the char of the winning player
 */
void finishGame(SPChessGame* game, SP_CHESS_GAME_MESSAGE result){

    if (result==SP_CHESS_GAME_DRAW)
        printf("The game ends in a draw\n") ;

    if  (result==SP_CHESS_GAME_CHECKMATE){
        char* winner = (game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL) ? "black" : "white" ;
        printf("Checkmate! %s player wins the game\n", winner) ;
    }
}


/**
 * returns the name of the difficulty level given by the parameter
 * @param difficulty an int representing a legal level
 * @return a string with the name of the level
 */
char* getDifficultyString(int difficulty){
    switch(difficulty){
        case 1:
            return "amateur" ;
        case 2:
            return "easy" ;
        case 3:
            return "moderate" ;
        case 4:
            return "hard" ;
        case 5:
            return "expert" ;
        default:
            return "amateur" ;
    }
}


/**
 * gets a char that represents a piece and returns it's name
 * @param piece a char representing a legal piece
 * @return a string representing the name of the piece
 */
char* getPieceString(char piece){
    if (piece=='m'||piece=='M')
        return "pawn" ;
    if (piece=='n'||piece=='N')
        return "knight" ;
    if (piece=='b'||piece=='B')
        return "bishop" ;
    if (piece=='r'||piece=='R')
        return "rook" ;
    if (piece=='q'||piece=='Q')
        return "queen" ;
    if (piece=='k'||piece=='K')
        return "king" ;
    return '\0' ;
}


/**
 * terminates the program
 */
void quit(){
    printf("Exiting...\n") ;
    exit(0) ;
}


