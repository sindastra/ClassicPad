cmake_minimum_required(VERSION 3.15)
project(ClassicPad)

set(SOURCE_FILES classicpad.cpp)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Use mingw cross-compiler on Linux
    set(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
    set(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
    set(CMAKE_RC_COMPILER i686-w64-mingw32-windres)

    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
endif()

add_executable(ClassicPad ${SOURCE_FILES})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Link required Windows libs
    target_link_libraries(ClassicPad PRIVATE comdlg32 user32 gdi32)
    # Use windows subsystem to hide console
    set_target_properties(ClassicPad PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS"
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Link Windows libs for cross compilation
    target_link_libraries(ClassicPad PRIVATE comdlg32 user32 gdi32)
endif()
