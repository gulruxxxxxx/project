// ============================================================================
//  menus.cpp - everything the user sees and types
// ============================================================================
#include "queueless.h"

// Table with all services.
void printOverview() {
    cout << "\n " << BOLD << left << setw(4) << "#" << setw(18) << "SERVICE"
         << setw(10) << "WAITING" << setw(10) << "COUNTERS" << setw(13) << "EST. WAIT"
         << "ADVICE" << RESET << "\n";
    for (int s = 0; s < MAX_SERVICES; s++) {
        double wait = waitOf(services[s]);
        string advice = adviceFor(services[s]);
        cout << " " << left << setw(4) << (s + 1) << setw(18) << services[s].name
             << setw(10) << services[s].queueSize << setw(10) << services[s].counters;
        cout << colorFor(advice) << setw(13) << ((wait < 0) ? string("--") : fmt1(wait) + " min")
             << advice << RESET << "\n";
    }
    int best = bestService();
    if (best >= 0) {
        cout << "\n " << CYAN << "Tip: " << services[best].name << " has the shortest wait right now ("
             << fmt1(waitOf(services[best])) << " min)." << RESET << "\n";
    }
}

// ---- 1: CHECK WAIT TIME ----
void checkWaitTime() {
    printHeader("CHECK WAIT TIME");
    printOverview();
    int c = readInt("\nSelect a service for details (0 = back): ", 0, MAX_SERVICES);
    if (c == 0) return;

    const Service& sv = services[c - 1];
    double wait = waitOf(sv);
    string advice = adviceFor(sv);
    cout << "\n " << BOLD << "SERVICE: " << sv.name << RESET << "\n";
    cout << " People ahead     : " << sv.queueSize << "\n";
    cout << " Average service  : " << fmt1(sv.avgMinutes) << " min/person\n";
    cout << " Active counters  : " << sv.counters << "\n";
    if (wait < 0) {
        cout << " " << RED << "No counter is open right now. Please check back later." << RESET << "\n";
    } else {
        cout << " " << BOLD << "ESTIMATED WAIT   : " << fmt1(wait) << " minutes" << RESET << "\n";
        cout << " Formula          : " << sv.queueSize << " people x " << fmt1(sv.avgMinutes)
             << " min / " << sv.counters << " counter(s)\n";
        cout << " Recommendation   : " << BOLD << colorFor(advice) << advice << RESET << "\n";
    }
    pauseScreen();
}

// ---- 2: JOIN A QUEUE ----
void joinQueue() {
    printHeader("JOIN A QUEUE");
    printOverview();
    int c = readInt("\nWhich service? (0 = back): ", 0, MAX_SERVICES);
    if (c == 0) return;

    Service& sv = services[c - 1];
    if (sv.counters <= 0) {
        cout << RED << "  " << sv.name << " has no open counter. You cannot join right now." << RESET << "\n";
        pauseScreen();
        return;
    }
    if (sv.queueSize >= MAX_QUEUE) {
        cout << RED << "  This queue is full (" << MAX_QUEUE << " people)." << RESET << "\n";
        pauseScreen();
        return;
    }

    string name = readName("Your name: ");
    int id = readInt("Your student ID (6 digits): ", 100000, 999999);

    int svc, pos;
    if (findPerson(id, svc, pos)) {
        cout << RED << "  ID " << id << " is already waiting in " << services[svc].name
             << " (position " << (pos + 1) << ")." << RESET << "\n";
        pauseScreen();
        return;
    }

    Person ticket;
    joinPerson(c - 1, name, id, ticket);
    saveAll();

    string advice = adviceForWait(ticket.estWait);
    cout << "\n " << GREEN << BOLD << "+------------------------------------+\n"
         << " |            YOUR TICKET             |\n"
         << " +------------------------------------+" << RESET << "\n";
    cout << "  Name      : " << ticket.name << "\n";
    cout << "  ID        : " << ticket.id << "\n";
    cout << "  Service   : " << sv.name << "\n";
    cout << "  Position  : #" << sv.queueSize << "\n";
    cout << "  Est. wait : " << fmt1(ticket.estWait) << " min   (" << colorFor(advice) << advice << RESET << ")\n";
    cout << "  Be back at: about " << clockIn(ticket.estWait) << "\n";
    cout << GRAY << "  Tip: use 'Track my place' any time to see your live position." << RESET << "\n";
    pauseScreen();
}

