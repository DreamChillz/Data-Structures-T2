#pragma once
#include <iostream>
#include <string>

using namespace std;

// Queue structure for storing incoming customer orders
struct Order
{
    int orderID;
    string customerName;
    string itemName;
};

Order orderQueue[100];
int front = -1;
int rear = -1;

Order completedOrders[100];
int completedCount = 0;


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

    cin.ignore();

    cout << "Enter Customer Name: ";
    getline(cin, newOrder.customerName);

    cout << "Enter Item Name: ";
    getline(cin, newOrder.itemName);

    if (front == -1)
        front = 0;

    rear++;
    orderQueue[rear] = newOrder;

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
    cout << "Item: " << current.itemName << endl;

    completedOrders[completedCount++] = current;

    front++;

    // Reset queue if it becomes empty
    if (front > rear)
    {
        front = -1;
        rear = -1;
    }

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
    cout << "Item: " << orderQueue[front].itemName << endl;
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
             << " | Item: " << orderQueue[i].itemName
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

    cout << "\n--- Completed Orders ---\n";

    for (int i = 0; i < completedCount; i++)
    {
        cout << "Order ID: " << completedOrders[i].orderID
             << " | Customer: " << completedOrders[i].customerName
             << " | Item: " << completedOrders[i].itemName
             << endl;
    }
}

// Menu
void orderManagementMenu()
{
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