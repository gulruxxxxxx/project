# QueueLess - campus waiting-time manager

Students see how long each campus service will take **before** they walk there
(Student Affairs, Clinic, Printing, Cafeteria), join a queue, and track their
place. Staff serve people and open/close counters. Core formula:

    wait = people ahead x average minutes per person / open counters

## Build and run

    g++ -std=c++17 *.cpp -o queueless      (or:  make)
    ./queueless                            (Windows: queueless.exe)

Run it from this folder (the one that contains `web/`). Staff PIN: **1234**.

## Running in VS Code (important)

VS Code's "Run C/C++ file" button compiles only ONE file, so you get
"undefined reference" errors. Use one of these instead:

1. **File -> Open Folder** and open the project folder itself (the one that contains `web/`).
2. Press **Ctrl+Shift+B** (Mac: Cmd+Shift+B) - the task "Build QueueLess" compiles all files.
3. In the VS Code terminal type `./queueless` (Windows PowerShell: `.\queueless`).

No VS Code? Double-click `run.bat` (Windows) or run `./run.sh` (Mac/Linux).
Windows needs a C++ compiler (MinGW-w64 / MSYS2); Mac needs `xcode-select --install`.
Check with `g++ --version`.

## The web dashboard

Main menu **10** opens `web/index.html`. The C++ program writes its data to
`web/data.js` after every change and the page redraws itself every 2 seconds.
The page only *displays*; every calculation happens in C++.

Exhibition setup: terminal on one side, browser on the other, then menu
**9 -> 2 (Live demo)** and watch the page move. "Big screen" enlarges the text.

## Code map (who explains what)

| File | What is inside | Course concepts |
|---|---|---|
| `queueless.h` | constants, structs, prototypes | structs, constants, arrays |
| `helpers.cpp` | input validation, formatting, clock | functions, strings, loops, validation |
| `queue.cpp` | wait formula, join / serve / leave | functions, arrays, structs, conditionals |
| `storage.cpp` | save and load `queues.txt`, `history.txt` | file handling (ifstream/ofstream) |
| `analytics.cpp` | peak and quietest hour from history | arrays, loops |
| `dashboard.cpp` | writes `web/data.js`, opens the browser | file output, strings |
| `menus.cpp` | all menus and screens | switch, loops, functions |
| `demo.cpp` | busy-day generator, live demo | rand(), loops |
| `web/index.html` | the visual dashboard (HTML/CSS/JS) | not C++ - display only |

## Files the program creates
`queues.txt` (queues, counters), `history.txt` (waiting-time records),
`web/data.js` (data for the web page). Delete the first two to start fresh.
