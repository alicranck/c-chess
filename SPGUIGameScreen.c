//
// Created by user on 3/10/2018.
//

#include <ctype.h>
#include "SPGUIGameScreen.h"


SDL_Texture* pieces[12] ;


SP_GUI_MESSAGE drawGameWindow(SPChessGame* game){

    SP_GUI_MESSAGE ret ;
    ChessBoard* board ;
    Widget** buttons ;

    // create main SDL window
    SDL_Window *window = SDL_CreateWindow(
            "SPChess",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1270,
            768,
            SDL_WINDOW_OPENGL);

    // make sure window was created successfully
    if (window == NULL) {
        printf("ERROR: unable to create window: %s\n", SDL_GetError());
        SDL_Quit();
        return ERROR;
    }

    // create a renderer for the window
    SDL_Renderer *rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (rend == NULL) {
        printf("ERROR: unable to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return ERROR;
    }

    // ensure renderer supports transparency
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

    // set background
    SDL_Surface* surface = SDL_LoadBMP("bmp/game/bg.bmp") ;
    if (surface==NULL){
        printf("ERROR: unable to load image: %s\n", SDL_GetError());
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        return ERROR ;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(rend, surface) ;
    if (texture==NULL){
        printf("ERROR: unable to create texture from image: %s\n", SDL_GetError());
        SDL_FreeSurface(surface) ;
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        return ERROR ;
    }
    SDL_FreeSurface(surface) ;
    SDL_RenderCopy(rend, texture, NULL, NULL);

    // create piece textures from image
    ret = createPieceTextures("bmp/game/pieces.bmp", rend) ;
    if (ret!=NONE){
        printf("ERROR: unable to create piece textures: %s\n", SDL_GetError());
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        return ERROR ;
    }

    // Create buttons
    buttons = createGameButtons(rend) ;
    if (buttons==NULL){
        destroyPieceTextures() ;
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        printf("ERROR: unable to create buttons: %s\n", SDL_GetError());
        return ERROR ;
    }


    // Create board
    board = createGUIChessGame(rend, "bmp/game/brightSquare.bmp", "bmp/game/darkSquare.bmp", game) ;
    if (board==NULL){
        printf("ERROR: unable to create board: %s\n", SDL_GetError());
        destroyPieceTextures() ;
        destroyButtons(buttons) ;
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
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
            buttons[i]->draw(buttons[i], rend) ;
        }

        // Draw board and present
        drawBoard(rend, board) ;
        SDL_RenderPresent(rend);

        SDL_Event e ;
        SDL_WaitEvent(&e) ;
        if (e.type==SDL_QUIT||e.key.keysym.sym==SDLK_ESCAPE){
            ret = QUIT ;
            break ;
        }

        // Seperately handle board and button events
        ret = handleBoardEvent(board, &e) ;
        if (ret==ERROR){
            printf("ERROR: error handling board event\n") ;
            break ;
        }

        // Buttons event handling
        for (int i=0; i<GAME_NUM_BUTTONS; i++){
            ret = buttons[i]->handleEvent(buttons[i], &e) ;
            if (ret==SAVE_GAME)
                ret = drawSaveLoadWindow(game, true) ;
            if (ret==LOAD_GAME)
                ret = drawSaveLoadWindow(game, false) ;
            if (ret==UNDO_MOVE)
                ret = undoGUIMove(game) ;
            if (ret==QUIT||ret==ERROR||ret==MAIN_MENU||ret==START_NEW_GAME||ret==RELOAD_GAME)
                break ;
        }
        if (ret==QUIT||ret==ERROR||ret==MAIN_MENU||ret==START_NEW_GAME)
            break ;
        if (ret==RELOAD_GAME){
            redrawBoard(board, game) ;
        }
        drawBoard(rend, board) ;
        SDL_RenderPresent(rend);

        // Check for computer turn and execute it
        if ((board->game->gameMode==1)&&((board->game->currentPlayer==SP_CHESS_GAME_WHITE_SYMBOL&&board->game->userColor==0)||
                                         (board->game->currentPlayer==SP_CHESS_GAME_BLACK_SYMBOL&&board->game->userColor==1))){
            int maxDepth = board->game->difficulty ;
            SPMove* move = spMinimaxSuggestMove(board->game, maxDepth) ;
            if (move==NULL)
                return ERROR ;
            executeGUIMove(board, move) ;
            continue ;
        }
    }

    destroyGUIGame(board) ;
    destroyPieceTextures() ;
    destroyButtons(buttons) ;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);

    return ret ;
}


