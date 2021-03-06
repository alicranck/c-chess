//
// Created by user on 3/10/2018.
//
#define _BSD_SOURCE
#include <ctype.h>
#include <unistd.h>
#include <SDL_messagebox.h>
#include "SPGUIGameScreen.h"




SP_GUI_MESSAGE drawGameWindow(SPChessGame* game){

    SP_GUI_MESSAGE ret ;
    ChessBoard* board ;
    Widget** buttons ;
    SDL_Rect* pieces[12] ;
    SDL_Texture* pieces_sprite ;
    bool saved = false ;

    Screen* screen = createScreen(1024, 768, "bmp/game/bg.bmp", "game") ;
    if (screen==NULL)
        return ERROR ;

    // create textures for game pieces from image
    ret = createPieceLocations(pieces) ;
    if(ret==ERROR){
        printf("ERROR: unable to create pieces textures: %s\n", SDL_GetError());
        destroyScreen(screen) ;
        return ERROR;
    }

    pieces_sprite = createPiecesSprite("bmp/game/pieces.bmp", screen->rend) ;
    if(pieces_sprite==NULL){
        printf("ERROR: unable to create pieces textures: %s\n", SDL_GetError());
        destroyPieceLocations(pieces) ;
        destroyScreen(screen) ;
        return ERROR;
    }

    // Create check indicators graphics
    SDL_Texture* indicator = createCheckIndicators(screen->rend) ;
    if (indicator == NULL) {
        printf("ERROR: unable to create check indicators: %s\n", SDL_GetError());
        SDL_DestroyTexture(pieces_sprite) ;
        destroyPieceLocations(pieces) ;
        destroyScreen(screen) ;
        return ERROR;
    }

    // Create buttons
    buttons = createGameButtons(screen->rend) ;
    if (buttons==NULL){
        SDL_DestroyTexture(pieces_sprite) ;
        destroyPieceLocations(pieces) ;
        destroyScreen(screen) ;
        printf("ERROR: unable to create game buttons: %s\n", SDL_GetError());
        return ERROR ;
    }

    // Create board
    board = createGUIChessGame(screen->rend, "bmp/game/brightSquare.bmp", "bmp/game/darkSquare.bmp", game, pieces) ;
    if (board==NULL){
        printf("ERROR: unable to create game board: %s\n", SDL_GetError());
        SDL_DestroyTexture(pieces_sprite) ;
        destroyPieceLocations(pieces) ;
        destroyButtons(buttons, GAME_NUM_BUTTONS) ;
        destroyScreen(screen) ;
        return ERROR ;
    }

    // Event loop
    while(1){
        // Disable undo button if no history
        Button* button ;
        button = (Button *) buttons[0]->data;
        if (game->history->actualSize==0)
            button->disabled = true;
        else
            button->disabled = false ;

        // Draw buttons
        for (int i=0; i<GAME_NUM_BUTTONS; i++){
            buttons[i]->draw(buttons[i], screen->rend, NULL) ;
        }

        // Draw indicators
        drawIndicators(screen->rend, indicator, board->game) ;

        // Draw board and present
        drawBoard(screen->rend, board, pieces_sprite) ;
        usleep(20) ;
        SDL_RenderPresent(screen->rend);

        SDL_Event e ;
        SDL_WaitEvent(&e) ;
        if (e.type==SDL_QUIT||e.key.keysym.sym==SDLK_ESCAPE){
            ret = QUIT ;
            break ;
        }

        // Seperately handle board and button events
        ret = handleBoardEvent(board, &e, &saved) ;
        if (ret==ERROR){
            printf("ERROR: error handling board event\n") ;
            break ;
        }

        // Buttons event handling
        for (int i=0; i<GAME_NUM_BUTTONS; i++){
            ret = buttons[i]->handleEvent(buttons[i], &e) ;
            if (ret==SAVE_GAME) {
                ret = drawSaveLoadWindow(game, true);
                if (ret==SAVED)
                    saved = true ;
            }
            if (ret==LOAD_GAME)
                ret = drawSaveLoadWindow(game, false) ;
            if (ret==UNDO_MOVE)
                ret = undoGUIMove(game) ;
            if ((ret==MAIN_MENU||ret==QUIT)&&saved==false) {
                ret = showSaveDialog(screen->window, game, ret, &saved);
            }
            if (ret==QUIT||ret==ERROR||ret==MAIN_MENU||ret==RESTART_GAME||ret==RELOAD_GAME)
                break ;
        }
        if (ret==QUIT||ret==ERROR||ret==MAIN_MENU||ret==RESTART_GAME)
            break ;
        if (ret==RELOAD_GAME){
            redrawBoard(board, game, pieces) ;
        }

        // redraw all graphics after last turn
        drawBoard(screen->rend, board, pieces_sprite) ;
        drawIndicators(screen->rend, indicator, board->game) ;
        usleep(20) ;
        SDL_RenderPresent(screen->rend);

        // check for mate/draw and finish game if necessary
        if (spChessCheckWinner(board->game)!=SP_CHESS_GAME_NO_WINNER){
            ret = finishGUIGame(screen->window, board->game) ;
            break ;
        }

        // Check for computer turn and execute it
        if ((board->game->gameMode==1)&&((board->game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL&&board->game->userColor==0)||
                                         (board->game->currentPlayer==SP_CHESS_GAME_BLACK_SYMBOL&&board->game->userColor==1))){
            int maxDepth = board->game->difficulty ;
            SPMove* move = spMinimaxSuggestMove(board->game, maxDepth) ;
            if (move==NULL)
                return ERROR ;
            executeGUIMove(board, move, &saved) ;
            if (spChessCheckWinner(board->game)!=SP_CHESS_GAME_NO_WINNER){
                ret = finishGUIGame(screen->window, board->game) ;
                break ;
            }
            continue ;
        }
    }

    // free all memory and return
    SDL_DestroyTexture(pieces_sprite) ;
    destroyGUIGame(board) ;
    destroyPieceLocations(pieces) ;
    destroyButtons(buttons, GAME_NUM_BUTTONS) ;
    destroyScreen(screen) ;

    return ret ;
}


