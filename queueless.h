// ============================================================================
//  QueueLess - shared header: constants, structs, global data, prototypes
// ============================================================================
#ifndef QUEUELESS_H
#define QUEUELESS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <cmath>

#ifdef _WIN32
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>     // colors in the console + Sleep()
#else
    #include <unistd.h>      // usleep()
#endif

using namespace std;

// ---- Settings (no "magic numbers" hidden in the code) ----------------------
const int    MAX_SERVICES     = 4;     // Student Affairs, Clinic, Printing, Cafeteria
const int    MAX_QUEUE        = 100;   // max people waiting in one queue
const int    MAX_HISTORY      = 5000;  // max saved waiting-time records
const int    MAX_COUNTERS     = 10;    // max counters a service can open
const int    MAX_ACTIVITY     = 12;    // recent events shown on the dashboard
const int    OPEN_HOUR        = 8;     // services open at 08:00
const int    CLOSE_HOUR       = 18;    // ... and close at 18:00
const double GO_NOW_MAX       = 5.0;   // wait <= 5 min   -> GO NOW
const double SHORT_WAIT_MAX   = 15.0;  // wait <= 15 min  -> SHORT WAIT, else CONSIDER WAITING
const int    SIM_EXTRA        = 2;     // counters the simulation adds by default
const int    SAMPLES_PER_HOUR = 8;     // used by "generate a busy day"
const int    DEMO_SECONDS     = 30;    // length of the live demo
const int    WEB_QUEUE_ROWS   = 10;    // people per queue sent to the web page
const string STAFF_PIN        = "1234";

const string QUEUE_FILE    = "queues.txt";
const string HISTORY_FILE  = "history.txt";
const string WEB_DATA_FILE = "web/data.js";      // written by C++, read by the web page
const string WEB_PAGE_FILE = "web/index.html";   // the visual dashboard

// ---- Terminal colors (ANSI codes) ------------------------------------------
const string RESET  = "\033[0m";
const string BOLD   = "\033[1m";
const string RED    = "\033[31m";
const string GREEN  = "\033[32m";
const string YELLOW = "\033[33m";
const string CYAN   = "\033[36m";
const string GRAY   = "\033[90m";

// ---- Data structures --------------------------------------------------------
struct Person {            // one person waiting in a queue
    string name;
    int    id;             // 6-digit student ID
    int    joinHour;       // hour of the day when they joined (0-23)
    double estWait;        // wait we quoted when they joined (minutes)
};

struct Service {           // one campus service with its own queue
    string name;
    double avgMinutes;     // average service time per person (minutes)
    int    counters;       // how many counters are open
    int    queueSize;      // how many people are waiting
    Person queue[MAX_QUEUE];
};

struct Record {            // one line of history (feeds the analytics)
    int    service;        // service index 0..3
    int    hour;           // hour of day 0..23
    double wait;           // wait quoted at that hour (minutes)
};

struct Activity {          // one line of the "what just happened" feed
    string time;
    string text;
};

// ---- Global data (defined once in storage.cpp) -----------------------------
extern Service  services[MAX_SERVICES];
extern Record   history[MAX_HISTORY];
extern int      historyCount;
extern Activity activities[MAX_ACTIVITY];
extern int      activityCount;

// ---- helpers.cpp ------------------------------------------------------------
string trim(const string& text);
string fmtNum(double value, int decimals);
string fmt1(double value);
string pad2(int n);
string jsonEscape(const string& text);
string maskedId(int id);
int    currentHour();
string nowString();
string clockNow();
string clockIn(double minutes);
void   sleepMs(int ms);
void   enableColors();
void   clearScreen();
void   pauseScreen();
void   inputClosed();
int    readInt(const string& prompt, int minValue, int maxValue);
string readName(const string& prompt);
void   printHeader(const string& title);
void   printBar(double minutes, int maxLength);

// ---- queue.cpp (wait-time logic + queue operations) -------------------------
double calcWait(int people, double avgMinutes, int counters);
double waitOf(const Service& s);
double positionWait(const Service& s, int position);
string adviceForWait(double wait);
string adviceFor(const Service& s);
string colorFor(const string& advice);
string cssClassFor(const string& advice);
int    bestService();
int    worstService();
int    countersNeeded(const Service& s, double target);
void   removeAt(Service& s, int index);
bool   findPerson(int id, int& svc, int& pos);
bool   addRecord(int service, int hour, double wait);
void   logActivity(const string& text);
bool   joinPerson(int svc, const string& name, int id, Person& ticket);
bool   serveNext(int svc, Person& served);

// ---- storage.cpp ------------------------------------------------------------
void loadDefaults();
bool saveQueues();
bool loadQueues();
bool saveHistory();
void loadHistory();
void saveAll();

// ---- analytics.cpp ----------------------------------------------------------
void   hourlyAverages(int service, double avg[], int cnt[]);
int    peakHour(const double avg[], const int cnt[]);
int    quietestHour(const double avg[], const int cnt[]);
int    countRecords(int service);
double overallAverage(int service);
void   showServiceAnalytics(int s);
void   showAnalyticsSummary();

// ---- demo.cpp ---------------------------------------------------------------
int  peakWeight(int service, int hour);
void simulateDay();
void liveDemo();

// ---- dashboard.cpp ----------------------------------------------------------
void writeDashboardData();
void openDashboard();

// ---- menus.cpp --------------------------------------------------------------
void printOverview();
void checkWaitTime();
void joinQueue();
void trackMyPlace();
void leaveQueue();
void bestTimeMenu();
void analyticsMenu();
void counterSimulation();
void staffPanel();
void demoMenu();
void showMainMenu();

#endif
