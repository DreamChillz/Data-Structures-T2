#pragma once
#include <iostream>
#include <string>
using namespace std;

struct LocationNode
{
    string name;               // e.g. "Zone A", "Aisle 1", "Shelf 3"
    string type;               // "warehouse", "zone", "aisle", "shelf"
    LocationNode *firstChild;  // pointer to first child
    LocationNode *nextSibling; // pointer to next sibling

    LocationNode(string n, string t)
    {
        name = n;
        type = t;
        firstChild = nullptr;
        nextSibling = nullptr;
    }
};

// Queue node used for BFS traversal
struct QueueNode
{
    LocationNode *location;
    QueueNode *next;

    QueueNode(LocationNode *loc)
    {
        location = loc;
        next = nullptr;
    }
};

// Simple queue for BFS
struct LocationQueue
{
    QueueNode *front;
    QueueNode *rear;

    LocationQueue()
    {
        front = nullptr;
        rear = nullptr;
    }

    // Check if queue is empty
    bool isEmpty() { return front == nullptr; }

    // Add a location to the back of the queue
    void enqueue(LocationNode *loc)
    {
        QueueNode *newNode = new QueueNode(loc);
        if (rear == nullptr)
        {
            front = rear = newNode;
        }
        else
        {
            rear->next = newNode;
            rear = newNode;
        }
    }

    // Remove and return the front location
    LocationNode *dequeue()
    {
        if (isEmpty())
            return nullptr;
        QueueNode *temp = front;
        LocationNode *loc = temp->location;
        front = front->next;
        if (front == nullptr)
            rear = nullptr;
        delete temp;
        return loc;
    }
};

// Path stack used to record BFS parent trail
struct PathStack
{
    LocationNode **data;
    int top;
    int capacity;

    PathStack(int cap)
    {
        top = -1;
        capacity = cap;
        data = new LocationNode *[cap];
    }

    ~PathStack() { delete[] data; }

    bool isEmpty() { return top == -1; }

    void push(LocationNode *loc)
    {
        if (top < capacity - 1)
            data[++top] = loc;
    }

    LocationNode *pop()
    {
        if (isEmpty())
            return nullptr;
        return data[top--];
    }

    LocationNode *peek() { return isEmpty() ? nullptr : data[top]; }
};

struct WarehouseTree
{
    LocationNode *root; 

    WarehouseTree() : root(nullptr) {}

    // Create the root warehouse node
    void initWarehouse(string warehouseName)
    {
        if (root != nullptr)
        {
            cout << "Warehouse already initialised.\n";
            return;
        }
        root = new LocationNode(warehouseName, "warehouse");
        cout << "Warehouse \"" << warehouseName << "\" created.\n";
    }

    // find a node by name using BFS
    LocationNode *findNode(const string &name)
    {
        if (root == nullptr)
            return nullptr;

        LocationQueue q;
        q.enqueue(root);

        while (!q.isEmpty())
        {
            LocationNode *current = q.dequeue();

            if (current->name == name)
                return current;

            // Enqueue all children
            LocationNode *child = current->firstChild;
            while (child != nullptr)
            {
                q.enqueue(child);
                child = child->nextSibling;
            }
        }
        return nullptr; // not found
    }

    // Add a child node under a given parent
    void addLocation(const string &parentName,
                     const string &childName,
                     const string &childType)
    {
        if (root == nullptr)
        {
            cout << "Warehouse not initialised. Please create warehouse first.\n";
            return;
        }

        LocationNode *parent = findNode(parentName);
        if (parent == nullptr)
        {
            cout << "Parent location \"" << parentName << "\" not found.\n";
            return;
        }

        // Enforce rules:
        // warehouse -> zone -> aisle -> shelf
        bool validHierarchy = false;
        if (parent->type == "warehouse" && childType == "zone")
            validHierarchy = true;
        else if (parent->type == "zone" && childType == "aisle")
            validHierarchy = true;
        else if (parent->type == "aisle" && childType == "shelf")
            validHierarchy = true;

        if (!validHierarchy)
        {
            cout << "Invalid hierarchy: cannot add type \"" << childType
                 << "\" under \"" << parent->type << "\".\n";
            cout << "Allowed rules:\n";
            cout << "  warehouse -> zone\n";
            cout << "  zone      -> aisle\n";
            cout << "  aisle     -> shelf\n";
            return;
        }

        LocationNode *newLoc = new LocationNode(childName, childType);

        // Insert as the last sibling of parent's children
        if (parent->firstChild == nullptr)
        {
            parent->firstChild = newLoc;
        }
        else
        {
            LocationNode *sibling = parent->firstChild;
            while (sibling->nextSibling != nullptr)
                sibling = sibling->nextSibling;
            sibling->nextSibling = newLoc;
        }

        cout << childType << " \"" << childName
             << "\" added under \"" << parentName << "\".\n";
    }