// ---- 3: TRACK MY PLACE (live position + ETA) ----
void trackMyPlace() {
    printHeader("TRACK MY PLACE");
    int id = readInt("Your student ID (6 digits): ", 100000, 999999);
    int svc, pos;
    if (!findPerson(id, svc, pos)) {
        cout << RED << "  ID " << id << " is not in any queue." << RESET << "\n";
        pauseScreen();
        return;
    }
    const Service& sv = services[svc];
    const Person& me = sv.queue[pos];
    double eta = positionWait(sv, pos);

    cout << "\n " << BOLD << me.name << RESET << " in " << BOLD << sv.name << RESET << "\n";
    cout << " Position     : #" << (pos + 1) << " of " << sv.queueSize << "\n";
    cout << " People ahead : " << pos << "\n";

    // little picture of the line:  o o o o o YOU
    cout << " The line     : ";
    int shown = (pos < 30) ? pos : 30;
    cout << GRAY;
    for (int i = 0; i < shown; i++) cout << "o ";
    if (pos > shown) cout << "(+" << (pos - shown) << ") ";
    cout << RESET << GREEN << BOLD << "YOU" << RESET << "\n";

    if (eta < 0) {
        cout << " " << RED << "All counters are closed right now - the wait is paused." << RESET << "\n";
    } else if (pos == 0) {
        cout << " " << GREEN << BOLD << "You are next! Please go to the counter." << RESET << "\n";
    } else {
        cout << " Est. wait    : " << BOLD << fmt1(eta) << " minutes" << RESET
             << "  (about " << clockIn(eta) << ")\n";
    }
    pauseScreen();
}

// ---- 4: LEAVE A QUEUE ----
void leaveQueue() {
    printHeader("LEAVE A QUEUE");
    int id = readInt("Enter your student ID (6 digits): ", 100000, 999999);
    int svc, pos;
    if (!findPerson(id, svc, pos)) {
        cout << RED << "  ID " << id << " is not in any queue." << RESET << "\n";
    } else {
        string who = services[svc].queue[pos].name;
        removeAt(services[svc], pos);
        logActivity(who + " left " + services[svc].name);
        saveAll();
        cout << GREEN << "  " << who << " left the " << services[svc].name << " queue." << RESET << "\n";
    }
    pauseScreen();
}

// ---- 5: BEST TIME TO VISIT (uses the history) ----
void bestTimeMenu() {
    printHeader("BEST TIME TO VISIT");
    if (historyCount == 0) {
        cout << "  No history yet, so we cannot predict a good time.\n"
             << "  Tip: main menu 9 -> 'Generate a busy day' creates sample data.\n";
        pauseScreen();
        return;
    }
    for (int s = 0; s < MAX_SERVICES; s++) cout << "  " << (s + 1) << ". " << services[s].name << "\n";
    int c = readInt("\nWhich service? (0 = back): ", 0, MAX_SERVICES);
    if (c == 0) return;

    int s = c - 1;
    double avg[24];
    int cnt[24];
    hourlyAverages(s, avg, cnt);
    int quiet = quietestHour(avg, cnt);
    int peak = peakHour(avg, cnt);
    if (quiet == -1) {
        cout << "  No data for " << services[s].name << " yet.\n";
        pauseScreen();
        return;
    }
    double now = waitOf(services[s]);

    cout << "\n " << BOLD << services[s].name << RESET << "\n";
    cout << " Quietest hour : " << GREEN << BOLD << pad2(quiet) << ":00" << RESET
         << "  (about " << fmt1(avg[quiet]) << " min)\n";
    cout << " Busiest hour  : " << RED << BOLD << pad2(peak) << ":00" << RESET
         << "  (about " << fmt1(avg[peak]) << " min)\n";
    if (now < 0) {
        cout << " Right now     : closed\n";
    } else {
        cout << " Right now     : " << fmt1(now) << " min\n";
        if (now > avg[quiet] * 1.5 + 1) {
            cout << "\n " << CYAN << "Tip: coming at " << pad2(quiet) << ":00 could save you about "
                 << fmt1(now - avg[quiet]) << " minutes." << RESET << "\n";
        } else {
            cout << "\n " << GREEN << "Right now is already a good time to go." << RESET << "\n";
        }
    }
    pauseScreen();
}

