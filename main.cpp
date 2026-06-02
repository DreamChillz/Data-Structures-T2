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
void orderManagementMenu();
void itemManagementMenu();

int main()
{
    int choice;

    do
    {
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

        switch (choice)
        {
        case 1:
            orderManagementMenu();
            break;
        case 2:
            robotAssignmentMenu();
            break;
        case 3:
            robotNavigationMenu();
            break;
        case 4:
            itemManagementMenu();
            break;
        case 5:
            warehouseNavigationMenu();
            break;
        case 6:
        {
            // needs to add other funciton to complete the system automation
            cout << "\n=== INITIATING FULL SYSTEM AUTOMATION ===\n";

            // Task 5 - load warehouse layout then generate route
            WarehouseTree warehouse;
            warehouse.loadDefaultLayout();

            string targetShelf = "Shelf A1-1";
            string pathArray[200];
            int pathLength = warehouse.getPathToShelf(targetShelf, pathArray);

            if (pathLength == 0)
            {
                cout << "Route not found. Automation aborted.\n";
                break;
            }
            cout << "Route generated (" << pathLength << " steps)\n";

            // Task 3 — execute movement and return
            translateAndLoadPath(pathArray, pathLength);
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
