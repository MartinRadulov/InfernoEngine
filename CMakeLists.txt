cmake_minimum_required(VERSION 3.0)

# Project Name
project(InfernoEngine)

# 1. FIND ALL C++ FILES
file(GLOB SOURCES "src/*.cpp")

# 2. CREATE THE EXECUTABLE
add_executable(InfernoEngine ${SOURCES})

# 3. DETECT OS AND LINK LIBRARIES
if(WIN32)
    # --- WINDOWS SETUP (For when you switch to PC) ---
    message(STATUS "Configuring for Windows...")
    set(SDL2_PATH "${CMAKE_SOURCE_DIR}/libs/SDL2")
    target_include_directories(InfernoEngine PRIVATE "${SDL2_PATH}/include")
    target_link_directories(InfernoEngine PRIVATE "${SDL2_PATH}/lib/x64")
    target_link_libraries(InfernoEngine SDL2 SDL2main shell32)
    
    # Copy DLL to output
    add_custom_command(TARGET InfernoEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_PATH}/lib/x64/SDL2.dll"
        $<TARGET_FILE_DIR:InfernoEngine>)

else()
    # --- LINUX SETUP (For right now) ---
    message(STATUS "Configuring for Linux...")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)

    target_include_directories(InfernoEngine PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(InfernoEngine ${SDL2_LIBRARIES})
endif()