// ---- 6: PEAK-HOUR ANALYTICS ----
void analyticsMenu() {
    printHeader("PEAK-HOUR ANALYTICS");
    if (historyCount == 0) {
        cout << "  No history yet.\n"
             << "  Tip: main menu 9 -> 'Generate a busy day' creates sample data.\n";
        pauseScreen();
        return;
    }
    cout << "  History records: " << historyCount << "\n\n";
    for (int s = 0; s < MAX_SERVICES; s++) {
        cout << "  " << (s + 1) << ". " << services[s].name << "\n";
    }
    cout << "  " << (MAX_SERVICES + 1) << ". Summary of all services\n";
    int c = readInt("\nChoose (0 = back): ", 0, MAX_SERVICES + 1);
    if (c == 0) return;
    if (c == MAX_SERVICES + 1) showAnalyticsSummary();
    else showServiceAnalytics(c - 1);
    pauseScreen();
}

// ---- 7: COUNTER SIMULATION (our own feature) ----
void counterSimulation() {
    printHeader("COUNTER SIMULATION");
    printOverview();
    int c = readInt("\nWhich service? (0 = back): ", 0, MAX_SERVICES);
    if (c == 0) return;

    const Service& sv = services[c - 1];
    cout << "\n " << BOLD << "COUNTER SIMULATION - " << sv.name << RESET << "\n";
    cout << " People waiting  : " << sv.queueSize << "\n";
    cout << " Current counters: " << sv.counters << "\n";
    if (sv.counters >= MAX_COUNTERS) {
        cout << " " << YELLOW << "This service already has the maximum of " << MAX_COUNTERS
             << " counters." << RESET << "\n";
        pauseScreen();
        return;
    }

    double current = waitOf(sv);
    if (current < 0) cout << " Current wait    : " << RED << "CLOSED (0 counters)" << RESET << "\n";
    else cout << " Current wait    : " << fmt1(current) << " min\n";

    int extra = readInt("\nHow many extra counters to test? ", 1, MAX_COUNTERS - sv.counters);
    double after = calcWait(sv.queueSize, sv.avgMinutes, sv.counters + extra);
    cout << "\n With +" << extra << " counter(s): " << BOLD << fmt1(after) << " min" << RESET << "\n";
    if (current >= 0) {
        cout << " Time saved       : " << GREEN << BOLD << fmt1(current - after) << " min" << RESET << "\n";
    }

    if (current < 0 || current > SHORT_WAIT_MAX) {
        int needed = countersNeeded(sv, SHORT_WAIT_MAX);
        if (needed > 0) {
            cout << "\n " << CYAN << "To get under " << (int)SHORT_WAIT_MAX << " minutes, "
                 << sv.name << " needs " << needed << " counters in total." << RESET << "\n";
        } else {
            cout << "\n " << YELLOW << "Even " << MAX_COUNTERS << " counters cannot get this queue under "
                 << (int)SHORT_WAIT_MAX << " minutes." << RESET << "\n";
        }
    }
    pauseScreen();
}

// ---- 8: STAFF PANEL (PIN protected) ----
static void resetToSampleData() {
    loadDefaults();
    historyCount = 0;
    activityCount = 0;
    logActivity("All data reset to sample data");
    saveAll();
}

