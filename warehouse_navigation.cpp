#pragma once
#include <iostream>
#include <string>
using namespace std;

//  SECTION 1 — TREE NODE

struct LocationNode
{
    string name;               // e.g. "Zone A", "Aisle A1", "Shelf A1-2"
    string type;               // "warehouse" | "zone" | "aisle" | "shelf"
    LocationNode *firstChild;  // leftmost child in the hierarchy
    LocationNode *nextSibling; // next node at the same level

    LocationNode(string n, string t)
    {
        name = n;
        type = t;
        firstChild = nullptr;
        nextSibling = nullptr;
    }
};

//  SECTION 2 — QUEUE  (for BFS)

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

struct LocationQueue
{
    QueueNode *front;
    QueueNode *rear;

    LocationQueue()
    {
        front = nullptr;
        rear = nullptr;
    }

    bool isEmpty() const { return front == nullptr; }

    // Add a location pointer to the back of the queue
    void enqueue(LocationNode *loc)
    {
        QueueNode *newNode = new QueueNode(loc);
        if (rear == nullptr)
            front = rear = newNode;
        else
        {
            rear->next = newNode;
            rear = newNode;
        }
    }

    // Remove and return the front location pointer
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

    // Count how many nodes are currently in the queue
    // Used by bfsDisplay() to process one level at a time
    int size() const
    {
        int count = 0;
        QueueNode *cur = front;
        while (cur != nullptr)
        {
            ++count;
            cur = cur->next;
        }
        return count;
    }
};

//  SECTION 3 — STACK  (for path reconstruction)

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

    bool isEmpty() const { return top == -1; }
    bool isFull() const { return top == capacity - 1; }

    // Push a location pointer onto the stack
    void push(LocationNode *loc)
    {
        if (!isFull())
            data[++top] = loc;
    }

    // Pop and return the top pointer
    LocationNode *pop()
    {
        return isEmpty() ? nullptr : data[top--];
    }

    LocationNode *peek() const
    {
        return isEmpty() ? nullptr : data[top];
    }
};

//  SECTION 4 — WAREHOUSE TREE

struct WarehouseTree
{
    LocationNode *root;

    WarehouseTree()
    {
        root = nullptr;
    }

    //  initWarehouse
    void initWarehouse(const string &warehouseName)
    {
        if (root != nullptr)
        {
            cout << "Warehouse already initialised.\n";
            return;
        }
        root = new LocationNode(warehouseName, "warehouse");
        cout << "Warehouse \"" << warehouseName << "\" created.\n";
    }

    //  findNode  (BFS search)
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