    //  Display the full warehouse layout
    void dfsDisplay(LocationNode *node, int depth)
    {
        if (node == nullptr)
            return;

        // Print indentation + tree connector
        for (int i = 0; i < depth; i++)
            cout << "    ";
        cout << "|-- [" << node->type << "] " << node->name << "\n";

        // Recurse into children (first child)
        dfsDisplay(node->firstChild, depth + 1);

        // Recurse into siblings
        dfsDisplay(node->nextSibling, depth);
    }

    void displayLayout()
    {
        if (root == nullptr)
        {
            cout << "Warehouse is empty.\n";
            return;
        }
        cout << "\n--- Warehouse Layout (DFS Traversal) ---\n";
        cout << "[warehouse] " << root->name << "\n";
        dfsDisplay(root->firstChild, 1);
    }

    // BFS: Find and print the path from root to a target node
    void bfsNavigate(const string &targetName)
    {
        if (root == nullptr)
        {
            cout << "Warehouse is empty.\n";
            return;
        }

        const int MAX_NODES = 200;

        // Arrays to track visited nodes and their parents for path reconstruction
        LocationNode *visited[MAX_NODES];
        LocationNode *parent[MAX_NODES];
        int visitedCount = 0;

        // Initialise all parent entries to nullptr
        for (int i = 0; i < MAX_NODES; i++)
        {
            visited[i] = nullptr;
            parent[i] = nullptr;
        }

        visited[visitedCount] = root;
        parent[visitedCount] = nullptr;
        visitedCount++;

        LocationQueue q;
        q.enqueue(root);

        LocationNode *target = nullptr;

        // BFS loop
        while (!q.isEmpty())
        {
            LocationNode *current = q.dequeue();

            if (current->name == targetName)
            {
                target = current;
                break;
            }

            LocationNode *child = current->firstChild;
            while (child != nullptr)
            {
                // Record child and its parent
                visited[visitedCount] = child;
                parent[visitedCount] = current;
                visitedCount++;

                q.enqueue(child);
                child = child->nextSibling;
            }
        }

        if (target == nullptr)
        {
            cout << "Location \"" << targetName << "\" not found.\n";
            return;
        }

        // Reconstruct path from target back to root using parent array
        // Store path in a stack then print in forward order
        PathStack pathStack(MAX_NODES);
        LocationNode *step = target;

        while (step != nullptr)
        {
            pathStack.push(step);

            // Find parent of step
            LocationNode *par = nullptr;
            for (int i = 0; i < visitedCount; i++)
            {
                if (visited[i] == step)
                {
                    par = parent[i];
                    break;
                }
            }
            step = par;
        }

        // Print path forward (root -> ... -> target)
        cout << "\n--- Navigation Route to \"" << targetName << "\" (BFS) ---\n";
        int stepNum = 1;
        while (!pathStack.isEmpty())
        {
            LocationNode *loc = pathStack.pop();
            cout << "Step " << stepNum++ << ": ["
                 << loc->type << "] " << loc->name << "\n";
        }
        cout << "Destination reached: " << targetName << "\n";
    }

