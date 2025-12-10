cmake_minimum_required(VERSION 3.0)

# Project Name
project(InfernoEngine)

# 1. FIND ALL C++ FILES
file(GLOB SOURCES "src/*.cpp")

# 2. CREATE THE EXECUTABLE
add_executable(InfernoEngine ${SOURCES})

# 3. DETECT OS AND LINK LIBRARIES
if(WIN32)
    # --- WINDOWS SETUP ---
    message(STATUS "Configuring for Windows...")
    
    # Paths (Assuming SDL2 is in libs/SDL2)
    set(SDL2_PATH "${CMAKE_SOURCE_DIR}/libs/SDL2")
    
    # Linking
    target_include_directories(InfernoEngine PRIVATE "${SDL2_PATH}/include")
    target_link_directories(InfernoEngine PRIVATE "${SDL2_PATH}/lib/x64")
    target_link_libraries(InfernoEngine SDL2 SDL2main shell32 SDL2_image) # Added SDL2_image
    
    # Copy DLLs to output folder
    add_custom_command(TARGET InfernoEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_PATH}/lib/x64/SDL2.dll"
        "$<TARGET_FILE_DIR:InfernoEngine>"
    )
    add_custom_command(TARGET InfernoEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_PATH}/lib/x64/SDL2_image.dll"
        "$<TARGET_FILE_DIR:InfernoEngine>"
    )

else()
    # --- LINUX / MACOS SETUP ---
    message(STATUS "Configuring for Linux/MacOS...")
    
    # Use PkgConfig to find system libraries 
    # The simplest form of linking that should never fail the configuration
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2 SDL2_image) 

    target_include_directories(InfernoEngine PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(InfernoEngine PRIVATE ${SDL2_LIBRARIES} SDL2_image)
endif()

# 4. ASSET COPY (CROSS-PLATFORM)
# The most robust way to copy the assets folder to the build directory.
file(COPY "${CMAKE_SOURCE_DIR}/assets" 
     DESTINATION "$<TARGET_FILE_DIR:InfernoEngine>")