/**
 * create an instance of a ChessBoard struct.
 * @param rend SDL_Renderer to create textures
 * @param brightSquareImg path to image of a dark chess square
 * @param darkSquareImg path to image of a bright chess square
 * @param game a pointer to an SPChessGame instance to be associated with this board
 * @return a ChessBoard pointer on sucess. NULL on SDL or allocation error
 */
ChessBoard* createGUIChessGame(SDL_Renderer* rend, char* brightSquareImg, char* darkSquareImg,
                               SPChessGame* game, SDL_Rect** pieces){
    ChessBoard* board = (ChessBoard*)malloc(sizeof(ChessBoard)) ;
    SDL_Rect* location = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (board==NULL||game==NULL||location==NULL){
        return NULL ;
    }

    for (int i=0;i<SP_CHESS_GAME_N_ROWS;i++){
        for (int j=0;j<SP_CHESS_GAME_N_COLUMNS;j++){
            SDL_Rect* loc = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
            if (loc==NULL)
                return NULL ;
            loc->x = BOARD_X + j*SQUARE_SIDE ;
            loc->y = BOARD_Y + i*SQUARE_SIDE ;
            loc->w = SQUARE_SIDE ;
            loc->h = SQUARE_SIDE ;
            if ((i+j)%2==0)
                board->squares[i][j] = createChessSquare(rend, brightSquareImg, loc,
                                                         getPieceLocation(game->gameBoard[7 - i][j], pieces)) ;
            else
                board->squares[i][j] = createChessSquare(rend, darkSquareImg, loc,
                                                         getPieceLocation(game->gameBoard[7 - i][j], pieces)) ;
            if (board->squares[i][j]==NULL)
                return NULL ;

        }
    }
    
    location->x = BOARD_X ;
    location->y = BOARD_Y ;
    location->h = SQUARE_SIDE*SP_CHESS_GAME_N_ROWS ;
    location->w = SQUARE_SIDE*SP_CHESS_GAME_N_COLUMNS ;
    
    board->location = location ;
    board->game = game ;
    board->pressed = false ;

    return board ;
}


/**
 *  Create game screen buttons
 * @param rend an SDL_Renderer for game window
 * @return an array of GAME_NUM_BUTTONS Widgets containing the buttons.  NULL on allocation error
 */
