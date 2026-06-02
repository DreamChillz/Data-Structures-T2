#pragma once
#include <iostream>
#include <string>

using namespace std;

// ---------------------------------------------------------
// CUSTOM DATA STRUCTURE: LINKED-LIST BASED STACK
// ---------------------------------------------------------
struct NavNode {
    string direction;
    NavNode* next;
    
    NavNode(string dir) {
        direction = dir;
        next = nullptr;
    }
};

class NavigationStack {
private:
    NavNode* top;
public:
    NavigationStack() { top = nullptr; }

    void push(string dir) {
        NavNode* newNode = new NavNode(dir);
        newNode->next = top;
        top = newNode;
    }

    string pop() {
        if (isEmpty()) return "";
        NavNode* temp = top;
        string dir = temp->direction;
        top = top->next;
        delete temp;
        return dir;
    }

    bool isEmpty() { return top == nullptr; }

    void clear() {
        while (!isEmpty()) {
            pop();
        }
    }
};

// ---------------------------------------------------------
// MODULE VARIABLES
// ---------------------------------------------------------
NavigationStack nav_robotPath;
string nav_movementLog[200]; 
int nav_logCount = 0;
bool nav_isReturned = false;

// Helper function to calculate the opposite direction
string getReverseDirection(string dir) {
    if (dir == "Forward") return "Backward";
    if (dir == "Backward") return "Forward";
    if (dir == "Turn Left") return "Turn Right";
    if (dir == "Turn Right") return "Turn Left";
    return dir; 
}

// ---------------------------------------------------------
// INTEGRATION FUNCTION (For Option 6: Full System Run)
// ---------------------------------------------------------
// Your teammates will call this function to instantly load the path
void translateAndLoadPath(string locationPath[], int pathLength) {
    // 1. Clear previous logs if a previous job was finished
    if (nav_isReturned) {
        nav_robotPath.clear();
        nav_logCount = 0;
        nav_isReturned = false;
    }

    cout << "\n[Robot Navigation] Translating map route into physical movements...\n";

    // 2. Translate locations and push to Stack
    // We start at i = 1 to skip "Main Warehouse" (robot is already there)
    for (int i = 1; i < pathLength; i++) {
        if (nav_logCount >= 200) {
            cout << "Navigation log full! Stopping early.\n";
            break;
        }

        string loc = locationPath[i];
        string physicalDir = "";

        // Translation Rules
        if (loc.find("Zone") != string::npos) {
            physicalDir = "Forward";
        } 
        else if (loc.find("A1") != string::npos || loc.find("B1") != string::npos || loc.find("C1") != string::npos) {
            if (loc.find("Aisle") != string::npos) physicalDir = "Turn Left";
            else if (loc.find("Shelf") != string::npos) physicalDir = "Forward";
        } 
        else if (loc.find("A2") != string::npos || loc.find("C2") != string::npos) {
            if (loc.find("Aisle") != string::npos) physicalDir = "Turn Right";
            else if (loc.find("Shelf") != string::npos) physicalDir = "Forward";
        }

        // Push to Stack and Log
        if (physicalDir != "") {
            nav_robotPath.push(physicalDir);
            nav_movementLog[nav_logCount++] = physicalDir;
            cout << " -> Translated [" << loc << "] into: " << physicalDir << "\n";
        }
    }
    
    cout << "[Robot Navigation] Route translated and loaded. Robot has reached the item.\n";
}

// ---------------------------------------------------------
// CORE MANUAL FUNCTIONS (For Your Individual Presentation)
// ---------------------------------------------------------
void recordMovement() {
    if (nav_isReturned) {
        cout << "\n[System] Robot previously returned. Resetting logs for new journey...\n";
        nav_robotPath.clear();
        nav_logCount = 0;
        nav_isReturned = false;
    }

    if (nav_logCount >= 200) {
        cout << "Navigation log full.\n";
        return;
    }

    int dirChoice;
    cout << "\n--- Record Movement Step ---\n";
    cout << "1. Forward\n2. Backward\n3. Turn Left\n4. Turn Right\nSelect direction: ";
    cin >> dirChoice;

    string dir = "";
    switch(dirChoice) {
        case 1: dir = "Forward"; break;
        case 2: dir = "Backward"; break;
        case 3: dir = "Turn Left"; break;
        case 4: dir = "Turn Right"; break;
        default: cout << "Invalid choice.\n"; return;
    }

    nav_robotPath.push(dir);
    nav_movementLog[nav_logCount++] = dir;
    cout << "Success: Movement '" << dir << "' recorded.\n";
}

void displayForwardPath() {
    if (nav_logCount == 0) {
        cout << "\nNo movements recorded yet.\n";
        return;
    }
    cout << "\n--- Forward Path to Destination ---\n";
    for (int i = 0; i < nav_logCount; i++) {
        cout << "Step " << i + 1 << ": " << nav_movementLog[i] << "\n";
    }
}

void returnUsingReversePath() {
    if (nav_robotPath.isEmpty()) {
        cout << "\nRobot is already at base or no path recorded.\n";
        return;
    }

    cout << "\n--- Initiating Return Journey ---\n";
    int step = 1;
    
    while (!nav_robotPath.isEmpty()) {
        string originalDir = nav_robotPath.pop();
        string reverseDir = getReverseDirection(originalDir);
        cout << "Return Step " << step++ << ": " << reverseDir 
             << " \t(Reversing: " << originalDir << ")\n";
    }
    
    cout << "\nSuccess: Robot safely retraced its steps and returned to the station!\n";
    nav_isReturned = true; 
}

void displayNavigationLog() {
    cout << "\n--- Complete Navigation Log ---\n";
    if (nav_logCount == 0) {
        cout << "No logs available.\n";
        return;
    }
    cout << "[Forward Journey Logs]\n";
    for (int i = 0; i < nav_logCount; i++) {
        cout << "  -> " << nav_movementLog[i] << "\n";
    }
    cout << "\n[Current Status]\n";
    if (nav_isReturned) cout << "  Status: Task finished. Return journey completed.\n";
    else cout << "  Status: Robot is currently navigating (Pending return).\n";
}

void robotNavigationMenu() {
    int choice;
    do {
        cout << "\n--- Robot Navigation Menu ---\n";
        cout << "1. Record Movement Step (Manual)\n";
        cout << "2. Display Forward Path\n";
        cout << "3. Return Using Reverse Path\n";
        cout << "4. Display Navigation Log\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: recordMovement(); break;
            case 2: displayForwardPath(); break;
            case 3: returnUsingReversePath(); break;
            case 4: displayNavigationLog(); break;
            case 0: cout << "Returning to main menu...\n"; break;
            default: cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);
}
