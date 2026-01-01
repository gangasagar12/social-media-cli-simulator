cmake_minimum_required(VERSION 3.10)
project(Socialmedia)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Platform-specific settings
if(WIN32)
    add_definitions(-D_WIN32)
else()
    # Linux/Unix specific settings
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/FileHandler.cpp
    src/Post.cpp
    src/Person.cpp
    src/User.cpp
    src/App.cpp
)

# Create executable
add_executable(SocialShell ${SOURCES})

# For Windows, link conio if needed
if(WIN32)
    # conio.h is part of Windows SDK, no linking needed
endif()