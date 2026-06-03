#include <iostream>
#include <string>

// Import all module files here
#include "item_management.cpp"
#include "warehouse_navigation.cpp"
#include "order_management.cpp"
#include "robot_assignment.cpp"
#include "robot_navigation.cpp"

using namespace std;

// Declare menu functions 
void robotAssignmentMenu();
void warehouseNavigationMenu();
void robotNavigationMenu(); 
void orderManagementMenu(); 
void itemManagementMenu();

int main() {
    cout << "\n[System] Loading warehouse layout, item database, robot states, and order logs...\n";
    globalWarehouse.loadDefaultLayout();
    globalItemTree.loadItemsFromCSV();
    loadRobotsFromCSV();
    loadOrdersFromCSV();
    
    int choice;

    do {
        cout << "\n--- Warehouse Robot Navigation System ---\n";
        cout << "1. Order Management (Pending Orders: " << ((front == -1 || front > rear) ? 0 : (rear - front + 1)) << ")\n";
        cout << "2. Robot Assignment\n";
        cout << "3. Robot Navigation\n";
        cout << "4. Item Management\n";
        cout << "5. Warehouse Navigation\n";
        cout << "6. Full System Run (Integration Demo)\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        if (cin.fail()) {
            if (cin.eof()) { choice = 0; break; } // EOF: exit program
            cin.clear();
            cin.ignore(1000, '\n');
            choice = -1;
        }

        switch (choice) {
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
            case 6: { 
                cout << "\n=== INITIATING FULL SYSTEM AUTOMATION ===\n"; 
                
                // 1. Retrieve order from queue
                if (front == -1 || front > rear) {
                    cout << "[System] No pending orders in queue.\n";
                    cout << "Would you like to create a new order now? (y/n): ";
                    char createOrderChoice;
                    cin >> createOrderChoice;
                    cin.ignore();
                    if (createOrderChoice == 'y' || createOrderChoice == 'Y') {
                        Order newOrder;
                        cout << "Enter Order ID: ";
                        cin >> newOrder.orderID;
                        if (cin.fail()) {
                            cin.clear();
                            cin.ignore(1000, '\n');
                            cout << "[System] Invalid Order ID. Aborting automation.\n";
                            break;
                        }
                        if (isOrderIDExists(newOrder.orderID)) {
                            cout << "[System] Error: Order ID already exists. Aborting automation.\n";
                            break;
                        }
                        cin.ignore();
                        cout << "Enter Customer Name: ";
                        getline(cin, newOrder.customerName);
                        cout << "Enter Item ID: ";
                        getline(cin, newOrder.itemID);

                        // Validate that the itemID exists!
                        ItemNode* itemNode = globalItemTree.searchByID(newOrder.itemID);
                        if (itemNode == nullptr) {
                            cout << "[System] Error: Item ID \"" << newOrder.itemID 
                                 << "\" does not exist. Aborting automation.\n";
                            break;
                        }
                        cout << "Enter Quantity: ";
                        cin >> newOrder.quantity;
                        if (cin.fail() || newOrder.quantity <= 0) {
                            cin.clear();
                            cin.ignore(1000, '\n');
                            cout << "[System] Invalid Quantity. Aborting automation.\n";
                            break;
                        }
                        cin.ignore();
                        newOrder.status = "Pending";
                        
                        if (front == -1) front = 0;
                        if (rear >= 99) {
                            cout << "[System] Order queue is full. Cannot add more orders.\n";
                            break;
                        }
                        rear++;
                        orderQueue[rear] = newOrder;
                        saveAllOrders();
                        cout << "[System] New order recorded and added to queue.\n";
                    } else {
                        cout << "[System] Aborting automation.\n";
                        break;
                    }
                }
                
                Order currentOrder = orderQueue[front];
                cout << "[System] Processing Order: " << currentOrder.orderID 
                     << " (Item ID: " << currentOrder.itemID;
                ItemNode* itemNode = globalItemTree.searchByID(currentOrder.itemID);
                if (itemNode != nullptr) {
                    cout << " | Name: " << itemNode->itemName;
                }
                cout << " | Qty: " << currentOrder.quantity << ")\n";
                     
                // 2. Search for the item in the global tree by ID (since order has itemID)
                if (itemNode == nullptr) {
                    cout << "[System] Error: Item ID \"" << currentOrder.itemID 
                         << "\" not found in item database!\n";
                    cout << "[System] Removing invalid order from queue.\n";
                    
                    // Dequeue and fail
                    Order failedOrder = orderQueue[front];
                    failedOrder.status = "Failed: Item Not Found";
                    if (completedCount < 100) {
                        completedOrders[completedCount++] = failedOrder;
                    }
                    front++;
                    if (front > rear) { front = -1; rear = -1; }
                    saveAllOrders();
                    break;
                }
                
                cout << "[System] Item \"" << itemNode->itemName 
                     << "\" found at location: " << itemNode->location 
                     << " | Stock Count: " << itemNode->quantity << "\n";
                
                // --- STOCK VALIDATION ---
                if (itemNode->quantity < currentOrder.quantity) {
                    cout << "[System] Error: Insufficient Stock! Requested: " << currentOrder.quantity 
                         << " | Available: " << itemNode->quantity << "\n";
                    cout << "[System] Marking order as failed and removing from queue.\n";
                    
                    // Dequeue and fail
                    Order failedOrder = orderQueue[front];
                    failedOrder.status = "Failed: Insufficient Stock";
                    if (completedCount < 100) {
                        completedOrders[completedCount++] = failedOrder;
                    }
                    front++;
                    if (front > rear) { front = -1; rear = -1; }
                    saveAllOrders();
                    break;
                }
                
                // 3. Assign an available robot
                string robotID = assignRobotProgrammatically(to_string(currentOrder.orderID));
                if (robotID.empty()) {
                    cout << "[System] Error: No available robots to handle this task!\n";
                    cout << "[System] Please go to Robot Assignment Menu to add/complete tasks.\n";
                    break;
                }
                
                cout << "[System] Assigned Robot: " << robotID << " to Task " << currentOrder.orderID << "\n";
                
                // Dequeue the order now that it has been successfully assigned
                front++;
                if (front > rear) { front = -1; rear = -1; }
                
                // 4. Generate navigation route
                string targetLocation = itemNode->location;
                string locationPath[200];
                int pathLength = globalWarehouse.getPathToShelf(targetLocation, locationPath);
                
                if (pathLength == 0) {
                    cout << "[System] Error: Location \"" << targetLocation 
                         << "\" is unreachable or not defined in layout tree!\n";
                    
                    // Restore robot availability since task failed
                    int robotIndex = findRobotIndex(robotID);
                    if (robotIndex != -1) {
                        robots[robotIndex].status = "available";
                        robots[robotIndex].currentTaskID = "-";
                        updateAssignmentStatus(to_string(currentOrder.orderID), robotID, "Failed: Path Not Found");
                        saveRobotsToCSV();
                    }
                    
                    // Restore order to history as Failed
                    currentOrder.status = "Failed: Path Not Found";
                    if (completedCount < 100) {
                        completedOrders[completedCount++] = currentOrder;
                    }
                    saveAllOrders();
                    break;
                }
                
                // 5. Print the route calculated
                cout << "[System] Mainframe calculated route from layout:\n";
                for (int i = 0; i < pathLength; i++) {
                    cout << "Step " << i + 1 << ": " << locationPath[i] << "\n";
                }
                
                // 6. Get directions using grid coordinates
                string directions[200];
                int dirLength = globalWarehouse.getDirections(locationPath, pathLength, directions);
        
                // 7. Dynamic Obstacle Simulation 
                cout << "\n[System] Do you want to simulate a dynamic/unmapped obstacle for this run? (y/n): ";
                char simObs;
                cin >> simObs;
                cin.ignore();

                if (simObs == 'y' || simObs == 'Y') {
                    cout << "\n[Robot Navigation] Executing route. Monitoring proximity sensors...\n";
                    
                    int crashPoint = dirLength / 2; // Obstacle appears halfway through the route
                    
                    for (int i = 0; i < dirLength; i++) {
                        if (i == crashPoint) {
                            //triggerObstacle and return 
                            triggerObstacle();
                            
                            cout << "\n[System] FATAL: Path blocked by unmapped object. Mission aborted.\n";
                            cout << "[System] Executing emergency return protocol...\n";
                            
                            // Use the Stack to automatically drive back home
                            returnUsingReversePath();
                            
                            // Mark order as failed so the system knows it wasn't completed
                            currentOrder.status = "Failed: Dynamic Obstacle Blocked Path";
                            if (completedCount < 100) {
                                completedOrders[completedCount++] = currentOrder;
                            }
                            
                            // Mark robot as available again
                            int robotIndex = findRobotIndex(robotID);
                            if (robotIndex != -1) {
                                robots[robotIndex].status = "available";
                                robots[robotIndex].currentTaskID = "-";
                                updateAssignmentStatus(to_string(currentOrder.orderID), robotID, "Failed: Obstacle");
                                saveRobotsToCSV();
                            }
                            break; // End the simulation loop
                        }
                        
                        executeSingleStep(directions[i]); // Move forward normally
                    }
                }
                else {
                    // 7. Load directions and execute forward path
                    loadDirectionsDirectly(directions, dirLength);
                    displayForwardPath();
                    
                    // 8. Simulate reverse journey to return robot to base
                    cout << "\n[System] Item retrieved. Executing return protocol...\n";
                    returnUsingReversePath();
                    
                    // 9. Mark the task as completed for this robot
                    int robotIndex = findRobotIndex(robotID);
                    if (robotIndex != -1) {
                        robots[robotIndex].status = "available";
                        robots[robotIndex].currentTaskID = "-";
                        saveRobotsToCSV();
                        
                        updateAssignmentStatus(to_string(currentOrder.orderID), robotID, "Completed");
                    }
                    
                    // deduct stock 
                    itemNode->quantity -= currentOrder.quantity;
                    globalItemTree.saveAllItemsToCSV();
                    cout << "[System] Stock Deducted: " << currentOrder.quantity 
                        << " unit(s) of \"" << itemNode->itemName << "\".\n";
                    cout << "[System] Remaining Stock Count for ID " << itemNode->itemID 
                        << ": " << itemNode->quantity << "\n";
                    
                    // 10. Record order as completed and save
                    currentOrder.status = "Completed";
                    if (completedCount < 100) {
                        completedOrders[completedCount++] = currentOrder;
                    }
                    saveAllOrders();
                    
                    cout << "\n=== AUTOMATION COMPLETE ===\n";
                    break;
                }
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
