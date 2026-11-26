// ============================================================================
//  demo.cpp - demo tools: generate a busy day, and a live traffic simulation
// ============================================================================
#include "queueless.h"

// How busy a service is at a given hour (each service has its own rush hour).
int peakWeight(int service, int hour) {
    int centers[MAX_SERVICES] = {11, 10, 13, 12};   // rush hour of each service
    int weight = 10 - 3 * abs(hour - centers[service]);
    if (weight < 1) weight = 1;
    return weight;
}

// Creates a day of realistic sample history using rand().
void simulateDay() {
    printHeader("GENERATE A BUSY DAY");
    cout << "  Creating sample waiting times for 08:00 - 18:00 ...\n";
    int scale[MAX_SERVICES] = {3, 3, 3, 5};   // how crowded each service gets at rush hour
    int added = 0;
    for (int s = 0; s < MAX_SERVICES; s++) {
        int counters = (services[s].counters > 0) ? services[s].counters : 1;
        for (int h = OPEN_HOUR; h < CLOSE_HOUR; h++) {
            for (int k = 0; k < SAMPLES_PER_HOUR; k++) {
                int people = peakWeight(s, h) * scale[s] + rand() % 5 - 2;
                if (people < 1) people = 1;
                addRecord(s, h, calcWait(people, services[s].avgMinutes, counters));
                added++;
            }
        }
    }
    logActivity("Generated " + to_string(added) + " history records");
    saveAll();
    cout << GREEN << "  Added " << added << " records." << RESET
         << " Open 'Peak-hour analytics' or the web dashboard to see the result!\n";
}

// LIVE DEMO: for DEMO_SECONDS seconds students arrive and get served at random.
// Every second the files are saved, so the web dashboard moves while you talk.
void liveDemo() {
    printHeader("LIVE DEMO");
    cout << "  Simulating " << DEMO_SECONDS << " seconds of campus traffic.\n"
         << "  Open the web dashboard (main menu 10) and watch it move.\n"
         << "  Ctrl+C stops it early - data is saved every second.\n\n";

    int arrivalChance[MAX_SERVICES] = {55, 60, 65, 70};   // % chance a student arrives each second
    int serveChance = 25;                                 // % chance an open counter finishes a student

    for (int tick = 1; tick <= DEMO_SECONDS; tick++) {
        for (int s = 0; s < MAX_SERVICES; s++) {
            // 1) maybe a new student arrives
            if (rand() % 100 < arrivalChance[s]) {
                int id = 700000 + rand() % 30000;      // demo guests use IDs 700000-729999
                int a, b;
                while (findPerson(id, a, b)) id++;     // make sure the ID is unique
                Person ticket;
                string name = "Guest " + to_string(1 + rand() % 500);
                if (joinPerson(s, name, id, ticket)) {
                    cout << "  " << GRAY << clockNow() << RESET << "  " << GREEN << "+ " << RESET
                         << name << " joined " << services[s].name << "\n";
                }
            }
            // 2) each open counter may finish serving someone
            for (int c = 0; c < services[s].counters; c++) {
                Person served;
                if (rand() % 100 < serveChance && serveNext(s, served)) {
                    cout << "  " << GRAY << clockNow() << RESET << "  " << CYAN << "> " << RESET
                         << services[s].name << " served " << served.name << "\n";
                }
            }
        }
        saveAll();
        sleepMs(1000);
    }
    cout << GREEN << "\n  Live demo finished." << RESET << "\n";
}
