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
            cout << "\n=== INITIATING FULL SYSTEM AUTOMATION ===\n";

            // Step 1: Build warehouse layout with 2D coordinates (Task 5)
            WarehouseTree warehouse;
            warehouse.loadDefaultLayout();

            // Step 2: Preload obstacles onto the warehouse grid
            warehouse.setGridObstacle(3, 1, true); // corridor between Zone A and Zone B
            warehouse.setGridObstacle(6, 1, true); // corridor between Zone B and Zone C
            warehouse.setGridObstacle(4, 2, true); // corridor between Aisle A2 and Aisle B1

            // Step 3: Show the 2D grid with obstacles
            warehouse.displayGrid();

            // Step 4: Generate the real route
            string targetShelf = "Shelf A1-1";
            string pathArray[200];
            int pathLength = warehouse.getPathToShelf(targetShelf, pathArray);

            if (pathLength == 0)
            {
                cout << "Route not found. Automation aborted.\n";
                break;
            }
            cout << "\nRoute generated (" << pathLength << " steps)\n";
            
            
            //robot navigation here

            // Step 6: Task 3 — translate route into physical movements
            translateAndLoadPath(pathArray, pathLength);
            displayForwardPath();

            // Step 7: Return to base
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
