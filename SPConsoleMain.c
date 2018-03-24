//
// Created by user on 6/24/2017.
//
#include "SPMainAux.h"
//#include <SDL.h>

int consoleMain(){
    //----------------------------------------Setting Mode--------------------------------------------
    while(true){
        bool print_flag = false ;
        int ret = 0 ;
        int currentColor ;
	    SP_CHESS_GAME_MESSAGE result ;
	    SPCommand *cmd  ;
        SPChessGame* game = startNewGame(); //Assumes game is no NULL. allocation errors are handled in the function
        //----------------------------------------Game Loop--------------------------------------------------
        spChessPrintBoard(game);
        result = SP_CHESS_GAME_NO_WINNER ;
        while (result == SP_CHESS_GAME_NO_WINNER) {
            currentColor = (game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL) ? 1 : 0 ;
            if (game->gameMode==2||game->userColor==currentColor) {
                if (print_flag)
                    spChessPrintBoard(game);
                cmd = getMoveFromUser(
                        game); //Assumes cmd is not NULL. allocation and IO errors are handled in the function
                ret = executeUserCommand(cmd, game); //ret options - (1=quit, 2=restart, 3=print status and repeat user turn,
                // 4=execute computer turn, 5=wait for further commands from user)
                free(cmd);
            }
            if (ret==1){
		        spChessDestroy(game);
                quit() ;
	        }
            if (ret==2) {
		        spChessDestroy(game);
                break;
            }
            if (ret==3){
                print_flag = true ;
                continue ;
            }

            if (ret==5){
                print_flag = false ;
                continue ;
            }

	        result = spChessCheckWinner(game) ;
            if (result!=SP_CHESS_GAME_NO_WINNER){
                finishGame(game, result) ;
                spChessDestroy(game);
                quit() ;
            }

            if (spChessIsCheck(game, game->currentPlayer)==SP_CHESS_GAME_UNDER_THREAT){
                char* player = (game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL) ? "white" : "black" ;
                printf("Check: %s king is threatened\n", player) ;
            }

            ret = executeComputerTurn(game) ;
            print_flag = true ;
            if (ret<0){
                spChessDestroy(game);
                quit() ;
	        }

            result = spChessCheckWinner(game) ;
            if (result!=SP_CHESS_GAME_NO_WINNER){
                finishGame(game, result) ;
                spChessDestroy(game);
                quit() ;
            }

            if (spChessIsCheck(game, game->currentPlayer)==SP_CHESS_GAME_UNDER_THREAT){
                char* player = (game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL) ? "white" : "black" ;
                printf("Check: %s king is threatened\n", player) ;
            }
        }
    }
    return 0 ;
}

