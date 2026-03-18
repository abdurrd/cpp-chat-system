cmake_minimum_required(VERSION 3.16)
project(ChatSystem VERSION 1.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt Configuration
find_package(Qt6 COMPONENTS Core Quick Widgets Network QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 COMPONENTS Core Quick Widgets Network REQUIRED)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Backend executables (Phase 1)
add_executable(server chat-server.cpp)
add_executable(client chat-client.cpp)

# Frontend Qt Application (Phase 2)
# add_executable(chat-app
#     frontend/main.cpp
#     frontend/models/messagemodel.cpp
# )
# target_link_libraries(chat-app
#     Qt${QT_VERSION_MAJOR}::Core
#     Qt${QT_VERSION_MAJOR}::Quick
#     Qt${QT_VERSION_MAJOR}::Widgets
#     Qt${QT_VERSION_MAJOR}::Network
# )

# Install targets
install(TARGETS server client DESTINATION bin)
