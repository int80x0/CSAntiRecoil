cmake_minimum_required(VERSION 3.12)
project(CSAntiRecoil VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add nlohmann/json as a dependency (Header-only library)
include(FetchContent)
FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

# Source files
set(SOURCES
        main.cpp
        AntiRecoilEngine.cpp
        PatternLoader.cpp
        ConfigManager.cpp
        InputManager.cpp
        ConsoleUI.cpp
        utils/Logger.cpp
)

# Header files
set(HEADERS
        AntiRecoilEngine.h
        PatternLoader.h
        ConfigManager.h
        InputManager.h
        ConsoleUI.h
        utils/Logger.h
)

# Create executable
add_executable(CSAntiRecoil ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(CSAntiRecoil PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Link libraries
target_link_libraries(CSAntiRecoil PRIVATE nlohmann_json::nlohmann_json)

# OS-specific settings
if(WIN32)
    # Windows-specific settings
    target_link_libraries(CSAntiRecoil PRIVATE winmm)

    # Add Windows-specific compile definitions
    target_compile_definitions(CSAntiRecoil PRIVATE
            WIN32_LEAN_AND_MEAN
            NOMINMAX
            _CRT_SECURE_NO_WARNINGS
    )

    # Add Windows manifest to enable Windows 10 features
    if(MSVC)
        set_target_properties(CSAntiRecoil PROPERTIES
                VS_GLOBAL_ApplicationManifest "${CMAKE_CURRENT_SOURCE_DIR}/app.manifest"
        )
    endif()
endif()

# Copy data directory to output
add_custom_command(TARGET CSAntiRecoil POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data
)

# Create logs directory in output
add_custom_command(TARGET CSAntiRecoil POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/logs
)

# Install target
install(TARGETS CSAntiRecoil DESTINATION bin)
install(DIRECTORY data/ DESTINATION bin/data)