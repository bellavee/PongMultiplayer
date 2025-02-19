﻿cmake_minimum_required(VERSION 3.30)
project(PongMulti)

set(CMAKE_CXX_STANDARD 20)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0-rc.2
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(PongMulti src/main.cpp
        src/Game.cpp
        src/Game.h
        src/Paddle.cpp
        src/Paddle.h
        src/Ball.cpp
        src/Ball.h
        src/Score.cpp
        src/Score.h
        src/Entity.cpp
        src/Entity.h
        src/Button.cpp
        src/Button.h
        src/UI_LostConnection.cpp
        src/UI_LostConnection.h
        src/UI_MainMenu.cpp
        src/UI_MainMenu.h
        src/UI_PauseMenu.cpp
        src/UI_PauseMenu.h
        src/InterfaceElement.cpp
        src/InterfaceElement.h
        src/resources.h
        src/WinsockClient.cpp
        src/WinsockClient.h
        src/TextInputField.cpp
        src/TextInputField.h
	src/UI_WaitingPlayer.cpp
	src/UI_WaitingPlayer.h
        src/TextInputField.cpp
	src/UI_PlayMenu.h
	src/UI_PlayMenu.cpp
)

add_executable(Server src/mainServer.cpp
        src/Server.cpp
        src/Server.h
        src/InterfaceElement.cpp
        src/InterfaceElement.h
        src/UI_ServerMenu.h
        src/UI_ServerMenu.cpp
        src/resources.h
        src/TextInputField.cpp
	src/TextInputField.cpp
        src/Button.cpp
        src/Button.h
        src/UI_ServerRunning.h
        src/UI_ServerRunning.cpp
        src/Ball.cpp
        src/Ball.h
        src/Entity.cpp
        src/Entity.h
        src/Paddle.cpp
        src/Paddle.h
)

include(FetchContent)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)

FetchContent_MakeAvailable(json)

#target_include_directories(nlohmann_json INTERFACE ${json_SOURCE_DIR})

target_link_libraries(PongMulti PRIVATE
        sfml-graphics
        sfml-audio
        nlohmann_json
)

target_link_libraries(Server PRIVATE
        sfml-graphics
        sfml-audio
        nlohmann_json
)



target_compile_features(PongMulti PRIVATE cxx_std_17)
target_compile_features(Server PRIVATE cxx_std_17)

if(WIN32)
    set(OPENAL_DLL "${CMAKE_BINARY_DIR}/_deps/sfml-src/extlibs/bin/x64/openal32.dll")
    if(EXISTS "${OPENAL_DLL}")
        add_custom_command(
                TARGET PongMulti POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy "${OPENAL_DLL}" "${CMAKE_BINARY_DIR}"
        )
        add_custom_command(
                TARGET Server POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy "${OPENAL_DLL}" "${CMAKE_BINARY_DIR}"
        )
    endif()

    target_link_libraries(PongMulti PRIVATE wsock32 ws2_32)
    target_link_libraries(Server PRIVATE wsock32 ws2_32)

endif()

set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/src/assets)

add_custom_command(
        TARGET PongMulti POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${ASSETS_DIR}
        ${CMAKE_BINARY_DIR}/assets
)

add_custom_command(
        TARGET Server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${ASSETS_DIR}
        ${CMAKE_BINARY_DIR}/assets
)