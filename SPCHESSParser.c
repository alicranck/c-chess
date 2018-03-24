//
// Created by user on 03/06/2017.
//
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "SPCHESSParser.h"

/**
 * Checks if a specified string represents a valid integer. It is recommended
 * to use this function prior to calling the standard library function atoi.
 *
 * @return
 * true if the string represents a valid integer, and false otherwise.
 */
bool spParserIsInt(const char* str){
    
    size_t size = strlen(str) ;
    bool flag = true ;
    if (size==0)
        flag = false ;
    for (unsigned int i=0;i<size;i++){
        if (str[i]=='-'&&i!=0)
            flag = false ;
        if (!isdigit(str[i])&&str[i]!='-')
            flag = false ;
    }
    return flag ;
}


/**
 * Parses a specified line. If the line is a command which has an integer
 * argument then the argument is parsed and is saved in the field arg and the
 * field validArg is set to true. In any other case then 'validArg' is set to
 * false and the value 'arg' is undefined
 *
 * @return
 * A parsed line such that:
 *   cmd - contains the command type, if the line is invalid then this field is
 *         set to INVALID_LINE
 *   validArg - is set to true if the command needs an argumant and it is valid
 *   arg      - the integer argument in case validArg is set to true
 *
 * NULL on allocation error
 */
SPCommand* spParserPraseLine(const char* str) {

    char cpy[1024];
    strcpy(cpy, str);
    SPCommand *ret = (SPCommand *) malloc(sizeof(SPCommand));
    if (ret==NULL){
        return NULL ;
    }

    ret->cmd = SP_INVALID_LINE;
    ret->validArg = false;
    char *token;
    token = strtok(cpy, " \r\t\n");

    if (token == NULL)
        ret->cmd = SP_INVALID_LINE;

    else if (!strcmp(token, "game_mode")) {
        token = strtok(NULL, " \r\t\n");

        if (token != NULL && spParserIsInt(token)) {
            ret->arg = atoi(token);
            ret->validArg = true;
            ret->cmd = SP_GAME_MODE;
        } else {
            ret->cmd = SP_GAME_MODE;
            ret->validArg = false;
        }
    } else if (!strcmp(token, "difficulty")) {
        token = strtok(NULL, " \r\t\n");

        if (token != NULL && spParserIsInt(token)) {
            ret->arg = atoi(token);
            ret->validArg = true;
            if (ret->arg<1||ret->arg>5)
                ret->validArg = false ;
            ret->cmd = SP_DIFFICULTY;
        } else {
            ret->cmd = SP_DIFFICULTY;
            ret->validArg = false;
        }
    } else if (!strcmp(token, "user_color")) {
        token = strtok(NULL, " \r\t\n");

        if (token != NULL && spParserIsInt(token)) {
            ret->arg = atoi(token);
            ret->validArg = true;
            if (ret->arg<0||ret->arg>1)
                ret->validArg = false ;
            ret->cmd = SP_USER_COLOR;
        } else {
            ret->cmd = SP_USER_COLOR;
            ret->validArg = false;
        }
    } else if (!strcmp(token, "load")) {
        token = strtok(NULL, " \r\t\n");
        ret->filePath = token;
        ret->validArg = true;
        ret->cmd = SP_LOAD;
    } else if (!strcmp(token, "save")) {
        token = strtok(NULL, " \r\t\n");

        ret->filePath = token;
        ret->validArg = true;
        ret->cmd = SP_SAVE;
    } else if (!strcmp(token, "undo"))
        ret->cmd = SP_UNDO_MOVE;

    else if (!strcmp(token, "default"))
        ret->cmd = SP_DEFAULT;

    else if (!strcmp(token, "print_settings"))
        ret->cmd = SP_PRINT_SETTING;

    else if (!strcmp(token, "move")) {
        token = strtok(NULL, "\n");
        int t = 0;
        if (token != NULL) {
            ret->cmd = SP_MOVE;
            SPMove* move = (SPMove*)malloc(sizeof(SPMove));
            char tmp1 ;
            char tmp2 ;
            if (move==NULL)
                return NULL;
            t = sscanf(token, " <%d,%c> to <%d,%c> \r\t\n",
                       &(move->sourceRow), &(tmp1), &(move->destRow), &(tmp2));
            move->sourceColumn = tolower(tmp1) - 'a' ;
            move->destColumn = tolower(tmp2) - 'a' ;
            move->sourceRow -= 1 ;
            move->destRow -= 1 ;
            printf("recieved: <%d,%d> to <%d,%d>\n",
                   (move->sourceRow), (move->sourceColumn), (move->destRow), (move->destColumn)) ;
            ret->move = move ;
        }
        if (t != 4) {
            ret->cmd = SP_INVALID_LINE;
            ret->validArg = false;
        }
    }
    else if (!strcmp(token, "get_moves")) {
        token = strtok(NULL, "\n");
        int t = 0;
        if (token != NULL) {
            ret->cmd = SP_GET_MOVES;
            SPMove* move = (SPMove*)malloc(sizeof(SPMove));
            char tmp ;
            if (move==NULL)
                return NULL;
            t = sscanf(token, " <%d,%c> \r\t\n", &(move->sourceRow), &(tmp));
            move->sourceColumn = tolower(tmp) - 'a' ;
            move->sourceRow -= 1 ;
            printf("recieved: <%d,%d>, t=%d\n", move->sourceRow, move->sourceColumn, t) ;
            ret->move = move ;
        }
        if (t != 2) {
            ret->cmd = SP_INVALID_LINE;
            ret->validArg = false;
        }
    }
    else if (!strcmp(token, "quit"))
        ret->cmd = SP_QUIT;

    else if (!strcmp(token, "start"))
        ret->cmd = SP_START;

    else if (!strcmp(token, "reset"))
        ret->cmd = SP_RESTART;

    return ret;
}