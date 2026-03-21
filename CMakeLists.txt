cmake_minimum_required(VERSION 3.16)
project(ChatSystem VERSION 1.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt Configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt
find_package(Qt6 COMPONENTS Core Quick Widgets Network QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 COMPONENTS Core Quick Widgets Network REQUIRED)
endif()

set(QT_VERSION_MAJOR ${Qt6_VERSION_MAJOR})
if(NOT Qt6_FOUND)
    set(QT_VERSION_MAJOR 5)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Backend executables (Phase 1 - raw sockets)
add_executable(server chat-server.cpp)

add_executable(client chat-client.cpp)
find_package(Threads REQUIRED)
target_link_libraries(client Threads::Threads)

# Frontend Qt Application (Phase 2)
qt_add_executable(chat-app
    frontend/main.cpp
    frontend/models/messagemodel.cpp
    frontend/models/messagemodel.h
    frontend/networkclient.h
    frontend/resources.qrc
)

target_link_libraries(chat-app PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

# Include directories
target_include_directories(chat-app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/frontend
    ${CMAKE_CURRENT_SOURCE_DIR}/frontend/models
)

# Install targets
install(TARGETS server client chat-app DESTINATION bin)
