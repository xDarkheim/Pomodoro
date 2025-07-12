# Pomodoro Timer

A simple Pomodoro Timer application built with C++ and the Qt framework.

## Features

*   Work, Short Break, and Long Break timers.
*   Customizable timer durations.
*   System tray integration with status updates.
*   Desktop notifications for state changes.
*   Session tracking.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Building

### Prerequisites

*   A C++17 compatible compiler
*   Qt 6
*   CMake

### Steps

1.  Clone the repository:
    ```bash
    git clone <repository-url>
    ```
2.  Create a build directory:
    ```bash
    cd PomodoroTimer
    mkdir build && cd build
    ```
3.  Run CMake and build the project:
    ```bash
    cmake ..
    make
    ```
4.  Run the application:
    ```bash
    ./PomodoroTimer
    ```
