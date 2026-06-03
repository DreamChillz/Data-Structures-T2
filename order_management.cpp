#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

// Queue structure for storing incoming customer orders
struct Order
{
    int orderID;
    string customerName;
    string itemID; // item ID instead of name
    int quantity; // quantity ordered
    string status; // "Pending", "Completed", "Failed: Insufficient Stock", etc.
};

Order orderQueue[100];
int front = -1;
int rear = -1;

Order completedOrders[100];
int completedCount = 0;

// Unique check helper
bool isOrderIDExists(int id)
{
    if (front != -1 && front <= rear)
    {
        for (int i = front; i <= rear; i++)
        {
            if (orderQueue[i].orderID == id) return true;
        }
    }
    for (int i = 0; i < completedCount; i++)
    {
        if (completedOrders[i].orderID == id) return true;
    }
    return false;
}

// Save all orders to CSV
void saveAllOrders()
{
    ofstream file("orders.csv");
    if (!file.is_open())
    {
        cout << "[Order Management] Warning: Could not open orders.csv for writing.\n";
        return;
    }
    file << "OrderID,CustomerName,ItemID,Quantity,Status\n";
    if (front != -1 && front <= rear)
    {
        for (int i = front; i <= rear; i++)
        {
            file << orderQueue[i].orderID << ","
                 << orderQueue[i].customerName << ","
                 << orderQueue[i].itemID << ","
                 << orderQueue[i].quantity << ","
                 << orderQueue[i].status << "\n";
        }
    }
    for (int i = 0; i < completedCount; i++)
    {
        file << completedOrders[i].orderID << ","
             << completedOrders[i].customerName << ","
             << completedOrders[i].itemID << ","
             << completedOrders[i].quantity << ","
             << completedOrders[i].status << "\n";
    }
    file.close();
}

// Load all orders from CSV
void loadOrdersFromCSV()
{
    front = -1;
    rear = -1;
    completedCount = 0;

    ifstream file("orders.csv");
    if (!file.is_open()) {
        ofstream newFile("orders.csv");
        newFile << "OrderID,CustomerName,ItemID,Quantity,Status\n";
        newFile.close();
        return;
    }

    string line;
    if (!getline(file, line))
    {
        file.close();
        return;
    }

    while (getline(file, line))
    {
        if (line.empty()) continue;
        stringstream ss(line);
        string idStr, customer, item, qtyStr, status;
        
        getline(ss, idStr, ',');
        getline(ss, customer, ',');
        getline(ss, item, ',');
        getline(ss, qtyStr, ',');
        getline(ss, status, ',');

        if (!idStr.empty())
        {
            int id = 0;
            int qty = 1;
            try {
                id = stoi(idStr);
                if (!qtyStr.empty()) qty = stoi(qtyStr);
            } catch (...) {
                id = 0;
                qty = 1;
            }

            Order orderObj = {id, customer, item, qty, status};
            if (status == "Pending" || status == "Assigned")
            {
                if (front == -1) front = 0;
                rear++;
                orderQueue[rear] = orderObj;
            }
            else
            {
                if (completedCount < 100)
                {
                    completedOrders[completedCount++] = orderObj;
                }
            }
        }
    }
    file.close();
}

// Add a new order to the rear of the queue
void addOrder()
{
    if (rear == 99)
    {
        cout << "Order queue is full.\n";
        return;
    }

    Order newOrder;

    cout << "Enter Order ID: ";
    cin >> newOrder.orderID;

    if (cin.fail())
    {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid Order ID.\n";
        return;
    }

    if (isOrderIDExists(newOrder.orderID))
    {
        cout << "Error: Order ID already exists. Please enter a unique ID.\n";
        return;
    }

    cin.ignore();

    cout << "Enter Customer Name: ";
    getline(cin, newOrder.customerName);

    cout << "Enter Item ID: ";
    getline(cin, newOrder.itemID);

    // Validate that the itemID exists!
    ItemNode* itemNode = globalItemTree.searchByID(newOrder.itemID);
    if (itemNode == nullptr)
    {
        cout << "Error: Item ID \"" << newOrder.itemID << "\" not found in item database. Order aborted.\n";
        return;
    }

    cout << "Enter Order Quantity: ";
    cin >> newOrder.quantity;
    if (cin.fail() || newOrder.quantity <= 0)
    {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid Quantity. Order aborted.\n";
        return;
    }
    cin.ignore();

    newOrder.status = "Pending";

    if (front == -1)
        front = 0;

    // Check if queue is full (rear has reached end of array)
    if (rear >= 99) {
        cout << "Order queue is full (max 100 orders). Cannot add more orders.\n";
        return;
    }

    rear++;
    orderQueue[rear] = newOrder;
    saveAllOrders();

    cout << "Order added successfully.\n";
}

