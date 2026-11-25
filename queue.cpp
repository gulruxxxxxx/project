// ============================================================================
//  queue.cpp - the core wait-time formula and the queue operations
//      ETA = people ahead x average service time / open counters
// ============================================================================
#include "queueless.h"

// Estimated wait in minutes, or -1 if no counter is open.
double calcWait(int people, double avgMinutes, int counters) {
    if (counters <= 0) return -1;
    return people * avgMinutes / counters;
}

// Wait for someone who joins the END of this queue.
double waitOf(const Service& s) {
    return calcWait(s.queueSize, s.avgMinutes, s.counters);
}

// Wait for the person at 'position' (0 = next in line): only the people ahead count.
double positionWait(const Service& s, int position) {
    return calcWait(position, s.avgMinutes, s.counters);
}

// Turns a wait time into advice text.
string adviceForWait(double wait) {
    if (wait < 0) return "CLOSED";
    if (wait <= GO_NOW_MAX) return "GO NOW";
    if (wait <= SHORT_WAIT_MAX) return "SHORT WAIT";
    return "CONSIDER WAITING";
}

string adviceFor(const Service& s) {
    return adviceForWait(waitOf(s));
}

string colorFor(const string& advice) {
    if (advice == "GO NOW") return GREEN;
    if (advice == "SHORT WAIT") return YELLOW;
    if (advice == "CONSIDER WAITING") return RED;
    return GRAY;
}

// Short status word used by the web page: go / short / wait / closed
string cssClassFor(const string& advice) {
    if (advice == "GO NOW") return "go";
    if (advice == "SHORT WAIT") return "short";
    if (advice == "CONSIDER WAITING") return "wait";
    return "closed";
}

// Index of the open service with the shortest wait (-1 if all are closed).
int bestService() {
    int best = -1;
    for (int i = 0; i < MAX_SERVICES; i++) {
        if (services[i].counters <= 0) continue;
        if (best == -1 || waitOf(services[i]) < waitOf(services[best])) best = i;
    }
    return best;
}

// Index of the open service with the longest wait (-1 if all are closed).
int worstService() {
    int worst = -1;
    for (int i = 0; i < MAX_SERVICES; i++) {
        if (services[i].counters <= 0) continue;
        if (worst == -1 || waitOf(services[i]) > waitOf(services[worst])) worst = i;
    }
    return worst;
}

// Smallest number of counters that brings the wait down to 'target' minutes.
// Returns -1 if even MAX_COUNTERS is not enough.
int countersNeeded(const Service& s, double target) {
    for (int c = 1; c <= MAX_COUNTERS; c++) {
        if (calcWait(s.queueSize, s.avgMinutes, c) <= target) return c;
    }
    return -1;
}

// Removes the person at 'index' and shifts everyone behind them forward.
void removeAt(Service& s, int index) {
    for (int i = index; i < s.queueSize - 1; i++) {
        s.queue[i] = s.queue[i + 1];
    }
    s.queueSize--;
}

// Looks for a student ID in all queues. Fills svc/pos if found.
bool findPerson(int id, int& svc, int& pos) {
    for (int s = 0; s < MAX_SERVICES; s++) {
        for (int i = 0; i < services[s].queueSize; i++) {
            if (services[s].queue[i].id == id) {
                svc = s;
                pos = i;
                return true;
            }
        }
    }
    return false;
}

// Adds one record to the history. When the history is full, the OLDEST
// record is dropped so the newest data is always kept.
bool addRecord(int service, int hour, double wait) {
    if (historyCount >= MAX_HISTORY) {
        for (int i = 1; i < historyCount; i++) history[i - 1] = history[i];
        historyCount--;
    }
    history[historyCount].service = service;
    history[historyCount].hour = hour;
    history[historyCount].wait = wait;
    historyCount++;
    return true;
}

// Adds a line to the activity feed (newest first, oldest falls off the end).
void logActivity(const string& text) {
    int last = (activityCount < MAX_ACTIVITY) ? activityCount : MAX_ACTIVITY - 1;
    for (int i = last; i > 0; i--) activities[i] = activities[i - 1];
    activities[0].time = clockNow();
    activities[0].text = text;
    if (activityCount < MAX_ACTIVITY) activityCount++;
}

// Puts a person at the end of a queue. Returns false if the service is closed
// or the queue is full. 'ticket' receives the saved person.
bool joinPerson(int svc, const string& name, int id, Person& ticket) {
    Service& sv = services[svc];
    if (sv.counters <= 0 || sv.queueSize >= MAX_QUEUE) return false;

    ticket.name = name;
    ticket.id = id;
    ticket.joinHour = currentHour();
    ticket.estWait = waitOf(sv);             // people ahead = current queue size
    sv.queue[sv.queueSize] = ticket;
    sv.queueSize++;
    logActivity(name + " joined " + sv.name + " (#" + to_string(sv.queueSize) + ")");
    return true;
}

// Serves the first person in the queue and logs their wait for the analytics.
// Returns false if nobody is waiting or no counter is open.
bool serveNext(int svc, Person& served) {
    Service& sv = services[svc];
    if (sv.queueSize == 0 || sv.counters == 0) return false;

    served = sv.queue[0];
    addRecord(svc, served.joinHour, served.estWait);
    removeAt(sv, 0);
    logActivity(sv.name + " served " + served.name);
    return true;
}