Widget** createGameButtons(SDL_Renderer* rend){
    Widget** buttons = (Widget**)malloc(sizeof(Widget*)*GAME_NUM_BUTTONS) ;
    if (buttons==NULL)
        return NULL ;

    // Undo button
    SDL_Rect* undoRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (undoRect==NULL)
        return NULL ;
    undoRect->x = GAME_X ;
    undoRect->y = UNDO_Y ;
    undoRect->w = GAME_BUTTON_WIDTH ;
    undoRect->h = GAME_BUTTON_HEIGHT ;
    buttons[0] = createButton(rend, "bmp/game/undo.bmp", "bmp/game/undoHL.bmp",
                              "bmp/game/undoP.bmp", undoRect, &undoAction) ;

    // Save button
    SDL_Rect* saveRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (saveRect==NULL)
        return NULL ;
    saveRect->x = GAME_X ;
    saveRect->y = GAME_Y ;
    saveRect->w = GAME_BUTTON_WIDTH ;
    saveRect->h = GAME_BUTTON_HEIGHT ;
    buttons[1] = createButton(rend, "bmp/game/save.bmp","bmp/game/saveHL.bmp",
                              "bmp/game/saveP.bmp", saveRect, &saveAction) ;

    // Load button
    SDL_Rect* loadRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (loadRect==NULL)
        return NULL ;
    loadRect->x = GAME_X ;
    loadRect->y = GAME_Y + GAME_VERTICAL_DIFF ;
    loadRect->w = GAME_BUTTON_WIDTH ;
    loadRect->h = GAME_BUTTON_HEIGHT ;
    buttons[2] = createButton(rend, "bmp/game/loadGame.bmp","bmp/game/loadGameHL.bmp",
                              "bmp/game/loadGameP.bmp", loadRect, &loadAction) ;

    // restart button
    SDL_Rect* restartRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (restartRect==NULL)
        return NULL ;
    restartRect->x = GAME_X ;
    restartRect->y = GAME_Y + 2*GAME_VERTICAL_DIFF ;
    restartRect->w = GAME_BUTTON_WIDTH ;
    restartRect->h = GAME_BUTTON_HEIGHT ;
    buttons[3] = createButton(rend, "bmp/game/restart.bmp","bmp/game/restartHL.bmp",
                              "bmp/game/restartP.bmp", restartRect, &restartAction) ;

    // Main menu button
    SDL_Rect* mainRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (mainRect==NULL)
        return NULL ;
    mainRect->x = GAME_X ;
    mainRect->y = GAME_Y + 3*GAME_VERTICAL_DIFF ;
    mainRect->w = GAME_BUTTON_WIDTH ;
    mainRect->h = GAME_BUTTON_HEIGHT ;
    buttons[4] = createButton(rend, "bmp/game/main.bmp","bmp/game/mainHL.bmp",
                              "bmp/game/mainP.bmp", mainRect, &mainAction) ;
    
    // quit button
    SDL_Rect* quitRect = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (quitRect==NULL)
        return NULL ;
    quitRect->x = GAME_X ;
    quitRect->y = GAME_Y + 4*GAME_VERTICAL_DIFF ;
    quitRect->w = GAME_BUTTON_WIDTH ;
    quitRect->h = GAME_BUTTON_HEIGHT ;
    buttons[5] = createButton(rend, "bmp/game/quit.bmp","bmp/game/quitHL.bmp",
                              "bmp/game/quitP.bmp", quitRect, &gameQuitAction) ;

    // Check that all buttons were created successfully
    for (int i=0;i<GAME_NUM_BUTTONS;i++){
        if (buttons[i]==NULL)
            return NULL ;
    }

    return buttons ;
}


/**
 * create check indicators texture
 * @param rend an SDL_Renderer pointer associated with this window
 * @return a pointer to the created texture. NULL on SDL or allocation error
 */
SDL_Texture* createCheckIndicators(SDL_Renderer* rend){

    SDL_Surface* surface = SDL_LoadBMP("bmp/game/checkIndicator.bmp") ;
    if (surface==NULL){
        printf("ERROR: unable to load image: %s\n", SDL_GetError());
        return NULL ;
    }
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 255, 255));
    SDL_Texture* texture = SDL_CreateTextureFromSurface(rend, surface) ;
    if (texture==NULL){
        SDL_FreeSurface(surface) ;
        printf("ERROR: unable to create texture from image: %s\n", SDL_GetError());
        return NULL ;
    }
    SDL_FreeSurface(surface) ;
    SDL_SetTextureColorMod(texture, 50, 50, 50) ;

    return texture ;
}


