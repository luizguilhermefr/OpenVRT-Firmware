cmake_minimum_required(VERSION 2.8.4)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake-build-debug/cmake_install.cmake)
set(PROJECT_NAME OpenVRT)

set(${CMAKE_PROJECT_NAME}_BOARD uno)
set(${CMAKE_PROJECT_NAME}_PORT /dev/ttyACM0)
set(${CMAKE_PROJECT_NAME}_SKETCH main.ino)

project(${PROJECT_NAME})

generate_arduino_firmware(${CMAKE_PROJECT_NAME})
