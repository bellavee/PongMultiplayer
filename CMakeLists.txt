cmake_minimum_required(VERSION 3.30)
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
	src/TextInputField.cpp
	src/TextInputField.cpp

)

target_link_libraries(PongMulti PRIVATE
        sfml-graphics
        sfml-audio
)
target_compile_features(PongMulti PRIVATE cxx_std_17)

if(WIN32)
    set(OPENAL_DLL "${CMAKE_BINARY_DIR}/_deps/sfml-src/extlibs/bin/x64/openal32.dll")
    if(EXISTS "${OPENAL_DLL}")
        add_custom_command(
                TARGET PongMulti POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy "${OPENAL_DLL}" "${CMAKE_BINARY_DIR}"
        )
    endif()

    target_link_libraries(PongMulti PRIVATE wsock32 ws2_32)

endif()

set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/src/assets)

add_custom_command(
        TARGET PongMulti POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${ASSETS_DIR}
        ${CMAKE_BINARY_DIR}/assets
)