/**
 * draw check indicators texture
 * @param rend an SDL_Renderer pointer associated with this window
 * @param tex the texture to draw
 * @param game an SPChessGame pointer to check for check
 */
void drawIndicators(SDL_Renderer* rend, SDL_Texture* tex, SPChessGame* game){

    int isCheck = (spChessIsCheck(game, game->currentPlayer)==SP_CHESS_GAME_UNDER_THREAT) ;
    int activeIndicatorY = (game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL) ? WHITE_INDICATOR_Y : BLACK_INDICATOR_Y ;

    SDL_Rect whiteRect = {.x = INDICATOR_X, .y = WHITE_INDICATOR_Y, .h = INDICATOR_SIDE, .w = INDICATOR_SIDE};
    SDL_RenderCopy(rend, tex, NULL , &whiteRect);

    SDL_Rect blackRect = {.x = INDICATOR_X, .y = BLACK_INDICATOR_Y, .h = INDICATOR_SIDE, .w = INDICATOR_SIDE};
    SDL_RenderCopy(rend, tex, NULL , &blackRect);

    if (isCheck){
        SDL_SetTextureColorMod(tex, 255, 255, 255) ;
        SDL_Rect activeRect = {.x = INDICATOR_X, .y = activeIndicatorY, .h = INDICATOR_SIDE, .w = INDICATOR_SIDE};
        SDL_RenderCopy(rend, tex, NULL, &activeRect);
    }
    SDL_SetTextureColorMod(tex, 50, 50, 50) ;
}


/**
 * draw a game board to screen
 * @param rend an SDL_Renderer pointer associated with this window
 * @param board a ChessBoard pointer for the board to draw
 */
void drawBoard(SDL_Renderer* rend, ChessBoard* board, SDL_Texture* sprite){
    for (int i=0;i<SP_CHESS_GAME_N_ROWS;i++) {
        for (int j = 0; j < SP_CHESS_GAME_N_COLUMNS; j++) {
            board->squares[i][j]->draw(board->squares[i][j], rend, sprite) ;
        }
    }
}


/**
 * reassign pieces textures to squares according to a given game state
 * @param board the board on which to reassign the pieces
 * @param game the game state to follow
 */
void redrawBoard(ChessBoard* board, SPChessGame* game, SDL_Rect** pieces){
    ChessSquare* square ;
    for (int i=0;i<SP_CHESS_GAME_N_ROWS;i++){
        for (int j=0;j<SP_CHESS_GAME_N_COLUMNS;j++){
            square = (ChessSquare*)board->squares[i][j]->data ;
            square->changed = true ;
            square->piece = getPieceLocation(game->gameBoard[7 - i][j], pieces) ;
        }
    }
}


/**
 * create an SDL_texture with the game pieces
 * @param path the path to the pieces image
 * @param rend an SDL_Renderer
 * @return an SDL_texture of the pieces
 */
SDL_Texture* createPiecesSprite(char* path, SDL_Renderer* rend){

    // load an image onto an SDL surface
    SDL_Surface* surf = SDL_LoadBMP(path);
    if (surf == NULL) {
        printf("ERROR: unable to load image: %s\n", SDL_GetError());
        return NULL;
    }

    // set a specific color (white: 255,255,255) to transparent
    SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 255, 255, 255));

    // create a texture from the surface image
    SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surf);
    if (tex == NULL) {
        printf("ERROR: unable to create texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surf);
        return NULL ;
    }

    // surface not needed anymore - free it
    SDL_FreeSurface(surf);

    return tex ;
}


/**
 * create game piece textures from image
 * @param piecesImg a path to the pieces sprite
 * @param rend
 * @return SP_GUI_MESSAGE NONE on success, ERROR on SDL or allocation error
 */
