#ifndef SPCHESSPARSER_H_
#define SPCHESSPARSER_H_
#include <stdbool.h>
#include "SPCHESSGame.h"


//a type used to represent a command
typedef enum {
	SP_UNDO_MOVE,
    SP_SAVE,
    SP_MOVE,
	SP_GET_MOVES,
	SP_QUIT,
	SP_RESTART,
	SP_INVALID_LINE,
	SP_GAME_MODE,
    SP_DIFFICULTY,
    SP_USER_COLOR,
    SP_LOAD,
    SP_DEFAULT,
    SP_PRINT_SETTING,
    SP_START,
} SP_COMMAND;

//a new type that is used to encapsulate a parsed line
typedef struct command_t {
	SP_COMMAND cmd;
	bool validArg; //is set to true if the line contains a valid argument
	int arg;
    char* filePath ;
    SPMove* move;
} SPCommand;

/**
 * Checks if a specified string represents a valid integer. It is recommended
 * to use this function prior to calling the standard library function atoi.
 *
 * @return
 * true if the string represents a valid integer, and false otherwise.
 */
bool spParserIsInt(const char* str);

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
 *   validArg - is set to true if the command is add_disc and the integer argument
 *              is valid
 *   arg      - the integer argument in case validArg is set to true
 */
SPCommand* spParserPraseLine(const char* str);

#endif
