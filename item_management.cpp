#pragma once
#include <iostream>
#include <string>
using namespace std;

// Node representing an item in the Binary Search Tree
struct ItemNode
{
    string itemID;
    string itemName;
    string category;
    string location;
    ItemNode *left;
    ItemNode *right;

    ItemNode(string id, string name, string cat, string loc)
    {
        itemID = id;
        itemName = name;
        category = cat;
        location = loc;
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

    // Helper function to insert a node recursively
    ItemNode *insertNode(ItemNode *node, string id, string name, string cat, string loc, bool &success)
    {
        if (node == nullptr)
        {
            success = true;
            return new ItemNode(id, name, cat, loc);
        }

        if (id < node->itemID)
        {
            node->left = insertNode(node->left, id, name, cat, loc, success);
        }
        else if (id > node->itemID)
        {
            node->right = insertNode(node->right, id, name, cat, loc, success);
        }
        else
        {
            success = false; // Duplicate ID found
        }
        return node;
    }

    void insertItem(string id, string name, string cat, string loc)
    {
        bool success = false;
        root = insertNode(root, id, name, cat, loc, success);
        if (success)
        {
            cout << "Item \"" << name << "\" added successfully.\n";
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
             << " | Location: " << node->location << "\n";
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
        insertItem("105", "Wireless Mouse", "Electronics", "Shelf A1-1");
        insertItem("102", "USB-C Hub", "Electronics", "Shelf A1-2");
        insertItem("108", "Desk Lamp", "Furniture", "Shelf B1-1");
        insertItem("101", "Mechanical Keyboard", "Electronics", "Shelf A1-3");
        insertItem("103", "Monitor Stand", "Furniture", "Shelf A2-1");
        insertItem("107", "Webcam HD", "Electronics", "Shelf B1-2");
        insertItem("110", "Ergonomic Chair", "Furniture", "Shelf C1-1");
        insertItem("104", "Laptop Stand", "Accessories", "Shelf A2-2");
        insertItem("106", "Headphones", "Electronics", "Shelf A1-1");
        insertItem("109", "Portable Charger", "Electronics", "Shelf C2-1");
        cout << "\nDefault items loaded successfully.\n";
    }
};

void itemManagementMenu()
{
    ItemTree itemTree;
    int choice;

    do
    {
        cout << "\n--- Item Search & Management Menu ---\n";
        cout << "1. Insert New Item\n";
        cout << "2. Search Item by ID\n";
        cout << "3. Search Item by Name\n";
        cout << "4. Update Item Details\n";
        cout << "5. Delete Item\n";
        cout << "6. Display All Items (Sorted)\n";
        cout << "7. Load Default Items (Demo)\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore(); // clear leftover newline

        switch (choice)
        {
        case 1:
        {
            string id, name, category, location;
            cout << "Enter Item ID: ";
            getline(cin, id);
            cout << "Enter Item Name: ";
            getline(cin, name);
            cout << "Enter Category: ";
            getline(cin, category);
            cout << "Enter Location (Shelf): ";
            getline(cin, location);
            itemTree.insertItem(id, name, category, location);
            break;
        }
        case 2:
        {
            string id;
            cout << "Enter Item ID to search: ";
            getline(cin, id);
            ItemNode *node = itemTree.searchByID(id);
            if (node != nullptr)
            {
                cout << "\nItem Found:\n";
                cout << "ID: " << node->itemID
                     << " | Name: " << node->itemName
                     << " | Category: " << node->category
                     << " | Location: " << node->location << "\n";
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
            ItemNode *node = itemTree.searchByName(name);
            if (node != nullptr)
            {
                cout << "\nItem Found:\n";
                cout << "ID: " << node->itemID
                     << " | Name: " << node->itemName
                     << " | Category: " << node->category
                     << " | Location: " << node->location << "\n";
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
            ItemNode *node = itemTree.searchByID(id);
            if (node != nullptr)
            {
                string name, category, location;
                cout << "Enter new Item Name (leave blank to keep current): ";
                getline(cin, name);
                cout << "Enter new Category (leave blank to keep current): ";
                getline(cin, category);
                cout << "Enter new Location (leave blank to keep current): ";
                getline(cin, location);

                if (!name.empty())
                    node->itemName = name;
                if (!category.empty())
                    node->category = category;
                if (!location.empty())
                    node->location = location;

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
            itemTree.removeItem(id);
            break;
        }
        case 6:
            itemTree.displayAllItems();
            break;
        case 7:
            itemTree.loadDefaultItems();
            break;
        case 0:
            cout << "Returning to main menu...\n";
            break;
        default:
            cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}