SP_GUI_MESSAGE createPieceLocations(SDL_Rect** pieces){
    for (int i=0;i<12;i++){
        SDL_Rect* rect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
        if (rect==NULL)
            return  ERROR ;
        rect->x = (PIECE_WIDTH)*(i%6) ;
        rect->y = PIECE_HEIGHT*(i/6) ;
        rect->h = PIECE_HEIGHT ;
        rect->w = PIECE_WIDTH ;

        pieces[i] = rect ;
    }
    return NONE ;
}


/**
 * returns the location of the given piece
 * @param piece a char representing the piece
 * @return SDL_Rect* to the requested texture
 */
SDL_Rect* getPieceLocation(char piece, SDL_Rect **pieces){

    int black = (isupper(piece)) ? 1 : 0 ;
    int index = 0 ;

    switch (tolower(piece)){
        case 'k':
            index = 0 ;
            break ;
        case 'q':
            index = 1 ;
            break ;
        case 'r':
            index = 2 ;
            break ;
        case 'n':
            index = 3 ;
            break ;
        case 'b':
            index = 4 ;
            break ;
        case 'm':
            index = 5 ;
            break ;
        default:
            return NULL ;
    }
    return pieces[index + 6*black] ;
}


/**
 * a function to handle all mouse events inside the chess board perimeter, i.e - highlight possible moves
 * on right click, selecting a piece, or executing a move.
 * @param board The board in which to handle the event
 * @param e the event
 * @return SP_GUI_MESSAGE NONE on success or ERROR on allocation error
 */
SP_GUI_MESSAGE handleBoardEvent(ChessBoard* board, SDL_Event* e, bool* saved){
    // Check if the event is mouse event, and if it's in the board
    if (e->type!=SDL_MOUSEMOTION&&e->type!=SDL_MOUSEBUTTONDOWN&&e->type!=SDL_MOUSEBUTTONUP)
        return NONE ;

    SP_GUI_MESSAGE ret ;
    SDL_Point p = {0, 0};
    SDL_GetMouseState(&p.x, &p.y) ;
    int squareCol = (p.x - BOARD_X)/SQUARE_SIDE ;
    int squareRow = (p.y - BOARD_Y)/SQUARE_SIDE ;

    ChessSquare* square ;
    char piece ;

    if (e->type==SDL_MOUSEMOTION){
        for (int i=0;i<SP_CHESS_GAME_N_ROWS;i++){
            for (int j=0;j<SP_CHESS_GAME_N_COLUMNS;j++) {
                square = (ChessSquare*)board->squares[i][j]->data ;
                if (SDL_PointInRect(&p, square->location)&&!square->hover&&square->piece!=NULL){
                    square->hover = true ;
                    square->changed = true ;
                }
                if (!SDL_PointInRect(&p, square->location)&&square->hover){
                    square->hover = false ;
                    square->changed = true ;
                }
            }
        }
    }

    if (e->type==SDL_MOUSEBUTTONDOWN&&e->button.button==SDL_BUTTON_RIGHT){
        ret = colorPossibleMoves(board, squareRow, squareCol) ;
        return ret ;
    }

    if(e->type==SDL_MOUSEBUTTONUP){
        // if click is out of board clear highlighted buttons and return
        if (!SDL_PointInRect(&p, board->location)||e->button.button==SDL_BUTTON_RIGHT){
            clearBoard(board) ;
            return NONE ;
        }
        square = (ChessSquare*)board->squares[squareRow][squareCol]->data ;
        piece = board->game->gameBoard[7-squareRow][squareCol] ;

        // if there is a chosen square on the board, try executing the move
        if (board->pressed){
            SPMove* move = (SPMove*)malloc(sizeof(SPMove)) ;
            if (move==NULL){
                printf("ERROR: allocation error on board event handling\n");
                return ERROR ;
            }
            move->sourceRow = 7 - board->pressedLocation[0] ;
            move->sourceColumn = board->pressedLocation[1] ;
            move->destRow = 7 - squareRow ;
            move->destColumn = squareCol ;
            if(executeGUIMove(board, move, saved)==NONE){
                clearBoard(board) ;
                free(move) ;
                return NONE ;
            }
        }

        clearBoard(board) ;
        if (piece!=SP_CHESS_GAME_EMPTY_ENTRY){
            board->pressed = true ;
            board->pressedLocation[0] = squareRow ;
            board->pressedLocation[1] = squareCol ;
            square->pressed = true ;
        }
    }
    return NONE ;
}


