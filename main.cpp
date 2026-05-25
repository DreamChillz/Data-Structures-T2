#include <iostream>

// import files here
#include "robot_assignment.cpp"
#include "item_management.cpp"
#include "warehouse_navigation.cpp"

using namespace std;

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
        cout << "6. Full System Run (all modules together)\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
        case 2:
            robotAssignmentMenu();
            break;
        case 4:
            itemManagementMenu();
            break;
        case 5:
            warehouseNavigationMenu();
            break;
        case 0:
            cout << "Exiting system...\n";
            break;
        default:
            cout << "Module not connected yet.\n";
        }

    } while (choice != 0);

    return 0;
}