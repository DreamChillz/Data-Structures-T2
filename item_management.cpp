#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

// Node representing an item in the Binary Search Tree
struct ItemNode
{
    string itemID;
    string itemName;
    string category;
    string location;
    int quantity; // Stock quantity in warehouse
    ItemNode *left;
    ItemNode *right;

    ItemNode(string id, string name, string cat, string loc, int qty)
    {
        itemID = id;
        itemName = name;
        category = cat;
        location = loc;
        quantity = qty;
        left = nullptr;
        right = nullptr;
    }
};

// Binary Search Tree for storing and managing items
struct ItemTree
{
    ItemNode *root;

    ItemTree()
    {
        root = nullptr;
    }

    void clearTree(ItemNode *node)
    {
        if (node == nullptr)
            return;
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }

    void clear()
    {
        clearTree(root);
        root = nullptr;
    }

    // Helper function to insert a node recursively
    ItemNode *insertNode(ItemNode *node, string id, string name, string cat, string loc, int qty, bool &success)
    {
        if (node == nullptr)
        {
            success = true;
            return new ItemNode(id, name, cat, loc, qty);
        }

        if (id < node->itemID)
        {
            node->left = insertNode(node->left, id, name, cat, loc, qty, success);
        }
        else if (id > node->itemID)
        {
            node->right = insertNode(node->right, id, name, cat, loc, qty, success);
        }
        else
        {
            success = false; // Duplicate ID found
        }
        return node;
    }

    // Recursive helper to save items to stream (in-order)
    void saveItemsToStream(ItemNode* node, ofstream& file)
    {
        if (node == nullptr) return;
        saveItemsToStream(node->left, file);
        file << node->itemID << ","
             << node->itemName << ","
             << node->category << ","
             << node->location << ","
             << node->quantity << "\n";
        saveItemsToStream(node->right, file);
    }

    void saveAllItemsToCSV()
    {
        ofstream file("items.csv");
        if (!file.is_open())
        {
            cout << "[Item Management] Warning: Could not open items.csv for writing.\n";
            return;
        }
        file << "ItemID,ItemName,Category,Location,Quantity\n";
        saveItemsToStream(root, file);
        file.close();
    }

    void loadItemsFromCSV()
    {
        clear();

        ifstream file("items.csv");
        if (!file.is_open())
        {
            // If file doesn't exist, load defaults and write to CSV
            loadDefaultItems();
            saveAllItemsToCSV();
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
            string id, name, category, location, qtyStr;
            
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, category, ',');
            getline(ss, location, ',');
            getline(ss, qtyStr, ',');

            if (!id.empty())
            {
                int qty = 0;
                try {
                    qty = stoi(qtyStr);
                } catch (...) {
                    qty = 0;
                }
                bool success = false;
                root = insertNode(root, id, name, category, location, qty, success);
            }
        }
        file.close();
    }

    void insertItem(string id, string name, string cat, string loc, int qty)
    {
        bool success = false;
        root = insertNode(root, id, name, cat, loc, qty, success);
        if (success)
        {
            cout << "Item \"" << name << "\" (Stock: " << qty << ") added successfully.\n";
            saveAllItemsToCSV();
        }
        else
        {
            cout << "Failed to add item. Item ID \"" << id << "\" already exists.\n";
        }
    }

    // Helper to search node by ID recursively
    ItemNode *searchNode(ItemNode *node, string id)
    {
        if (node == nullptr || node->itemID == id)
            return node;

        if (id < node->itemID)
            return searchNode(node->left, id);

        return searchNode(node->right, id);
    }

    ItemNode *searchByID(string id)
    {
        return searchNode(root, id);
    }

    // Helper to search node by name using in-order traversal
    ItemNode *searchByNameNode(ItemNode *node, string name)
    {
        if (node == nullptr)
            return nullptr;

        // Search left subtree
        ItemNode *found = searchByNameNode(node->left, name);
        if (found != nullptr)
            return found;

        // Check current node
        if (node->itemName == name)
            return node;

        // Search right subtree
        return searchByNameNode(node->right, name);
    }

    ItemNode *searchByName(string name)
    {
        return searchByNameNode(root, name);
    }

    // Helper to find minimum node in a subtree
    ItemNode *findMin(ItemNode *node)
    {
        while (node && node->left != nullptr)
            node = node->left;
        return node;
    }

    // Helper to delete node recursively
    ItemNode *deleteNode(ItemNode *node, string id, bool &success)
    {
        if (node == nullptr)
            return nullptr;

        if (id < node->itemID)
        {
            node->left = deleteNode(node->left, id, success);
        }
        else if (id > node->itemID)
        {
            node->right = deleteNode(node->right, id, success);
        }
        else
        {
            success = true;
            // Case 1 & 2: One child or no child
            if (node->left == nullptr)
            {
                ItemNode *temp = node->right;
                delete node;
                return temp;
            }
            else if (node->right == nullptr)
            {
                ItemNode *temp = node->left;
                delete node;
                return temp;
            }

            // Case 3: Two children
            ItemNode *temp = findMin(node->right);
            node->itemID = temp->itemID;
            node->itemName = temp->itemName;
            node->category = temp->category;
            node->location = temp->location;
            node->quantity = temp->quantity;
            node->right = deleteNode(node->right, temp->itemID, success);
        }
        return node;
    }

    void removeItem(string id)
    {
        bool success = false;
        root = deleteNode(root, id, success);
        if (success)
        {
            cout << "Item with ID \"" << id << "\" deleted successfully.\n";
            saveAllItemsToCSV();
        }
        else
        {
            cout << "Item with ID \"" << id << "\" not found.\n";
        }
    }

    // Helper for displaying nodes in sorted order (in-order traversal)
    void inOrderTraversal(ItemNode *node)
    {
        if (node == nullptr)
            return;

        inOrderTraversal(node->left);
        cout << "ID: " << node->itemID
             << " | Name: " << node->itemName
             << " | Category: " << node->category
             << " | Location: " << node->location
             << " | Stock Count: " << node->quantity << "\n";
        inOrderTraversal(node->right);
    }

    void displayAllItems()
    {
        if (root == nullptr)
        {
            cout << "Item list is empty.\n";
            return;
        }
        cout << "\n--- Item List (Sorted by ID) ---\n";
        inOrderTraversal(root);
    }

    void loadDefaultItems()
    {
        // Clear any existing items first so we don't duplicate
        clear();
        insertItem("105", "Wireless Mouse", "Electronics", "Shelf A1-1", 50);
        insertItem("102", "USB-C Hub", "Electronics", "Shelf A1-2", 30);
        insertItem("108", "Desk Lamp", "Furniture", "Shelf B1-1", 20);
        insertItem("101", "Mechanical Keyboard", "Electronics", "Shelf A1-3", 15);
        insertItem("103", "Monitor Stand", "Furniture", "Shelf A2-1", 25);
        insertItem("107", "Webcam HD", "Electronics", "Shelf B1-2", 40);
        insertItem("110", "Ergonomic Chair", "Furniture", "Shelf C1-1", 10);
        insertItem("104", "Laptop Stand", "Accessories", "Shelf A2-2", 35);
        insertItem("106", "Headphones", "Electronics", "Shelf A1-1", 15);
        insertItem("109", "Portable Charger", "Electronics", "Shelf C2-1", 60);
    }
};

