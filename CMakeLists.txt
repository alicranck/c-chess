cmake_minimum_required(VERSION 3.7)
project(SPFinalProject)

set(CMAKE_CXX_STANDARD 99)

set(SOURCE_FILES
        main.c
        SPArrayList.c
        SPArrayList.h
        SPCHESSGame.c
        SPCHESSGame.h
        SPCHESSParser.c
        SPCHESSParser.h
        SPMainAux.c
        SPMainAux.h
        SPMiniMax.c
        SPMiniMax.h

)

add_executable(SPFinalProject ${SOURCE_FILES})