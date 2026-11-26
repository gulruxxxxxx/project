// ============================================================================
//  helpers.cpp - small helper functions (input checking, formatting, time)
// ============================================================================
#include "queueless.h"

// Removes spaces / tabs / line-ends from both ends of a text.
string trim(const string& text) {
    size_t start = text.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

// Number -> text with a fixed number of decimals, e.g. fmtNum(24, 1) -> "24.0"
string fmtNum(double value, int decimals) {
    ostringstream out;
    out << fixed << setprecision(decimals) << value;
    return out.str();
}

string fmt1(double value) { return fmtNum(value, 1); }

// 7 -> "07" (used for clock times)
string pad2(int n) {
    return string(n < 10 ? "0" : "") + to_string(n);
}

// Makes a text safe to put inside a JSON string (names typed by users).
string jsonEscape(const string& text) {
    string result = "";
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        if (c == '"') result += "\\\"";
        else if (c == '\\') result += "\\\\";
        else if ((unsigned char)c < 32) result += ' ';
        else result += c;
    }
    return result;
}

// Hides most of a student ID on public screens: 123456 -> "***456"
string maskedId(int id) {
    int last = id % 1000;
    string digits = to_string(last);
    while (digits.length() < 3) digits = "0" + digits;
    return "***" + digits;
}

// Current hour of the day (0-23) from the computer clock.
int currentHour() {
    time_t now = time(0);
    tm* t = localtime(&now);
    return t->tm_hour;
}

// Current date and time as text.
string nowString() {
    time_t now = time(0);
    char buffer[40];
    strftime(buffer, sizeof(buffer), "%d %b %Y, %H:%M", localtime(&now));
    return string(buffer);
}

// Current time with seconds, e.g. "14:03:22".
string clockNow() {
    time_t now = time(0);
    char buffer[16];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", localtime(&now));
    return string(buffer);
}

// The clock time after 'minutes' from now, e.g. clockIn(10) -> "14:13"
string clockIn(double minutes) {
    time_t later = time(0) + (time_t)(minutes * 60);
    char buffer[16];
    strftime(buffer, sizeof(buffer), "%H:%M", localtime(&later));
    return string(buffer);
}

// Pauses the program (works on Windows and on Mac/Linux).
void sleepMs(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

// Lets the Windows console understand color codes (does nothing elsewhere).
void enableColors() {
#ifdef _WIN32
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(console, &mode)) SetConsoleMode(console, mode | 0x0004);
    SetConsoleOutputCP(65001);
#endif
}

// Clears the screen (ANSI). Set to false if your terminal shows strange symbols.
const bool CLEAR_SCREEN = true;
void clearScreen() {
    if (CLEAR_SCREEN) cout << "\033[2J\033[H";
}

void pauseScreen() {
    cout << GRAY << "\nPress Enter to continue..." << RESET;
    string dummy;
    if (!getline(cin, dummy)) inputClosed();
}

// Ends the program politely if the input stream is closed (e.g. Ctrl+D).
void inputClosed() {
    cout << "\nInput closed. Goodbye!\n";
    exit(0);
}

// INPUT VALIDATION: keeps asking until the user types ONE whole number
// between minValue and maxValue. Letters, symbols, "12abc" and "1 2" are rejected.
int readInt(const string& prompt, int minValue, int maxValue) {
    while (true) {
        cout << prompt;
        string line;
        if (!getline(cin, line)) inputClosed();
        istringstream in(trim(line));
        int value;
        char extra;
        if (in >> value && !(in >> extra)) {
            if (value >= minValue && value <= maxValue) return value;
            cout << RED << "  Please enter a number from " << minValue
                 << " to " << maxValue << "." << RESET << endl;
        } else {
            cout << RED << "  That is not a valid number. Try again." << RESET << endl;
        }
    }
}

// Reads a name (1-30 characters, not empty).
string readName(const string& prompt) {
    string line;
    while (true) {
        cout << prompt;
        if (!getline(cin, line)) inputClosed();
        line = trim(line);
        if (line.length() >= 1 && line.length() <= 30) return line;
        cout << RED << "  Name must be 1 to 30 characters." << RESET << endl;
    }
}

void printHeader(const string& title) {
    cout << "\n" << CYAN << BOLD
         << "================================================================\n"
         << "  " << title << "\n"
         << "================================================================"
         << RESET << "\n";
}

// Draws a bar of '#' characters, colored by how long the wait is.
void printBar(double minutes, int maxLength) {
    int length = (int)floor(minutes + 0.5);
    if (length > maxLength) length = maxLength;
    if (length < 1) length = 1;
    string color = GREEN;
    if (minutes > SHORT_WAIT_MAX) color = RED;
    else if (minutes > GO_NOW_MAX) color = YELLOW;
    cout << color << string(length, '#') << RESET;
}
