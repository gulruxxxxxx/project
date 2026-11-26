// ============================================================================
//  dashboard.cpp - the bridge to the web page
//
//  The C++ program writes all its current data to  web/data.js  after every
//  change.  The page  web/index.html  (open it in any browser) reads that file
//  every 2 seconds and redraws itself.  So: C++ = brain, browser = screen.
// ============================================================================
#include "queueless.h"

// Writes a list of numbers as a JSON array, e.g. [1.50,2.00,-1.00]
// A value of -1 means "no data".
static void writeNumberList(ofstream& f, const double values[], int count, int decimals) {
    f << "[";
    for (int i = 0; i < count; i++) {
        if (i > 0) f << ",";
        f << fmtNum(values[i], decimals);
    }
    f << "]";
}

void writeDashboardData() {
    ofstream f(WEB_DATA_FILE.c_str());
    if (!f) return;   // no web/ folder: the console program still works

    f << "window.QL = {\n";
    f << "\"updated\":\"" << jsonEscape(nowString()) << "\",\n";
    f << "\"hour\":" << currentHour() << ",\n";
    f << "\"open\":" << OPEN_HOUR << ",\"close\":" << CLOSE_HOUR << ",\n";
    f << "\"goMax\":" << fmtNum(GO_NOW_MAX, 1) << ",\"shortMax\":" << fmtNum(SHORT_WAIT_MAX, 1)
      << ",\"maxCounters\":" << MAX_COUNTERS << ",\n";
    f << "\"historyCount\":" << historyCount << ",\n";
    f << "\"best\":" << bestService() << ",\"worst\":" << worstService() << ",\n";

    // ---- services ----
    f << "\"services\":[\n";
    for (int s = 0; s < MAX_SERVICES; s++) {
        const Service& sv = services[s];
        double wait = waitOf(sv);
        string advice = adviceFor(sv);

        double avg[24], hourly[24];
        int cnt[24];
        hourlyAverages(s, avg, cnt);
        for (int h = 0; h < 24; h++) hourly[h] = (cnt[h] > 0) ? avg[h] : -1;
        int peak = peakHour(avg, cnt);
        int quiet = quietestHour(avg, cnt);

        int needed = 0;                               // 0 = already under the "short wait" limit
        if (wait < 0 || wait > SHORT_WAIT_MAX) needed = countersNeeded(sv, SHORT_WAIT_MAX);

        f << "{\"name\":\"" << jsonEscape(sv.name) << "\""
          << ",\"avg\":" << fmtNum(sv.avgMinutes, 1)
          << ",\"counters\":" << sv.counters
          << ",\"queueSize\":" << sv.queueSize
          << ",\"wait\":" << fmtNum(wait, 2)
          << ",\"status\":\"" << cssClassFor(advice) << "\""
          << ",\"advice\":\"" << advice << "\""
          << ",\"needed\":" << needed
          << ",\"peak\":" << peak << ",\"quiet\":" << quiet
          << ",\"overall\":" << fmtNum(overallAverage(s), 2)
          << ",\"records\":" << countRecords(s) << ",\n";

        f << " \"hourly\":";
        writeNumberList(f, hourly, 24, 2);
        f << ",\n \"queue\":[";
        int show = (sv.queueSize < WEB_QUEUE_ROWS) ? sv.queueSize : WEB_QUEUE_ROWS;
        for (int i = 0; i < show; i++) {
            if (i > 0) f << ",";
            f << "{\"n\":\"" << jsonEscape(sv.queue[i].name) << "\""
              << ",\"id\":\"" << maskedId(sv.queue[i].id) << "\""
              << ",\"eta\":" << fmtNum(positionWait(sv, i), 2) << "}";
        }
        f << "]}";
        if (s < MAX_SERVICES - 1) f << ",";
        f << "\n";
    }
    f << "],\n";

    // ---- activity feed ----
    f << "\"activity\":[";
    for (int i = 0; i < activityCount; i++) {
        if (i > 0) f << ",";
        f << "{\"t\":\"" << jsonEscape(activities[i].time) << "\""
          << ",\"text\":\"" << jsonEscape(activities[i].text) << "\"}";
    }
    f << "]\n};\n";
}

// Opens the web dashboard in the default browser.
void openDashboard() {
    writeDashboardData();
    ifstream test(WEB_PAGE_FILE.c_str());
    if (!test) {
        cout << RED << "  Could not find " << WEB_PAGE_FILE << "." << RESET << "\n"
             << "  Run the program from the project folder (the one that contains the 'web' folder).\n";
        return;
    }
    test.close();
    int result;
#ifdef _WIN32
    result = system("start \"\" \"web\\index.html\"");
#elif defined(__APPLE__)
    result = system("open web/index.html");
#else
    result = system("xdg-open web/index.html >/dev/null 2>&1");
#endif
    if (result != 0) cout << YELLOW << "  Could not open the browser automatically. Open web/index.html yourself." << RESET << "\n";
    cout << GREEN << "  Dashboard opened in your browser." << RESET << "\n"
         << "  Keep it open - it updates by itself every time you change something here.\n";
}