    // BFS: Level-order display (shows tree level by level)
    void bfsDisplay()
    {
        if (root == nullptr)
        {
            cout << "Warehouse is empty.\n";
            return;
        }

        cout << "\n--- Warehouse Layout (BFS Level-Order) ---\n";
        LocationQueue q;
        q.enqueue(root);
        int level = 0;

        while (!q.isEmpty())
        {
            // Count nodes at current level
            int levelSize = 0;
            LocationQueue temp;

            // We cannot check size directly, so we use a second pass trick:
            // Drain q into temp to count, then restore
            while (!q.isEmpty())
            {
                LocationNode *n = q.dequeue();
                temp.enqueue(n);
                levelSize++;
            }

            cout << "Level " << level << ": ";
            for (int i = 0; i < levelSize; i++)
            {
                LocationNode *current = temp.dequeue();
                cout << current->name;
                if (i < levelSize - 1)
                    cout << "  |  ";

                // Re-enqueue children for next level
                LocationNode *child = current->firstChild;
                while (child != nullptr)
                {
                    q.enqueue(child);
                    child = child->nextSibling;
                }
            }
            cout << "\n";
            level++;
        }
    }

    // Load a default warehouse layout for demonstration
    void loadDefaultLayout()
    {
        initWarehouse("Main Warehouse");

        // Zones
        addLocation("Main Warehouse", "Zone A", "zone");
        addLocation("Main Warehouse", "Zone B", "zone");
        addLocation("Main Warehouse", "Zone C", "zone");

        // Zone A aisles
        addLocation("Zone A", "Aisle A1", "aisle");
        addLocation("Zone A", "Aisle A2", "aisle");

        // Zone B aisles
        addLocation("Zone B", "Aisle B1", "aisle");

        // Zone C aisles
        addLocation("Zone C", "Aisle C1", "aisle");
        addLocation("Zone C", "Aisle C2", "aisle");

        // Shelves under Aisle A1
        addLocation("Aisle A1", "Shelf A1-1", "shelf");
        addLocation("Aisle A1", "Shelf A1-2", "shelf");
        addLocation("Aisle A1", "Shelf A1-3", "shelf");

        // Shelves under Aisle A2
        addLocation("Aisle A2", "Shelf A2-1", "shelf");
        addLocation("Aisle A2", "Shelf A2-2", "shelf");

        // Shelves under Aisle B1
        addLocation("Aisle B1", "Shelf B1-1", "shelf");
        addLocation("Aisle B1", "Shelf B1-2", "shelf");

        // Shelves under Aisle C1
        addLocation("Aisle C1", "Shelf C1-1", "shelf");

        // Shelves under Aisle C2
        addLocation("Aisle C2", "Shelf C2-1", "shelf");
        addLocation("Aisle C2", "Shelf C2-2", "shelf");

        cout << "\nDefault warehouse layout loaded successfully.\n";
    }
};

void warehouseNavigationMenu()
{
    WarehouseTree warehouse;
    int choice;

    do
    {
        cout << "\n--- Warehouse Layout & Navigation Menu ---\n";
        cout << "1. Initialise Warehouse\n";
        cout << "2. Add Location (Zone / Aisle / Shelf)\n";
        cout << "3. Display Layout (DFS)\n";
        cout << "4. Display Layout (BFS Level-Order)\n";
        cout << "5. Navigate to Location (BFS Pathfinding)\n";
        cout << "6. Load Default Layout (Demo)\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore(); // clear leftover newline from cin >> choice

        switch (choice)
        {
        case 1:
        {
            string name;
            cout << "Enter warehouse name: ";
            getline(cin, name);
            warehouse.initWarehouse(name);
            break;
        }
        case 2:
        {
            string parent, child, type;
            cout << "Enter parent location name: ";
            getline(cin, parent);
            cout << "Enter new location name: ";
            getline(cin, child);
            cout << "Enter type (zone / aisle / shelf): ";
            getline(cin, type);
            warehouse.addLocation(parent, child, type);
            break;
        }
        case 3:
            warehouse.displayLayout();
            break;
        case 4:
            warehouse.bfsDisplay();
            break;
        case 5:
        {
            string target;
            cout << "Enter destination location name: ";
            getline(cin, target);
            warehouse.bfsNavigate(target);
            break;
        }
        case 6:
            warehouse.loadDefaultLayout();
            break;
        case 0:
            cout << "Returning to main menu...\n";
            break;
        default:
            cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}