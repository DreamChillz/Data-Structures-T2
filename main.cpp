#include <iostream>
#include <string>

// Import all module files here
#include "order_management.cpp"
#include "robot_assignment.cpp"
#include "robot_navigation.cpp" 
#include "item_management.cpp"
#include "warehouse_navigation.cpp"

using namespace std;

// Declare menu functions 
void robotAssignmentMenu();
void warehouseNavigationMenu();
void robotNavigationMenu(); 
// void orderManagementMenu(); // Uncomment when your teammate finishes Task 1
// void itemManagementMenu();  // Uncomment when your teammate finishes Task 4

int main() {
    int choice;

    do {
        cout << "\n--- Warehouse Robot Navigation System ---\n";
        cout << "1. Order Management\n";
        cout << "2. Robot Assignment\n";
        cout << "3. Robot Navigation\n";
        cout << "4. Item Management\n";
        cout << "5. Warehouse Navigation\n";
        cout << "6. Full System Run (Integration Demo)\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                // add orderManagementMenu();
                break;
            case 2:
                robotAssignmentMenu();
                break;
            case 3: 
                robotNavigationMenu();
                break;
            case 4:
                // add itemManagementMenu();
                break;
            case 5:
                warehouseNavigationMenu();
                break;
            case 6: { 
                // needs to add other funciton to complete the system automation 
                cout << "\n=== INITIATING FULL SYSTEM AUTOMATION ===\n"; 
                
                //change to accept the location list from warehouse
                cout << "[System] Mainframe calculated route to Shelf A1-1...\n";
                string locationsFromTask5[] = {"Main Warehouse", "Zone A", "Aisle A1", "Shelf A1-1"}; 
                int pathLength = 4;

                // send the location to robot_navigation
                // put the location list from warehouse into the funciton below
                translateAndLoadPath(locationsFromTask5, pathLength);
                displayForwardPath();

                // reverse to home 
                cout << "\n[System] Item retrieved. Executing return protocol...\n";
                returnUsingReversePath();
                
                cout << "\n=== AUTOMATION COMPLETE ===\n";
                break;
            }
            case 0:
                cout << "Exiting system...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 0);

    return 0;
}
