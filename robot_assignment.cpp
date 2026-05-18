#pragma once

#include <iostream>
#include <string>
using namespace std;

const int MAX_ROBOTS = 5;

struct Robot {
    string robotID;
    string status; // "available", "busy", "maintenance"
    int taskCount;
};

Robot robots[MAX_ROBOTS];
int robotCount = 0;
int currentIndex = 0;

void addRobot() {
    if (robotCount >= MAX_ROBOTS) {
        cout << "Robot list is full.\n";
        return;
    }

    cout << "Enter Robot ID: ";
    cin >> robots[robotCount].robotID;

    robots[robotCount].status = "available";
    robots[robotCount].taskCount = 0;

    robotCount++;

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
             << " | Tasks: " << robots[i].taskCount << endl;
    }
}

void updateRobotStatus() {
    string id;
    cout << "Enter Robot ID: ";
    cin >> id;

    for (int i = 0; i < robotCount; i++) {
        if (robots[i].robotID == id) {
            cout << "Enter new status (available/busy/maintenance): ";
            cin >> robots[i].status;

            cout << "Status updated.\n";
            return;
        }
    }

    cout << "Robot not found.\n";
}

void assignRobot() {
    if (robotCount == 0) {
        cout << "No robots available.\n";
        return;
    }

    int checked = 0;

    while (checked < robotCount) {
        if (robots[currentIndex].status == "available") {
            cout << "Task assigned to Robot: "
                 << robots[currentIndex].robotID << endl;

            robots[currentIndex].taskCount++;

            currentIndex = (currentIndex + 1) % robotCount;
            return;
        }

        currentIndex = (currentIndex + 1) % robotCount;
        checked++;
    }

    cout << "No available robot. All robots are busy or under maintenance.\n";
}

void robotAssignmentMenu() {
    int choice;

    do {
        cout << "\n--- Robot Assignment Menu ---\n";
        cout << "1. Add Robot\n";
        cout << "2. Assign Task to Robot\n";
        cout << "3. Update Robot Status\n";
        cout << "4. Display Robot Status\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;

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
            case 0:
                cout << "Returning to main menu...\n";
                break;
            default:
                cout << "Invalid choice.\n";
        }

    } while (choice != 0);
}