void staffPanel() {
    printHeader("STAFF PANEL");
    bool ok = false;
    for (int tries = 0; tries < 3 && !ok; tries++) {
        cout << "Enter staff PIN: ";
        string pin;
        if (!getline(cin, pin)) inputClosed();
        if (trim(pin) == STAFF_PIN) ok = true;
        else cout << RED << "  Wrong PIN." << RESET << "\n";
    }
    if (!ok) {
        pauseScreen();
        return;
    }

    int choice;
    do {
        printOverview();
        cout << "\n " << BOLD << "STAFF OPTIONS" << RESET << "\n"
             << "  1. Serve next person\n"
             << "  2. Open a counter\n"
             << "  3. Close a counter\n"
             << "  4. View a queue\n"
             << "  5. Clear history\n"
             << "  6. Reset everything to sample data\n"
             << "  0. Back to main menu\n";
        choice = readInt("Choose: ", 0, 6);

        switch (choice) {
            case 1: {
                int c = readInt("Service (1-4, 0 = cancel): ", 0, MAX_SERVICES);
                if (c == 0) break;
                Service& sv = services[c - 1];
                Person p;
                if (sv.queueSize == 0) {
                    cout << YELLOW << "  Nobody is waiting in " << sv.name << "." << RESET << "\n";
                } else if (sv.counters == 0) {
                    cout << RED << "  Open a counter first." << RESET << "\n";
                } else if (serveNext(c - 1, p)) {
                    saveAll();
                    cout << GREEN << "  Now serving: " << p.name << " (ID " << p.id << "). "
                         << sv.queueSize << " still waiting." << RESET << "\n";
                }
                break;
            }
            case 2:
            case 3: {
                int c = readInt("Service (1-4, 0 = cancel): ", 0, MAX_SERVICES);
                if (c == 0) break;
                Service& sv = services[c - 1];
                if (choice == 2) {
                    if (sv.counters >= MAX_COUNTERS) {
                        cout << YELLOW << "  Maximum of " << MAX_COUNTERS << " counters reached." << RESET << "\n";
                        break;
                    }
                    sv.counters++;
                    logActivity(sv.name + ": counter opened (" + to_string(sv.counters) + " open)");
                } else {
                    if (sv.counters == 0) {
                        cout << YELLOW << "  All counters are already closed." << RESET << "\n";
                        break;
                    }
                    sv.counters--;
                    logActivity(sv.name + ": counter closed (" + to_string(sv.counters) + " open)");
                }
                saveAll();
                cout << GREEN << "  " << sv.name << " now has " << sv.counters << " counter(s)." << RESET << "\n";
                if (sv.counters == 0) cout << YELLOW << "  The service is now CLOSED." << RESET << "\n";
                break;
            }
            case 4: {
                int c = readInt("Service (1-4, 0 = cancel): ", 0, MAX_SERVICES);
                if (c == 0) break;
                const Service& sv = services[c - 1];
                cout << "\n " << BOLD << sv.name << " - " << sv.queueSize << " waiting" << RESET << "\n";
                for (int i = 0; i < sv.queueSize; i++) {
                    cout << "  " << right << setw(3) << (i + 1) << ". " << left << setw(30)
                         << sv.queue[i].name << " ID " << sv.queue[i].id << "\n";
                }
                if (sv.queueSize == 0) cout << "  (empty)\n";
                break;
            }
            case 5:
                historyCount = 0;
                logActivity("History cleared");
                saveAll();
                cout << GREEN << "  History cleared." << RESET << "\n";
                break;
            case 6:
                if (readInt("Reset ALL data? (1 = yes, 0 = no): ", 0, 1) == 1) {
                    resetToSampleData();
                    cout << GREEN << "  Everything reset to the sample data." << RESET << "\n";
                }
                break;
            default:
                break;
        }
        if (choice != 0) pauseScreen();
    } while (choice != 0);
}

// ---- 9: DEMO TOOLS ----
void demoMenu() {
    printHeader("DEMO TOOLS");
    cout << "  1. Generate a busy day of history (fills the analytics)\n"
         << "  2. Live demo - " << DEMO_SECONDS << " seconds of students arriving and being served\n"
         << "  0. Back\n";
    int c = readInt("Choose: ", 0, 2);
    if (c == 1) simulateDay();
    else if (c == 2) liveDemo();
    if (c != 0) pauseScreen();
}

// ---- Main menu ----
void showMainMenu() {
    clearScreen();
    cout << CYAN << BOLD
         << "\n   ___                       _\n"
         << "  / _ \\ _   _  ___ _   _  __| | ___  ___ ___\n"
         << " | | | | | | |/ _ \\ | | |/ _` |/ _ \\/ __/ __|\n"
         << " | |_| | |_| |  __/ |_| | (_| |  __/\\__ \\__ \\\n"
         << "  \\__\\_\\\\__,_|\\___|\\__,_|\\__,_|\\___||___/___/\n"
         << RESET << GRAY << "  Know your wait before you queue.   " << nowString() << RESET << "\n";
    printOverview();
    cout << "\n " << BOLD << "MAIN MENU" << RESET << "\n"
         << "  1. Check wait times          6. Peak-hour analytics\n"
         << "  2. Join a queue              7. Counter simulation\n"
         << "  3. Track my place            8. Staff panel (PIN)\n"
         << "  4. Leave a queue             9. Demo tools\n"
         << "  5. Best time to visit       10. Open web dashboard\n"
         << "  0. Exit\n\n";
}