/**
 * undo last two moves (if possible). uses 'spChessUndoPrevMove' from 'SPCHESSGame.c'
 * @param game a game state to update
 * @return SP_GUI_MESSAGE NONE if no history, RELOAD_GAME on successful undo
 */
SP_GUI_MESSAGE undoGUIMove(SPChessGame* game){

    SP_CHESS_GAME_MESSAGE ret ;
    ret = spChessUndoPrevMove(game) ;
    if (ret==SP_CHESS_GAME_NO_HISTORY)
        return NONE ;

    spChessUndoPrevMove(game) ;
    return RELOAD_GAME ;
}


/**
 * clears all extra graphics (highlighting etc.) from the game squares
 * @param board
 */
void clearBoard(ChessBoard* board){
    ChessSquare* square ;
    for (int i=0;i<SP_CHESS_GAME_N_ROWS;i++){
        for (int j=0;j<SP_CHESS_GAME_N_COLUMNS;j++) {
            square = (ChessSquare*)board->squares[i][j]->data ;
            square->pressed = false ;
            square->highlighted = false ;
            square->threatend = false ;
            square->capture = false ;
            square->changed = true ;
        }
    }
    board->pressed = false ;
}


/**
 * execute a given move on the board and the game state
 * @param board a board instance on which to execute the move
 * @param move the move to execute
 * @return SP_GUI_MESSAGE NONE on success. ERROR in case of illegal move
 */
SP_GUI_MESSAGE executeGUIMove(ChessBoard* board, SPMove* move, bool* saved){

    if (spChessCheckLegalMove(board->game, move, board->game->currentPlayer)!=SP_CHESS_GAME_LEGAL_MOVE)
        return ERROR ;

    ChessSquare* dest = (ChessSquare*)board->squares[7-move->destRow][move->destColumn]->data ;
    ChessSquare* src = (ChessSquare*)board->squares[7-move->sourceRow][move->sourceColumn]->data ;

    executeMove(board->game, move) ;

    // move piece texture from source to dest square
    dest->piece = src->piece ;
    src->piece = NULL ;

    dest->changed = true ;
    src->changed = true ;

    *saved = false ;

    return NONE ;
}


/**
 * color possible moves according to suggested scheme: legal - green
 *                                                     capture - blue
 *                                                     threatend - red
 * @param board
 * @param row row of the position for which to check possible moves
 * @param col
 * @return ERROR on allocation error, NONE on success
 */
SP_GUI_MESSAGE colorPossibleMoves(ChessBoard* board, int row, int col){

    SPArrayList* moves = spChessGetMoves(board->game, 7 - row, col, board->game->currentPlayer) ;
    if (moves==NULL)
        return ERROR ;

    // check if the piece belongs to the current player
    char piece = board->game->gameBoard[7-row][col] ;
    char player = board->game->currentPlayer ;
    if ((board->game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL&&isupper(piece))||
            (board->game->currentPlayer==SP_CHESS_GAME_BLACK_SYMBOL&&islower(piece)))
        return NONE ;


    SPMove* move ;
    ChessSquare* square ;

    for (int i=0;i<moves->actualSize; i++){
        move = spArrayListGetAt(moves, i) ;
        square = (ChessSquare*)board->squares[7-move->destRow][move->destColumn]->data ;
        square->highlighted = true ;
        square->changed = true ;

        if (square->piece!=NULL)
            square->capture = true ;

        testMove(board->game, move) ;
        if (spChessIsThreatend(board->game, move->destRow, move->destColumn, player)==SP_CHESS_GAME_UNDER_THREAT)
            square->threatend = true ;
        undoTestMove(board->game, move) ;
    }

    return NONE ;
}


/**
 * in case of quit/main_menu, present a dialog box for the user to save
 * @param window
 * @param game
 * @param ret - the command to perform next (quit/main)
 * @return ERROR on SDL error. QUIT/RESTART_GAME/MAIN_MENU according to user selection otherwise
 */
