// ============================================================================
//  QueueLess - campus waiting-time manager      main.cpp
// ============================================================================
#include "queueless.h"

int main() {
    enableColors();
    srand((unsigned)time(0));
    cout << fixed << setprecision(1);

    // Load saved data. If the file is missing or damaged, start with sample data.
    if (!loadQueues()) {
        loadDefaults();
        cout << YELLOW << "No valid save file found - starting with sample data." << RESET << "\n";
    }
    loadHistory();
    logActivity("QueueLess started");
    saveAll();

    ifstream web(WEB_PAGE_FILE.c_str());
    if (!web) {
        cout << YELLOW << "Note: web/index.html not found. Run the program from the project folder"
             << " to use the web dashboard." << RESET << "\n";
        pauseScreen();
    }

    int choice;
    do {
        showMainMenu();
        choice = readInt("Choose (0-10): ", 0, 10);
        switch (choice) {
            case 1:  checkWaitTime();     break;
            case 2:  joinQueue();         break;
            case 3:  trackMyPlace();      break;
            case 4:  leaveQueue();        break;
            case 5:  bestTimeMenu();      break;
            case 6:  analyticsMenu();     break;
            case 7:  counterSimulation(); break;
            case 8:  staffPanel();        break;
            case 9:  demoMenu();          break;
            case 10: openDashboard(); pauseScreen(); break;
            case 0:  break;
        }
    } while (choice != 0);

    saveAll();
    cout << "\n" << GREEN << "Data saved. Thank you for using QueueLess!" << RESET << "\n";
    return 0;
}