ItemTree globalItemTree;

void itemManagementMenu()
{
    globalItemTree.loadItemsFromCSV();
    int choice;

    do
    {
        cout << "\n--- Item Search & Management Menu ---\n";
        cout << "1. Insert New Item\n";
        cout << "2. Search Item by ID\n";
        cout << "3. Search Item by Name\n";
        cout << "4. Update Item Details / Stock\n";
        cout << "5. Delete Item\n";
        cout << "6. Display All Items (Sorted)\n";
        cout << "7. Load Default Items (Demo)\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;
        if (cin.fail()) {
            if (cin.eof()) { choice = 0; break; } // EOF: exit menu
            cin.clear();
            cin.ignore(1000, '\n');
            choice = -1;
        } else {
            cin.ignore(); // clear leftover newline
        }

        switch (choice)
        {
        case 1:
        {
            string id, name, category, location;
            int qty;
            cout << "Enter Item ID: ";
            getline(cin, id);
            cout << "Enter Item Name: ";
            getline(cin, name);
            cout << "Enter Category: ";
            getline(cin, category);
            cout << "Enter Location (Shelf): ";
            getline(cin, location);
            cout << "Enter Initial Stock Quantity: ";
            cin >> qty;
            cin.ignore();
            globalItemTree.insertItem(id, name, category, location, qty);
            break;
        }
        case 2:
        {
            string id;
            cout << "Enter Item ID to search: ";
            getline(cin, id);
            ItemNode *node = globalItemTree.searchByID(id);
            if (node != nullptr)
            {
                cout << "\nItem Found:\n";
                cout << "ID: " << node->itemID
                     << " | Name: " << node->itemName
                     << " | Category: " << node->category
                     << " | Location: " << node->location
                     << " | Stock Count: " << node->quantity << "\n";
            }
            else
            {
                cout << "Item with ID \"" << id << "\" not found.\n";
            }
            break;
        }
        case 3:
        {
            string name;
            cout << "Enter Item Name to search: ";
            getline(cin, name);
            ItemNode *node = globalItemTree.searchByName(name);
            if (node != nullptr)
            {
                cout << "\nItem Found:\n";
                cout << "ID: " << node->itemID
                     << " | Name: " << node->itemName
                     << " | Category: " << node->category
                     << " | Location: " << node->location
                     << " | Stock Count: " << node->quantity << "\n";
            }
            else
            {
                cout << "Item with name \"" << name << "\" not found.\n";
            }
            break;
        }
        case 4:
        {
            string id;
            cout << "Enter Item ID to update: ";
            getline(cin, id);
            ItemNode *node = globalItemTree.searchByID(id);
            if (node != nullptr)
            {
                string name, category, location, qtyStr;
                cout << "Enter new Item Name (leave blank to keep current): ";
                getline(cin, name);
                cout << "Enter new Category (leave blank to keep current): ";
                getline(cin, category);
                cout << "Enter new Location (leave blank to keep current): ";
                getline(cin, location);
                cout << "Enter new Stock Quantity (leave blank to keep current): ";
                getline(cin, qtyStr);

                if (!name.empty())
                    node->itemName = name;
                if (!category.empty())
                    node->category = category;
                if (!location.empty())
                    node->location = location;
                if (!qtyStr.empty()) {
                    try {
                        node->quantity = stoi(qtyStr);
                    } catch (...) {
                        cout << "Invalid quantity entered. Stock quantity was not changed.\n";
                    }
                }

                globalItemTree.saveAllItemsToCSV();
                cout << "Item details updated successfully.\n";
            }
            else
            {
                cout << "Item with ID \"" << id << "\" not found.\n";
            }
            break;
        }
        case 5:
        {
            string id;
            cout << "Enter Item ID to delete: ";
            getline(cin, id);
            globalItemTree.removeItem(id);
            break;
        }
        case 6:
            globalItemTree.displayAllItems();
            break;
        case 7:
            globalItemTree.loadDefaultItems();
            globalItemTree.saveAllItemsToCSV();
            cout << "\nDefault items loaded and saved to CSV.\n";
            break;
        case 0:
            cout << "Returning to main menu...\n";
            break;
        default:
            cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}