// Process Order
void processOrder()
{
    if (front == -1 || front > rear)
    {
        cout << "No pending orders.\n";
        return;
    }

    Order current = orderQueue[front];

    cout << "\nProcessing Order:\n";
    cout << "Order ID: " << current.orderID << endl;
    cout << "Customer: " << current.customerName << endl;
    cout << "Item ID: " << current.itemID;
    ItemNode* itemNode = globalItemTree.searchByID(current.itemID);
    if (itemNode != nullptr) {
        cout << " (" << itemNode->itemName << ")";
    }
    cout << endl;
    cout << "Quantity: " << current.quantity << endl;

    current.status = "Completed";
    completedOrders[completedCount++] = current;

    front++;

    // Reset queue if it becomes empty
    if (front > rear)
    {
        front = -1;
        rear = -1;
    }
    saveAllOrders();

    cout << "Order completed.\n";
}

// Current Order
void viewCurrentOrder()
{
    if (front == -1 || front > rear)
    {
        cout << "No current order.\n";
        return;
    }

    cout << "\nCurrent Order:\n";
    cout << "Order ID: " << orderQueue[front].orderID << endl;
    cout << "Customer: " << orderQueue[front].customerName << endl;
    cout << "Item ID: " << orderQueue[front].itemID;
    ItemNode* itemNode = globalItemTree.searchByID(orderQueue[front].itemID);
    if (itemNode != nullptr) {
        cout << " (" << itemNode->itemName << ")";
    }
    cout << endl;
    cout << "Quantity: " << orderQueue[front].quantity << endl;
}

// Pending Orders
void viewPendingOrders()
{
    if (front == -1 || front > rear)
    {
        cout << "No pending orders.\n";
        return;
    }

    cout << "\n--- Pending Orders ---\n";

    for (int i = front; i <= rear; i++)
    {
        cout << "Order ID: " << orderQueue[i].orderID
             << " | Customer: " << orderQueue[i].customerName
             << " | Item ID: " << orderQueue[i].itemID;
        ItemNode* itemNode = globalItemTree.searchByID(orderQueue[i].itemID);
        if (itemNode != nullptr) {
            cout << " (" << itemNode->itemName << ")";
        }
        cout << " | Quantity: " << orderQueue[i].quantity
             << endl;
    }
}

// Completed Orders
void viewCompletedOrders()
{
    if (completedCount == 0)
    {
        cout << "No completed orders.\n";
        return;
    }

    cout << "\n--- Completed/Failed Orders History ---\n";

    for (int i = 0; i < completedCount; i++)
    {
        cout << "Order ID: " << completedOrders[i].orderID
             << " | Customer: " << completedOrders[i].customerName
             << " | Item ID: " << completedOrders[i].itemID;
        ItemNode* itemNode = globalItemTree.searchByID(completedOrders[i].itemID);
        if (itemNode != nullptr) {
            cout << " (" << itemNode->itemName << ")";
        }
        cout << " | Quantity: " << completedOrders[i].quantity
             << " | Status: " << completedOrders[i].status
             << endl;
    }
}

// Menu
void orderManagementMenu()
{
    loadOrdersFromCSV();
    int choice;

    do
    {
        cout << "\n--- Order Management Menu ---\n";
        cout << "1. Add Order\n";
        cout << "2. Process Order\n";
        cout << "3. View Current Order\n";
        cout << "4. View Pending Orders\n";
        cout << "5. View Completed Orders\n";
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

        switch (choice)
        {
        case 1:
            addOrder();
            break;
        case 2:
            processOrder();
            break;
        case 3:
            viewCurrentOrder();
            break;
        case 4:
            viewPendingOrders();
            break;
        case 5:
            viewCompletedOrders();
            break;
        case 0:
            cout << "Returning to main menu...\n";
            break;
        default:
            cout << "Invalid choice.\n";
        }

    } while (choice != 0);
}