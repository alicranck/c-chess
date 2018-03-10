cmake_minimum_required(VERSION 3.7)
project(SPFinalProject)

set(CMAKE_CXX_STANDARD 99)

set(SOURCE_FILES
        SPConsoleMain.c
        SPConsoleMain.h
        SPGUIStartScreen.c
        SPGUIStartScreen.h
        SPGUITypes.c
        SPGUITypes.h
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
        main.c
        SPGUISettingsScreen.c SPGUISettingsScreen.h)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(SPFinalProject ${SOURCE_FILES})
target_link_libraries(SPFinalProject mingw32 SDL2main SDL2)