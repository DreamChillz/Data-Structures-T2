#pragma once
#include <iostream>
#include <string>

using namespace std;

//linked-list stack
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

// module variables
NavigationStack nav_robotPath;
string nav_movementLog[200]; 
int nav_logCount = 0;
bool nav_isReturned = false;

// Helper function to calculate the opposite direction
string getReverseDirection(string dir) {
    if (dir == "Forward (North)") return "Backward (South)";
    if (dir == "Backward (South)") return "Forward (North)";
    if (dir == "Turn Left (West)") return "Turn Right (East)";
    if (dir == "Turn Right (East)") return "Turn Left (West)";
    
    // Fallbacks for the automated system integration
    if (dir == "Forward") return "Backward";
    if (dir == "East") return "West";
    if (dir == "West") return "East";
    if (dir == "South") return "North";
    if (dir == "North") return "South";
    return dir; 
}

// integration function (For Option 6: Full System Run)
void loadDirectionsDirectly(string directions[], int dirLength) {
    if (nav_isReturned) {
        nav_robotPath.clear();
        nav_logCount = 0;
        nav_isReturned = false;
    }

    cout << "\n[Robot Navigation] Loading calculated directions into robot memory...\n";
    for (int i = 0; i < dirLength; i++) {
        if (nav_logCount >= 200) break;

        string physicalDir = directions[i];
        if (physicalDir != "Stay") {
            nav_robotPath.push(physicalDir);
            nav_movementLog[nav_logCount++] = physicalDir;
            cout << " -> Loaded direction: " << physicalDir << "\n";
        }
    }
    cout << "[Robot Navigation] Route loaded. Robot has reached the destination.\n";
}

// Allows main.cpp to feed directions one-by-one for real-time simulation (to simulate obstacle )
void executeSingleStep(string physicalDir) {
    if (nav_logCount >= 200 || physicalDir == "Stay") return;
    nav_robotPath.push(physicalDir);
    nav_movementLog[nav_logCount++] = physicalDir;
    cout << " -> Executed: " << physicalDir << "\n";
}

// Allows main.cpp to trigger the obstacle event mid-journey
void triggerObstacle() {
    if (nav_robotPath.isEmpty()) return;
    
    cout << "\n[SENSOR ALERT] Unmapped obstacle detected in path.\n";
    cout << "[System] Backtracking to safe node...\n";
    
    string badMove = nav_robotPath.pop();
    string correctionMove = getReverseDirection(badMove);
    
    cout << " -> Evasive Action: " << correctionMove << " (Reversing blocked step: " << badMove << ")\n";
    nav_movementLog[nav_logCount++] = " Backtracked: " + correctionMove;
}

// Individual Demo
void simulateRouteExecution() {
    // Reset if starting a fresh simulation
    if (nav_isReturned) {
        cout << "\n[System] Robot previously returned. Resetting memory logs for new task...\n";
        nav_robotPath.clear();
        nav_logCount = 0;
        nav_isReturned = false;
    }

    int dirChoice;
    do {
        cout << "[ CURRENT MOVEMENT MEMORY STACK ]\n";
        
        if (nav_logCount == 0) {
            cout << " (Stack is Empty - Robot is at Start Point)\n";
        } else {
            for (int i = 0; i < nav_logCount; i++) {
                cout << "  Step " << i + 1 << ": " << nav_movementLog[i] << "\n";
            }
        }
        cout << "--------------------------------------------------\n";

        // Original menu choices
        cout << "1. Step Forward (North)\n";
        cout << "2. Step Backward (South)\n";
        cout << "3. Turn Left (West)\n";
        cout << "4. Turn Right (East)\n";
        cout << "5. Undo Last Move (Obstacle OR Incorrect Path)\n";
        cout << "0. End Simulation & Return to Menu\n";
        cout << "Select system event: ";
        cin >> dirChoice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        string dir = "";
        switch(dirChoice) {
            case 1: dir = "Forward (North)"; break;
            case 2: dir = "Backward (South)"; break;
            case 3: dir = "Turn Left (West)"; break;
            case 4: dir = "Turn Right (East)"; break;
            case 5: 
                // Undo
                if (nav_robotPath.isEmpty()) {
                    cout << "\n[System] Robot is at the Warehouse. Cannot Reverse Any Further.\n";
                } else {
                    cout << "\n Obstacle Detected OR Incorrect Path Taken\n";
                    cout << "[System] Backtracking to safe node...\n";
                    
                    string badMove = nav_robotPath.pop();
                    string correctionMove = getReverseDirection(badMove);
                    
                    cout << " -> Evasive Action: " << correctionMove << " (Reversing blocked step: " << badMove << ")\n";
                    nav_movementLog[nav_logCount++] = " Backtracked: " + correctionMove;
                }
                continue; // Skip the standard push logic below since dodging
            case 0:
                cout << "Exiting Route Simulator...\n";
                continue;
            default:
                cout << "Invalid choice.\n";
                continue;
        }

        // Standard movement logging
        if (nav_logCount >= 200) {
            cout << "Navigation memory full.\n";
            continue;
        }
        nav_robotPath.push(dir);
        nav_movementLog[nav_logCount++] = dir;
        cout << "[System] Step recorded and executed: " << dir << "\n";

    } while (dirChoice != 0);
}

void displayForwardPath() {
    if (nav_logCount == 0) {
        cout << "\nNo movements recorded yet.\n";
        return;
    }
    cout << "\n--- Current Path Memory in Robot --- \n";
    for (int i = 0; i < nav_logCount; i++) {
        cout << "Step " << i + 1 << ": " << nav_movementLog[i] << "\n";
    }
}

void returnUsingReversePath() {
    if (nav_robotPath.isEmpty()) {
        cout << "\nRobot is already at base or no path recorded.\n";
        return;
    }

    cout << "\n--- Initiating Autonomous Return Sequence ---\n";
    int step = 1;
    
    while (!nav_robotPath.isEmpty()) {
        string originalDir = nav_robotPath.pop();
        string reverseDir = getReverseDirection(originalDir);
        cout << "Return Step " << step++ << ": " << reverseDir 
             << " \t(Reversing: " << originalDir << ")\n";
    }
    
    cout << "\nSuccess: Robot safely retraced its memory and returned to the station!\n";
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
    if (nav_isReturned) cout << "  Status: Task finished. Return sequence completed.\n";
    else cout << "  Status: Robot is currently navigating (Pending return).\n";
}

void robotNavigationMenu() {
    int choice;
    do {
        cout << "\n--- Robot Navigation Module ---\n";
        cout << "1. Simulate Route Execution & Sensor Triggers\n";
        cout << "2. Display Current Path Memory\n";
        cout << "3. Execute Auto-Return to Base (Reverse Full Path)\n";
        cout << "4. Display Complete Navigation Log\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;
        if (cin.fail()) {
            if (cin.eof()) { choice = 0; break; } 
            cin.clear();
            cin.ignore(1000, '\n');
            choice = -1;
        } else {
            cin.ignore();
        }

        switch (choice) {
            case 1: simulateRouteExecution(); break;
            case 2: displayForwardPath(); break;
            case 3: returnUsingReversePath(); break;
            case 4: displayNavigationLog(); break;
            case 0: cout << "Returning to main menu...\n"; break;
            default: cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);
}
