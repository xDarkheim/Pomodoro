cmake_minimum_required(VERSION 3.20)
project(PomodoroTimer VERSION 2.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt6_standard_project_setup()

# Define source files
set(HEADERS
    PomodoroTimer.h
    CircularProgressBar.h
    SettingsDialog.h
    StatisticsDialog.h
    TimerState.h
    SystemTrayManager.h
    KeyboardShortcuts.h
    NotificationManager.h
)

set(SOURCES
    main.cpp
    PomodoroTimer.cpp
    CircularProgressBar.cpp
    SettingsDialog.cpp
    StatisticsDialog.cpp
    TimerState.cpp
    SystemTrayManager.cpp
    KeyboardShortcuts.cpp
    NotificationManager.cpp
)

# Create executable
qt6_add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

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
        "${CMAKE_CURRENT_SOURCE_DIR}/pomodoro-timer.desktop.in"
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
