// ============================================================================
//  analytics.cpp - peak-hour statistics from the saved history
// ============================================================================
#include "queueless.h"

// Average wait per hour for one service (arrays avg[24] and cnt[24] are filled in).
void hourlyAverages(int service, double avg[], int cnt[]) {
    double total[24];
    for (int h = 0; h < 24; h++) {
        total[h] = 0;
        cnt[h] = 0;
    }
    for (int i = 0; i < historyCount; i++) {
        if (history[i].service == service) {
            total[history[i].hour] += history[i].wait;
            cnt[history[i].hour]++;
        }
    }
    for (int h = 0; h < 24; h++) {
        avg[h] = (cnt[h] > 0) ? total[h] / cnt[h] : 0;
    }
}

// Busiest hour = hour with the highest average wait (-1 if no data).
int peakHour(const double avg[], const int cnt[]) {
    int best = -1;
    for (int h = 0; h < 24; h++) {
        if (cnt[h] > 0 && (best == -1 || avg[h] > avg[best])) best = h;
    }
    return best;
}

// Quietest opening hour = lowest average wait with data (-1 if no data).
int quietestHour(const double avg[], const int cnt[]) {
    int best = -1;
    for (int h = OPEN_HOUR; h < CLOSE_HOUR; h++) {
        if (cnt[h] > 0 && (best == -1 || avg[h] < avg[best])) best = h;
    }
    return best;
}

int countRecords(int service) {
    int n = 0;
    for (int i = 0; i < historyCount; i++) {
        if (history[i].service == service) n++;
    }
    return n;
}

double overallAverage(int service) {
    double total = 0;
    int n = 0;
    for (int i = 0; i < historyCount; i++) {
        if (history[i].service == service) {
            total += history[i].wait;
            n++;
        }
    }
    return (n > 0) ? total / n : 0;
}

void showServiceAnalytics(int s) {
    double avg[24];
    int cnt[24];
    hourlyAverages(s, avg, cnt);
    int peak = peakHour(avg, cnt);
    int quiet = quietestHour(avg, cnt);

    printHeader("PEAK HOURS - " + services[s].name);
    if (peak == -1) {
        cout << "  No data for this service yet.\n";
        return;
    }
    cout << "  (each # = 1 minute of average waiting time)\n\n";
    for (int h = 0; h < 24; h++) {
        bool inOpeningHours = (h >= OPEN_HOUR && h < CLOSE_HOUR);
        if (cnt[h] == 0 && !inOpeningHours) continue;
        cout << "  " << pad2(h) << ":00  ";
        if (cnt[h] == 0) {
            cout << GRAY << "no data" << RESET << "\n";
            continue;
        }
        printBar(avg[h], 40);
        cout << " " << fmt1(avg[h]) << " min";
        if (h == peak) cout << BOLD << RED << "   <-- PEAK" << RESET;
        if (h == quiet) cout << BOLD << GREEN << "   <-- QUIETEST" << RESET;
        cout << "\n";
    }
}

void showAnalyticsSummary() {
    printHeader("PEAK HOURS - ALL SERVICES");
    cout << BOLD << left << setw(18) << "  SERVICE" << setw(14) << "BUSIEST HOUR"
         << setw(16) << "WAIT AT PEAK" << setw(14) << "AVG WAIT" << "RECORDS" << RESET << "\n";
    for (int s = 0; s < MAX_SERVICES; s++) {
        double avg[24];
        int cnt[24];
        hourlyAverages(s, avg, cnt);
        int peak = peakHour(avg, cnt);
        cout << "  " << left << setw(16) << services[s].name;
        if (peak == -1) {
            cout << GRAY << "no data" << RESET << "\n";
            continue;
        }
        cout << setw(14) << (pad2(peak) + ":00")
             << setw(16) << (fmt1(avg[peak]) + " min")
             << setw(14) << (fmt1(overallAverage(s)) + " min")
             << countRecords(s) << "\n";
    }
}