ChessBoard* createGUIChessGame(SDL_Renderer* rend, char* brightSquareImg, char* darkSquareImg,
                               SPChessGame* game){
    
    ChessBoard* board = (ChessBoard*)malloc(sizeof(ChessBoard)) ;
    SDL_Rect* location = (SDL_Rect*)malloc(sizeof(SDL_Rect)) ;
    if (board==NULL||game==NULL||location==NULL)
        return NULL ;
    
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
                board->squares[i][j] = createChessSquare(rend, brightSquareImg, loc, getPieceTex(game->gameBoard[7-i][j])) ;
            else
                board->squares[i][j] = createChessSquare(rend, darkSquareImg, loc, getPieceTex(game->gameBoard[7-i][j])) ;
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


void redrawBoard(ChessBoard* board, SPChessGame* game){
    ChessSquare* square ;
    for (int i=0;i<SP_CHESS_GAME_N_ROWS;i++){
        for (int j=0;j<SP_CHESS_GAME_N_COLUMNS;j++){
            square = (ChessSquare*)board->squares[i][j]->data ;
            square->piece = getPieceTex(game->gameBoard[7-i][j]) ;
        }
    }
}


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


void drawBoard(SDL_Renderer* rend, ChessBoard* board){
    for (int i=0;i<SP_CHESS_GAME_N_ROWS;i++) {
        for (int j = 0; j < SP_CHESS_GAME_N_COLUMNS; j++) {
            board->squares[i][j]->draw(board->squares[i][j], rend) ;
        }
    }
}


SP_GUI_MESSAGE createPieceTextures(char* piecesImg, SDL_Renderer* rend){

    SDL_Rect* rect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
    if (rect==NULL)
        return  ERROR ;
    // load an image onto an SDL surface
    SDL_Surface* surf = SDL_LoadBMP(piecesImg);
    if (surf == NULL) {
        printf("ERROR: unable to load image: %s\n", SDL_GetError());
        free(rect) ;
        return ERROR;
    }
    // set a specific color (white: 255,255,255) to transparent
    SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 255, 255, 255));
    // create a texture from the surface image
    SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surf);
    if (tex == NULL) {
        printf("ERROR: unable to create texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surf);
        free(rect) ;
        return ERROR ;
    }

    // surface not needed anymore - free it
    SDL_FreeSurface(surf);

    for (int i=0;i<12;i++){
        rect->x = PIECE_WIDTH*(i%6) ;
        rect->y = PIECE_HEIGHT*(i/6) ;
        rect->h = PIECE_HEIGHT ;
        rect->w = PIECE_WIDTH ;
        pieces[i] = SDL_CreateTexture(rend,SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, SQUARE_SIDE, SQUARE_SIDE) ;
        SDL_SetTextureBlendMode(pieces[i], SDL_BLENDMODE_BLEND);
        if (pieces[i] == NULL) {
            printf("ERROR: unable to create piece texture: %s\n", SDL_GetError());
            free(rect) ;
            return ERROR;
        }

        SDL_SetRenderTarget(rend, pieces[i]);
        SDL_RenderCopy(rend, tex, rect, NULL);
    }
    free(rect) ;
    SDL_SetRenderTarget(rend, NULL);
    return NONE ;
}


SDL_Texture* getPieceTex(char piece){

    int white ;
    int index ;

    if (isupper(piece))
        white = 1 ;
    else
        white = 0 ;

    switch (tolower(piece)){
        case 'r':
            index = 0 ;
            break ;
        case 'b':
            index = 1 ;
            break ;
        case 'q':
            index = 2 ;
            break ;
        case 'k':
            index = 3 ;
            break ;
        case 'n':
            index = 4 ;
            break ;
        case 'm':
            index = 5 ;
            break ;
        default:
            return NULL ;
    }

    index += white*6 ;
    return pieces[index] ;
}



SP_GUI_MESSAGE handleBoardEvent(ChessBoard* board, SDL_Event* e){
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
                }
                if (!SDL_PointInRect(&p, square->location)&&square->hover){
                    square->hover = false ;
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
            if (move==NULL)
                return ERROR ;
            move->sourceRow = 7 - board->pressedLocation[0] ;
            move->sourceColumn = board->pressedLocation[1] ;
            move->destRow = 7 - squareRow ;
            move->destColumn = squareCol ;
            if(executeGUIMove(board, move)==NONE){
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


SP_GUI_MESSAGE undoGUIMove(SPChessGame* game){

    SP_CHESS_GAME_MESSAGE ret ;
    ret = spChessUndoPrevMove(game) ;
    if (ret==SP_CHESS_GAME_NO_HISTORY)
        return NONE ;

    spChessUndoPrevMove(game) ;
    return RELOAD_GAME ;
}


void clearBoard(ChessBoard* board){
    ChessSquare* square ;
    for (int i=0;i<SP_CHESS_GAME_N_ROWS;i++){
        for (int j=0;j<SP_CHESS_GAME_N_COLUMNS;j++) {
            square = (ChessSquare*)board->squares[i][j]->data ;
            square->pressed = false ;
            square->highlighted = false ;
            square->threatend = false ;
            square->capture = false ;
        }
    }
    board->pressed = false ;
}


SP_GUI_MESSAGE executeGUIMove(ChessBoard* board, SPMove* move){

    if (spChessCheckLegalMove(board->game, move)!=SP_CHESS_GAME_LEGAL_MOVE)
        return ERROR ;

    ChessSquare* dest = (ChessSquare*)board->squares[7-move->destRow][move->destColumn]->data ;
    ChessSquare* src = (ChessSquare*)board->squares[7-move->sourceRow][move->sourceColumn]->data ;

    executeMove(board->game, move) ;

    // move piece texture from source to dest square
    dest->piece = src->piece ;
    src->piece = NULL ;

    return NONE ;
}


SP_GUI_MESSAGE colorPossibleMoves(ChessBoard* board, int row, int col){

    SPArrayList* moves = spChessGetMoves(board->game, 7 - row, col) ;
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

        if (square->piece!=NULL)
            square->capture = true ;

        testMove(board->game, move) ;
        if (spChessIsThreatend(board->game, move->destRow, move->destColumn, player)==SP_CHESS_GAME_UNDER_THREAT)
            square->threatend = true ;
        undoTestMove(board->game, move) ;
    }

    return NONE ;
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


void destroyPieceTextures(){
    for (int i=0;i<12;i++){
        SDL_DestroyTexture(pieces[i]) ;
    }
}


void destroyButtons(Widget** buttons){
    for (int i=0; i<GAME_NUM_BUTTONS; i++){
        buttons[i]->destroy(buttons[i]) ;
        free(buttons[i]) ;
    }
}


SP_GUI_MESSAGE undoAction(){
    return UNDO_MOVE ;
}

SP_GUI_MESSAGE restartAction(){
    return START_NEW_GAME ;
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