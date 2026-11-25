// ============================================================================
//  storage.cpp - global data + saving / loading (file handling)
// ============================================================================
#include "queueless.h"

// The program's data lives here (declared "extern" in queueless.h).
Service  services[MAX_SERVICES];
Record   history[MAX_HISTORY];
int      historyCount = 0;
Activity activities[MAX_ACTIVITY];
int      activityCount = 0;

// Fills the program with sample data.
void loadDefaults() {
    string names[MAX_SERVICES] = {"Student Affairs", "Campus Clinic", "Printing Center", "Cafeteria"};
    double avg[MAX_SERVICES] = {4.0, 6.0, 2.0, 1.5};
    int counters[MAX_SERVICES] = {3, 2, 2, 4};
    int waiting[MAX_SERVICES] = {18, 7, 12, 25};

    for (int s = 0; s < MAX_SERVICES; s++) {
        services[s].name = names[s];
        services[s].avgMinutes = avg[s];
        services[s].counters = counters[s];
        services[s].queueSize = waiting[s];
        for (int i = 0; i < waiting[s]; i++) {
            services[s].queue[i].name = "Walk-in " + to_string(i + 1);
            services[s].queue[i].id = 990000 + s * 100 + i;   // fake IDs for sample people
            services[s].queue[i].joinHour = currentHour();
            services[s].queue[i].estWait = calcWait(i, avg[s], counters[s]);
        }
    }
}

// Saves all queues and settings. Returns false if the file cannot be opened.
bool saveQueues() {
    ofstream file(QUEUE_FILE.c_str());
    if (!file) return false;
    file << "QUEUELESS 1\n";
    for (int s = 0; s < MAX_SERVICES; s++) {
        file << services[s].name << "\n";
        file << services[s].avgMinutes << " " << services[s].counters << " "
             << services[s].queueSize << "\n";
        for (int i = 0; i < services[s].queueSize; i++) {
            const Person& p = services[s].queue[i];
            file << p.id << " " << p.joinHour << " " << p.estWait << " " << p.name << "\n";
        }
    }
    return true;
}

// Loads queues and settings. Returns false if the file is missing or damaged;
// the program then keeps working with the sample data.
bool loadQueues() {
    ifstream file(QUEUE_FILE.c_str());
    if (!file) return false;

    string header;
    getline(file, header);
    if (trim(header) != "QUEUELESS 1") return false;

    Service temp[MAX_SERVICES];   // load into a temporary copy first
    bool ok = true;
    for (int s = 0; s < MAX_SERVICES && ok; s++) {
        getline(file, temp[s].name);
        temp[s].name = trim(temp[s].name);
        if (temp[s].name.empty()) { ok = false; break; }
        if (!(file >> temp[s].avgMinutes >> temp[s].counters >> temp[s].queueSize)) { ok = false; break; }
        if (temp[s].avgMinutes <= 0 || temp[s].avgMinutes > 120) { ok = false; break; }
        if (temp[s].counters < 0 || temp[s].counters > MAX_COUNTERS) { ok = false; break; }
        if (temp[s].queueSize < 0 || temp[s].queueSize > MAX_QUEUE) { ok = false; break; }
        file.ignore(1000, '\n');
        for (int i = 0; i < temp[s].queueSize; i++) {
            Person& p = temp[s].queue[i];
            if (!(file >> p.id >> p.joinHour >> p.estWait)) { ok = false; break; }
            getline(file, p.name);
            p.name = trim(p.name);
            if (p.name.empty()) { ok = false; break; }
        }
    }
    if (ok) {
        for (int s = 0; s < MAX_SERVICES; s++) services[s] = temp[s];   // all good: use it
    }
    return ok;
}

bool saveHistory() {
    ofstream file(HISTORY_FILE.c_str());
    if (!file) return false;
    for (int i = 0; i < historyCount; i++) {
        file << history[i].service << " " << history[i].hour << " " << history[i].wait << "\n";
    }
    return true;
}

// Loads history; bad lines are skipped. A missing file simply means "no history yet".
void loadHistory() {
    historyCount = 0;
    ifstream file(HISTORY_FILE.c_str());
    if (!file) return;
    int s, h;
    double w;
    while (historyCount < MAX_HISTORY && (file >> s >> h >> w)) {
        if (s >= 0 && s < MAX_SERVICES && h >= 0 && h <= 23 && w >= 0 && w < 10000) {
            history[historyCount].service = s;
            history[historyCount].hour = h;
            history[historyCount].wait = w;
            historyCount++;
        }
    }
}

// Saves everything and refreshes the web dashboard. Called after every change.
void saveAll() {
    if (!saveQueues()) cout << RED << "  Warning: could not save " << QUEUE_FILE << RESET << endl;
    if (!saveHistory()) cout << RED << "  Warning: could not save " << HISTORY_FILE << RESET << endl;
    writeDashboardData();
}