            // Enqueue all children of current node
            LocationNode *child = current->firstChild;
            while (child != nullptr)
            {
                q.enqueue(child);
                child = child->nextSibling;
            }
        }
        return nullptr;
    }

    //  addLocation
    //  Inserts a new child node under an existing parent.
    void addLocation(const string &parentName, const string &childName, const string &childType)
    {
        if (root == nullptr)
        {
            cout << "Warehouse not initialised. Create warehouse first.\n";
            return;
        }

        LocationNode *parent = findNode(parentName);
        if (parent == nullptr)
        {
            cout << "Parent location \"" << parentName << "\" not found.\n";
            return;
        }

        // Validate parent→child type pairing
        bool validPair = (parent->type == "warehouse" && childType == "zone") ||
                         (parent->type == "zone" && childType == "aisle") ||
                         (parent->type == "aisle" && childType == "shelf");

        if (!validPair)
        {
            cout << "Invalid hierarchy: cannot add type \""
                 << childType << "\" under \"" << parent->type << "\".\n";
            cout << "    Allowed: warehouse→zone | zone→aisle | aisle→shelf\n";
            return;
        }

        LocationNode *newLoc = new LocationNode(childName, childType);

        // Append as last sibling under parent
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

    //  displayLayout  (DFS — depth-first, pre-order)
    void displayLayout()
    {
        if (root == nullptr)
        {
            cout << "Warehouse is empty.\n";
            return;
        }
        cout << "\n=== Warehouse Layout (DFS - Pre-order) ===\n";
        cout << "[warehouse] " << root->name << "\n";
        dfsHelper(root->firstChild, 1);
    }

    //  bfsDisplay  (BFS — level-order)
    void bfsDisplay()
    {
        if (root == nullptr)
        {
            cout << "Warehouse is empty.\n";
            return;
        }

        cout << "\n=== Warehouse Layout (BFS - Level-Order) ===\n";

        LocationQueue q;
        q.enqueue(root);
        int level = 0;

        while (!q.isEmpty())
        {
            int levelSize = q.size(); // nodes at current level
            cout << "Level " << level << ": ";

            for (int i = 0; i < levelSize; i++)
            {
                LocationNode *current = q.dequeue();
                cout << "[" << current->type << "] " << current->name;
                if (i < levelSize - 1)
                    cout << "  |  ";

                // Enqueue children for the next level
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

    //  bfsNavigate
    //  Finds a route from the warehouse root to targetName
    void bfsNavigate(const string &targetName)
    {
        if (root == nullptr)
        {
            cout << "Warehouse is empty.\n";
            return;
        }

        const int MAX_NODES = 200;

        LocationNode *visited[MAX_NODES];
        LocationNode *parentOf[MAX_NODES];
        int visitedCount = 0;

        for (int i = 0; i < MAX_NODES; i++)
            visited[i] = parentOf[i] = nullptr;

        visited[visitedCount] = root;
        parentOf[visitedCount] = nullptr;
        visitedCount++;

        LocationQueue q;
        q.enqueue(root);
        LocationNode *target = nullptr;

        // stop as soon as target is found
        while (!q.isEmpty() && target == nullptr)
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
                if (visitedCount < MAX_NODES)
                {
                    visited[visitedCount] = child;
                    parentOf[visitedCount] = current;
                    visitedCount++;
                }
                q.enqueue(child);
                child = child->nextSibling;
            }
        }

        if (target == nullptr)
        {
            cout << "Location \"" << targetName << "\" not found.\n";
            return;
        }

        // Walk parent pointers from target → root, push onto stack
        PathStack pathStack(MAX_NODES);
        LocationNode *step = target;

        while (step != nullptr)
        {
            pathStack.push(step);

            // Look up the parent of 'step'
            LocationNode *par = nullptr;
            for (int i = 0; i < visitedCount; i++)
            {
                if (visited[i] == step)
                {
                    par = parentOf[i];
                    break;
                }
            }
            step = par;
        }

        cout << "\n=== Navigation Route to \"" << targetName << "\" (BFS) ===\n";
        int stepNum = 1;
        while (!pathStack.isEmpty())
        {
            LocationNode *loc = pathStack.pop();
            cout << "  Step " << stepNum++ << ": ["
                 << loc->type << "] " << loc->name << "\n";
        }
        cout << "  >> Destination reached: " << targetName << "\n";
    }

    //  Populates a sample warehouse for demo / testing.
    void loadDefaultLayout()
    {
        initWarehouse("Main Warehouse");

        // --- Zones ---
        addLocation("Main Warehouse", "Zone A", "zone");
        addLocation("Main Warehouse", "Zone B", "zone");
        addLocation("Main Warehouse", "Zone C", "zone");

        // --- Zone A ---
        addLocation("Zone A", "Aisle A1", "aisle");
        addLocation("Zone A", "Aisle A2", "aisle");
        addLocation("Aisle A1", "Shelf A1-1", "shelf");
        addLocation("Aisle A1", "Shelf A1-2", "shelf");
        addLocation("Aisle A1", "Shelf A1-3", "shelf");
        addLocation("Aisle A2", "Shelf A2-1", "shelf");
        addLocation("Aisle A2", "Shelf A2-2", "shelf");

        // --- Zone B ---
        addLocation("Zone B", "Aisle B1", "aisle");
        addLocation("Aisle B1", "Shelf B1-1", "shelf");
        addLocation("Aisle B1", "Shelf B1-2", "shelf");

        // --- Zone C ---
        addLocation("Zone C", "Aisle C1", "aisle");
        addLocation("Zone C", "Aisle C2", "aisle");
        addLocation("Aisle C1", "Shelf C1-1", "shelf");
        addLocation("Aisle C2", "Shelf C2-1", "shelf");
        addLocation("Aisle C2", "Shelf C2-2", "shelf");

        cout << "\nDefault warehouse layout loaded.\n";
    }

private:
    void dfsHelper(LocationNode *node, int depth)
    {
        if (node == nullptr)
            return;

        // Indent to show depth
        for (int i = 0; i < depth; i++)
            cout << "    ";
        cout << "|-- [" << node->type << "] " << node->name << "\n";

        dfsHelper(node->firstChild, depth + 1); // go deeper
        dfsHelper(node->nextSibling, depth);    // stay at same level
    }
};

//  SECTION 5 — MENU

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
        cin.ignore();

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
            cout << "Enter destination name: ";
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
            cout << "Invalid choice. Try again.\n";
        }

    } while (choice != 0);
}