SP_GUI_MESSAGE showSaveDialog(SDL_Window* window, SPChessGame* game, SP_GUI_MESSAGE ret, bool* saved){
    const SDL_MessageBoxButtonData buttons[] = {
            { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Yes" },
            { 0, 1, "No" },
            { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "Cancel" },
    };
    const SDL_MessageBoxColorScheme colorScheme = {
            { /* .colors (.r, .g, .b) */
                    /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
                    { 255,   0,   0 },
                    /* [SDL_MESSAGEBOX_COLOR_TEXT] */
                    {   0, 255,   0 },
                    /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
                    { 255, 255,   0 },
                    /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
                    {   0,   0, 255 },
                    /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
                    { 255,   0, 255 }
            }
    };
    SDL_MessageBoxData messageboxdata = {
            SDL_MESSAGEBOX_INFORMATION, /* .flags */
            window, /* .window */
            "Save Changes", /* .title */
            "", /* .message */
            SDL_arraysize(buttons), /* .numbuttons */
            buttons, /* .buttons */
            &colorScheme /* .colorScheme */
    };


    messageboxdata.message = "Would you like to save the current game?" ;


    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
        printf("error displaying message box\n");
        return ERROR;
    }
    if (buttonid==-1||buttonid==1)
        return ret ;
    if (buttonid==0) {
        if (drawSaveLoadWindow(game, true)==SAVED) {
            *saved = true;
            return ret;
        }
    }
    if (buttonid==2)
        return NONE ;

    return NONE;
}


/**
 * in case of mate/draw, present a dialog box for the user to determine next step
 * @param window
 * @param game
 * @return ERROR on SDL error. QUIT/RESTART_GAME/MAIN_MENU according to user selection otherwise
 */
SP_GUI_MESSAGE finishGUIGame(SDL_Window* window, SPChessGame* game){
    const SDL_MessageBoxButtonData buttons[] = {
            { /* .flags, .buttonid, .text */        0, 0, "Restart" },
            { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Main Menu" },
            { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "Quit" },
    };
    const SDL_MessageBoxColorScheme colorScheme = {
            { /* .colors (.r, .g, .b) */
                    /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
                    { 255,   0,   0 },
                    /* [SDL_MESSAGEBOX_COLOR_TEXT] */
                    {   0, 255,   0 },
                    /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
                    { 255, 255,   0 },
                    /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
                    {   0,   0, 255 },
                    /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
                    { 255,   0, 255 }
            }
    };
    SDL_MessageBoxData messageboxdata = {
            SDL_MESSAGEBOX_INFORMATION, /* .flags */
            window, /* .window */
            "GAME OVER", /* .title */
            "", /* .message */
            SDL_arraysize(buttons), /* .numbuttons */
            buttons, /* .buttons */
            &colorScheme /* .colorScheme */
    };

    if (spChessCheckWinner(game)==SP_CHESS_GAME_CHECKMATE)
        messageboxdata.message = "Checkmate, Please choose what to do next" ;
    else
        messageboxdata.message = "Draw, Please choose what to do next" ;

    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
        printf("error displaying message box\n");
        return ERROR;
    }
    if (buttonid==-1||buttonid==2)
        return QUIT ;
    if (buttonid==0)
        return RESTART_GAME ;
    if (buttonid==1)
        return MAIN_MENU ;

    return QUIT;
}


void destroyGUIGame(ChessBoard* board){

    for (int i=0;i<SP_CHESS_GAME_N_ROWS;i++) {
        for (int j = 0; j < SP_CHESS_GAME_N_COLUMNS; j++) {
            board->squares[i][j]->destroy(board->squares[i][j]) ;
            free(board->squares[i][j]) ;
        }
    }
    spChessDestroy(board->game) ;
    free(board->location) ;
    free(board) ;
}


void destroyPieceLocations(SDL_Rect** pieces){
    for (int i=0;i<12;i++){
        free(pieces[i]) ;
    }
}


void destroyButtons(Widget** buttons, int n){
    for (int i=0; i<n; i++){
        buttons[i]->destroy(buttons[i]) ;
        free(buttons[i]) ;
    }
}


SP_GUI_MESSAGE undoAction(){
    return UNDO_MOVE ;
}

SP_GUI_MESSAGE restartAction(){
    return RESTART_GAME ;
}

SP_GUI_MESSAGE mainAction(){
    return MAIN_MENU ;
}

SP_GUI_MESSAGE gameQuitAction(){
    return QUIT ;
}

SP_GUI_MESSAGE saveAction(){
    return SAVE_GAME ;
}