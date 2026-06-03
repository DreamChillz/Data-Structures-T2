#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

const int MAX_ROBOTS = 5;
const int MAX_ASSIGNMENTS = 50;

struct Robot {
    string robotID;
    string status; // "available", "busy", "maintenance"
    string currentTaskID;
    int taskCount;
};

struct AssignmentRecord {
    string taskID;
    string robotID;
    string status;
};

Robot robots[MAX_ROBOTS];
AssignmentRecord assignmentHistory[MAX_ASSIGNMENTS];
int robotCount = 0;
int currentIndex = 0;
int assignmentCount = 0;

bool isValidStatus(string status) {
    return status == "available" || status == "busy" || status == "maintenance";
}

int findRobotIndex(string id) {
    for (int i = 0; i < robotCount; i++) {
        if (robots[i].robotID == id) {
            return i;
        }
    }
    return -1;
}

void saveRobotsToCSV() {
    ofstream file("robots.csv");
    if (!file.is_open()) {
        cout << "[Robot Assignment] Warning: Could not open robots.csv for writing.\n";
        return;
    }
    file << "RobotID,Status,CurrentTaskID,TaskCount\n";
    for (int i = 0; i < robotCount; i++) {
        file << robots[i].robotID << ","
             << robots[i].status << ","
             << robots[i].currentTaskID << ","
             << robots[i].taskCount << "\n";
    }
    file.close();
}

void loadDefaultRobots() {
    // Clear first to prevent duplication
    robotCount = 0;
    robots[0] = {"ROBOT_01", "available", "-", 0};
    robots[1] = {"ROBOT_02", "available", "-", 0};
    robots[2] = {"ROBOT_03", "available", "-", 0};
    robotCount = 3;
    currentIndex = 0;
    cout << "Default robots (ROBOT_01, ROBOT_02, ROBOT_03) loaded successfully.\n";
}

void loadRobotsFromCSV() {
    robotCount = 0;
    currentIndex = 0;

    ifstream file("robots.csv");
    if (!file.is_open()) {
        // If file doesn't exist, load defaults and write to CSV
        loadDefaultRobots();
        saveRobotsToCSV();
        return;
    }

    string line;
    if (!getline(file, line)) {
        file.close();
        return;
    }

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id, status, taskID, taskCountStr;
        
        getline(ss, id, ',');
        getline(ss, status, ',');
        getline(ss, taskID, ',');
        getline(ss, taskCountStr, ',');

        if (!id.empty() && robotCount < MAX_ROBOTS) {
            int tCount = 0;
            try {
                if (!taskCountStr.empty()) {
                    tCount = stoi(taskCountStr);
                }
            } catch (...) {
                tCount = 0;
            }

            robots[robotCount].robotID = id;
            robots[robotCount].status = status;
            robots[robotCount].currentTaskID = taskID;
            robots[robotCount].taskCount = tCount;
            robotCount++;
        }
    }
    file.close();
}

void addRobot() {
    if (robotCount >= MAX_ROBOTS) {
        cout << "Robot list is full.\n";
        return;
    }

    cout << "Enter Robot ID: ";
    cin >> robots[robotCount].robotID;

    if (findRobotIndex(robots[robotCount].robotID) != -1) {
        cout << "Robot ID already exists. Please use a unique ID.\n";
        return;
    }

    robots[robotCount].status = "available";
    robots[robotCount].currentTaskID = "-";
    robots[robotCount].taskCount = 0;

    robotCount++;
    saveRobotsToCSV();

    cout << "Robot added successfully.\n";
}

void displayRobots() {
    cout << "\n--- Robot Status ---\n";

    if (robotCount == 0) {
        cout << "No robots available.\n";
        return;
    }

    for (int i = 0; i < robotCount; i++) {
        cout << i + 1 << ". "
             << robots[i].robotID
             << " | Status: " << robots[i].status
             << " | Current Task: " << robots[i].currentTaskID
             << " | Tasks: " << robots[i].taskCount << endl;
    }
}

void displayAssignmentHistory() {
    cout << "\n--- Robot Assignment History ---\n";

    if (assignmentCount == 0) {
        cout << "No task assignments recorded.\n";
        return;
    }

    for (int i = 0; i < assignmentCount; i++) {
        cout << i + 1 << ". Task " << assignmentHistory[i].taskID
             << " -> Robot " << assignmentHistory[i].robotID
             << " | " << assignmentHistory[i].status << endl;
    }
}

void updateRobotStatus() {
    string id;
    cout << "Enter Robot ID: ";
    cin >> id;

    int robotIndex = findRobotIndex(id);

    if (robotIndex == -1) {
        cout << "Robot not found.\n";
        return;
    }

    string newStatus;
    cout << "Enter new status (available/busy/maintenance): ";
    cin >> newStatus;

    if (!isValidStatus(newStatus)) {
        cout << "Invalid status. Status must be available, busy, or maintenance.\n";
        return;
    }

    robots[robotIndex].status = newStatus;

    if (newStatus == "available" || newStatus == "maintenance") {
        robots[robotIndex].currentTaskID = "-";
    }

    saveRobotsToCSV();
    cout << "Status updated.\n";
}

