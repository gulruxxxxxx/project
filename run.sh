#!/bin/sh
# Mac / Linux: builds ALL source files and starts the program.
cd "$(dirname "$0")" || exit 1
g++ -std=c++17 -Wall main.cpp helpers.cpp queue.cpp storage.cpp analytics.cpp dashboard.cpp menus.cpp demo.cpp -o queueless && ./queueless
