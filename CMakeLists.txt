cmake_minimum_required(VERSION 3.20)
project(PomodoroTimer VERSION 0.1.1 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt6_standard_project_setup()

# Include directories
include_directories(src/core)
include_directories(src/ui)
include_directories(src/system)
include_directories(src/utils)

# Define source files with new structure
set(CORE_HEADERS
    src/core/TimerState.h
    src/core/TimerController.h
    src/core/PomodoroConfig.h
)

set(UI_HEADERS
    src/ui/PomodoroTimer.h
    src/ui/CircularProgressBar.h
    src/ui/SettingsDialog.h
    src/ui/StatisticsDialog.h
)

set(SYSTEM_HEADERS
    src/system/SystemTrayManager.h
    src/system/KeyboardShortcuts.h
    src/system/NotificationManager.h
)

set(CORE_SOURCES
    src/core/TimerState.cpp
    src/core/TimerController.cpp
    src/core/PomodoroConfig.cpp
)

set(UI_SOURCES
    src/ui/PomodoroTimer.cpp
    src/ui/CircularProgressBar.cpp
    src/ui/SettingsDialog.cpp
    src/ui/StatisticsDialog.cpp
)

set(SYSTEM_SOURCES
    src/system/SystemTrayManager.cpp
    src/system/KeyboardShortcuts.cpp
    src/system/NotificationManager.cpp
)

set(ALL_HEADERS ${CORE_HEADERS} ${UI_HEADERS} ${SYSTEM_HEADERS})
set(ALL_SOURCES main.cpp ${CORE_SOURCES} ${UI_SOURCES} ${SYSTEM_SOURCES})

# Create executable
qt6_add_executable(${PROJECT_NAME} ${ALL_SOURCES} ${ALL_HEADERS})

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
)

# Set target properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    OUTPUT_NAME "PomodoroTimer"
)

# Platform-specific configurations
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
elseif(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "Pomodoro Timer"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.pomodoroapp.timer"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_INFO_STRING "Pomodoro Focus Timer"
    )
endif()

# Compiler warnings and optimizations
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4 /WX-
        $<$<CONFIG:Release>:/O2>
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Wpedantic
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Debug>:-g -O0>
    )
endif()

# --- Installation Rules ---

# Configure desktop file for Linux
if(UNIX AND NOT APPLE)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/pomodoro-timer.desktop.in"
        "${CMAKE_CURRENT_BINARY_DIR}/pomodoro-timer.desktop"
        @ONLY
    )

    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/pomodoro-timer.desktop"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications"
    )
endif()


# Install rules (optional)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Install prefix" FORCE)
endif()

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