void completeRobotTask() {
    string id;
    cout << "Enter Robot ID that completed its task: ";
    cin >> id;

    int robotIndex = findRobotIndex(id);

    if (robotIndex == -1) {
        cout << "Robot not found.\n";
        return;
    }

    if (robots[robotIndex].status != "busy") {
        cout << "Robot is not currently busy.\n";
        return;
    }

    for (int i = assignmentCount - 1; i >= 0; i--) {
        if (assignmentHistory[i].robotID == robots[robotIndex].robotID &&
            assignmentHistory[i].taskID == robots[robotIndex].currentTaskID) {
            assignmentHistory[i].status = "Completed";
            break;
        }
    }

    robots[robotIndex].status = "available";
    robots[robotIndex].currentTaskID = "-";

    saveRobotsToCSV();
    cout << "Task completed. Robot is now available.\n";
}

void assignRobot() {
    if (robotCount == 0) {
        cout << "No robots available.\n";
        return;
    }

    if (assignmentCount >= MAX_ASSIGNMENTS) {
        cout << "Assignment history is full. Cannot record more assignments.\n";
        return;
    }

    string taskID;
    cout << "Enter Task/Order ID: ";
    cin >> taskID;

    int checked = 0;

    while (checked < robotCount) {
        if (robots[currentIndex].status == "available") {
            int assignedIndex = currentIndex;

            cout << "Task assigned to Robot: "
                 << robots[assignedIndex].robotID << endl;

            robots[assignedIndex].status = "busy";
            robots[assignedIndex].currentTaskID = taskID;
            robots[assignedIndex].taskCount++;

            assignmentHistory[assignmentCount].taskID = taskID;
            assignmentHistory[assignmentCount].robotID = robots[assignedIndex].robotID;
            assignmentHistory[assignmentCount].status = "Assigned";
            assignmentCount++;

            currentIndex = (currentIndex + 1) % robotCount;
            saveRobotsToCSV();
            return;
        }

        currentIndex = (currentIndex + 1) % robotCount;
        checked++;
    }

    cout << "No available robot. All robots are busy or under maintenance.\n";
}

string assignRobotProgrammatically(string taskID) {
    if (robotCount == 0) {
        return "";
    }

    if (assignmentCount >= MAX_ASSIGNMENTS) {
        return "";
    }

    int checked = 0;

    while (checked < robotCount) {
        if (robots[currentIndex].status == "available") {
            int assignedIndex = currentIndex;

            robots[assignedIndex].status = "busy";
            robots[assignedIndex].currentTaskID = taskID;
            robots[assignedIndex].taskCount++;

            assignmentHistory[assignmentCount].taskID = taskID;
            assignmentHistory[assignmentCount].robotID = robots[assignedIndex].robotID;
            assignmentHistory[assignmentCount].status = "Assigned";
            assignmentCount++;

            string assignedID = robots[assignedIndex].robotID;
            currentIndex = (currentIndex + 1) % robotCount;
            saveRobotsToCSV();
            return assignedID;
        }

        currentIndex = (currentIndex + 1) % robotCount;
        checked++;
    }

    return "";
}

void updateAssignmentStatus(string taskID, string robotID, string newStatus) {
    for (int i = assignmentCount - 1; i >= 0; i--) {
        if (assignmentHistory[i].taskID == taskID &&
            assignmentHistory[i].robotID == robotID) {
            assignmentHistory[i].status = newStatus;
            return;
        }
    }
}

void robotAssignmentMenu() {
    loadRobotsFromCSV();
    int choice;

    do {
        cout << "\n--- Robot Assignment Menu ---\n";
        cout << "1. Add Robot\n";
        cout << "2. Assign Task to Robot\n";
        cout << "3. Update Robot Status\n";
        cout << "4. Display Robot Status\n";
        cout << "5. Display Assignment History\n";
        cout << "6. Complete Robot Task\n";
        cout << "7. Load Default Robots (Demo)\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;
        if (cin.fail()) {
            if (cin.eof()) { choice = 0; break; } // EOF: exit menu
            cin.clear();
            cin.ignore(1000, '\n');
            choice = -1;
        } else {
            cin.ignore(); // consume trailing newline
        }

        switch (choice) {
            case 1:
                addRobot();
                break;
            case 2:
                assignRobot();
                break;
            case 3:
                updateRobotStatus();
                break;
            case 4:
                displayRobots();
                break;
            case 5:
                displayAssignmentHistory();
                break;
            case 6:
                completeRobotTask();
                break;
            case 7:
                loadDefaultRobots();
                saveRobotsToCSV();
                break;
            case 0:
                cout << "Returning to main menu...\n";
                break;
            default:
                cout << "Invalid choice.\n";
        }

    } while (choice != 0);
}
