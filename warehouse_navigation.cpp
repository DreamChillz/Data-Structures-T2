#pragma once
#include <iostream>
#include <string>
using namespace std;

//  SECTION 1 — TREE NODE

struct LocationNode
{
    string name;
    string type;
    LocationNode *firstChild;
    LocationNode *nextSibling;
    int x, y;       // 2-D grid coordinates; -1 = unset
    bool isBlocked; // true = robot cannot enter this cell

    LocationNode(string n, string t)
    {
        name = n;
        type = t;
        firstChild = nullptr;
        nextSibling = nullptr;
        x = -1;
        y = -1;
        isBlocked = false;
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

    void push(LocationNode *loc)
    {
        if (!isFull())
            data[++top] = loc;
    }

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
    static const int GMAX = 12;

    struct Obstacle { int x, y; };

    LocationNode *root;
    Obstacle obstacles[50]; // positioned the same way as location nodes
    int obstacleCount;

    WarehouseTree()
    {
        root = nullptr;
        obstacleCount = 0;
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

    //  findNode  (BFS — visits all nodes regardless of isBlocked)
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

        bool validPair = (parent->type == "warehouse" && childType == "zone") ||
                         (parent->type == "zone" && childType == "aisle") ||
                         (parent->type == "aisle" && childType == "shelf");

        if (!validPair)
        {
            cout << "Invalid hierarchy: cannot add type \""
                 << childType << "\" under \"" << parent->type << "\".\n";
            cout << "    Allowed: warehouse->zone | zone->aisle | aisle->shelf\n";
            return;
        }

        LocationNode *newLoc = new LocationNode(childName, childType);

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

    //  setCoords — assign 2-D grid position to a named node (silent)
    void setCoords(const string &name, int x, int y)
    {
        LocationNode *node = findNode(name);
        if (node)
        {
            node->x = x;
            node->y = y;
        }
    }

    //  setBlocked — mark or clear an obstacle on a named node
    void setBlocked(const string &name, bool blocked)
    {
        LocationNode *node = findNode(name);
        if (node == nullptr)
        {
            cout << "Location \"" << name << "\" not found.\n";
            return;
        }
        node->isBlocked = blocked;
        // Sync to the shared obstacle list using the node's grid coordinates
        if (node->x >= 0 && node->y >= 0)
            setGridObstacle(node->x, node->y, blocked);
        else
            cout << "\"" << name << "\" is now "
                 << (blocked ? "BLOCKED (obstacle)." : "UNBLOCKED.") << "\n";
    }

    //  setGridObstacle — place or remove an obstacle at any (x,y) grid cell
    void setGridObstacle(int x, int y, bool blocked)
    {
        if (x < 0 || y < 0 || x >= GMAX || y >= GMAX)
        {
            cout << "Coordinates (" << x << "," << y << ") are out of grid bounds.\n";
            return;
        }
        if (blocked)
        {
            // Add if not already present
            if (!isObstacleAt(x, y) && obstacleCount < 50)
            {
                obstacles[obstacleCount].x = x;
                obstacles[obstacleCount].y = y;
                obstacleCount++;
            }
        }
        else
        {
            // Remove by shifting remaining entries left
            for (int i = 0; i < obstacleCount; i++)
            {
                if (obstacles[i].x == x && obstacles[i].y == y)
                {
                    obstacles[i] = obstacles[--obstacleCount];
                    break;
                }
            }
        }
        // Sync the node at this cell if one exists
        if (root)
        {
            LocationQueue q;
            q.enqueue(root);
            while (!q.isEmpty())
            {
                LocationNode *cur = q.dequeue();
                if (cur->x == x && cur->y == y) { cur->isBlocked = blocked; break; }
                for (LocationNode *ch = cur->firstChild; ch; ch = ch->nextSibling)
                    q.enqueue(ch);
            }
        }
        cout << "Cell (" << x << "," << y << ") is now "
             << (blocked ? "BLOCKED." : "UNBLOCKED.") << "\n";
    }

    //  displayGrid — ASCII 2-D map of the warehouse
    //  Pass highlightPath / pathLen to mark the robot's route with > <
    void displayGrid(string *highlightPath = nullptr, int pathLen = 0)
    {
        if (root == nullptr)
        {
            cout << "Warehouse is empty.\n";
            return;
        }

        string lbl[GMAX][GMAX];
        bool pth[GMAX][GMAX] = {};
        int maxX = 0, maxY = 0;

        // BFS: place every positioned node onto the grid
        LocationQueue q;
        q.enqueue(root);
        while (!q.isEmpty())
        {
            LocationNode *cur = q.dequeue();
            if (cur->x >= 0 && cur->y >= 0 && cur->x < GMAX && cur->y < GMAX)
            {
                lbl[cur->y][cur->x] = nodeLabel(cur); // exactly 4 chars
                if (cur->x > maxX) maxX = cur->x;
                if (cur->y > maxY) maxY = cur->y;
            }
            for (LocationNode *ch = cur->firstChild; ch; ch = ch->nextSibling)
                q.enqueue(ch);
        }

        // Expand grid bounds to include obstacle positions
        for (int i = 0; i < obstacleCount; i++)
        {
            if (obstacles[i].x > maxX) maxX = obstacles[i].x;
            if (obstacles[i].y > maxY) maxY = obstacles[i].y;
        }

        // Mark path nodes
        if (highlightPath)
            for (int i = 0; i < pathLen; i++)
            {
                LocationNode *n = findNode(highlightPath[i]);
                if (n && n->x >= 0 && n->y >= 0 && n->x < GMAX && n->y < GMAX)
                    pth[n->y][n->x] = true;
            }

        cout << "\n=== 2D Warehouse Grid ===\n";

        // Column header — each column is 7 chars wide (6 cell + 1 space)
        cout << "     "; // 5-char indent matching row label width
        for (int c = 0; c <= maxX; c++)
        {
            string h = " [" + to_string(c) + "]  "; // " [0]  " = 6 chars
            cout << h << " ";
        }
        cout << "\n";

        for (int r = 0; r <= maxY; r++)
        {
            // Row label: "[0]  " = 5 chars
            cout << "[" << r << "]  ";

            for (int c = 0; c <= maxX; c++)
            {
                string cell;
                bool obs = isObstacleAt(c, r);
                if (obs && lbl[r][c].empty())
                    cell = "[####]"; // obstacle on an empty cell
                else if (obs)
                    cell = "[BLKD]"; // obstacle on a named location
                else if (pth[r][c])
                    cell = ">" + lbl[r][c] + "<"; // on robot path
                else if (!lbl[r][c].empty())
                    cell = "[" + lbl[r][c] + "]"; // normal node
                else
                    cell = "      "; // empty cell

                cout << cell << " ";
            }
            cout << "\n";
        }
        cout << "Legend: [abcd]=location  [####]=obstacle\n";
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
            int levelSize = q.size();
            cout << "Level " << level << ": ";

            for (int i = 0; i < levelSize; i++)
            {
                LocationNode *current = q.dequeue();
                cout << "[" << current->type << "] " << current->name;
                if (i < levelSize - 1)
                    cout << "  |  ";

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

    //  bfsNavigate — finds and prints route from root to target
    //  Skips blocked nodes: if an obstacle blocks the path, navigation fails
    void bfsNavigate(const string &targetName)
    {
        if (root == nullptr)
        {
            cout << "Warehouse is empty.\n";
            return;
        }

        // Pre-check: target exists and isn't itself an obstacle
        LocationNode *chk = findNode(targetName);
        if (chk == nullptr)
        {
            cout << "Location \"" << targetName << "\" not found.\n";
            return;
        }
        if (isNodeBlocked(chk))
        {
            cout << "\"" << targetName << "\" is blocked. Navigation aborted.\n";
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
                // Skip blocked nodes — robot cannot pass through them
                if (!isNodeBlocked(child) && visitedCount < MAX_NODES)
                {
                    visited[visitedCount] = child;
                    parentOf[visitedCount] = current;
                    visitedCount++;
                    q.enqueue(child);
                }
                child = child->nextSibling;
            }
        }

        if (target == nullptr)
        {
            cout << "\"" << targetName << "\" is unreachable";
            cout << " (a blocked location may be obstructing the path).\n";
            return;
        }

        PathStack pathStack(MAX_NODES);
        LocationNode *step = target;

        while (step != nullptr)
        {
            pathStack.push(step);
            LocationNode *par = nullptr;
            for (int i = 0; i < visitedCount; i++)
                if (visited[i] == step) { par = parentOf[i]; break; }
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

    //  getPathToShelf — integration function called by main.cpp
    //  Returns number of steps written to outPath[], or 0 on failure
    int getPathToShelf(const string &targetName, string outPath[])
    {
        if (root == nullptr)
            return 0;

        LocationNode *chk = findNode(targetName);
        if (chk == nullptr)
        {
            cout << "\"" << targetName << "\" not found.\n";
            return 0;
        }
        if (isNodeBlocked(chk))
        {
            cout << "\"" << targetName << "\" is blocked. Route unavailable.\n";
            return 0;
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
                if (!isNodeBlocked(child) && visitedCount < MAX_NODES)
                {
                    visited[visitedCount] = child;
                    parentOf[visitedCount] = current;
                    visitedCount++;
                    q.enqueue(child);
                }
                child = child->nextSibling;
            }
        }

        if (target == nullptr)
        {
            cout << "\"" << targetName << "\" is unreachable";
            cout << " (check for blocked locations on the path).\n";
            return 0;
        }

        PathStack pathStack(MAX_NODES);
        LocationNode *step = target;

        while (step != nullptr)
        {
            pathStack.push(step);
            LocationNode *par = nullptr;
            for (int i = 0; i < visitedCount; i++)
                if (visited[i] == step) { par = parentOf[i]; break; }
            step = par;
        }

        int count = 0;
        while (!pathStack.isEmpty())
            outPath[count++] = pathStack.pop()->name;

        return count;
    }

    //  getDirections — converts a name path into cardinal directions using x,y coords
    //  Fills outDirs[] with one direction per step (pathLen-1 entries).
    //  Returns the number of direction strings written.
    //
    //  4-direction rule (dominant axis wins):
    //    |dx| >= |dy|  ->  East (dx>0) or West (dx<0)
    //    |dy|  > |dx|  ->  South (dy>0) or North (dy<0)
    int getDirections(string pathNames[], int pathLen, string outDirs[])
    {
        int count = 0;
        for (int i = 0; i < pathLen - 1; i++)
        {
            LocationNode *curr = findNode(pathNames[i]);
            LocationNode *next = findNode(pathNames[i + 1]);

            // Fallback if coordinates are not set on either node
            if (!curr || !next || curr->x < 0 || next->x < 0)
            {
                outDirs[count++] = "Forward";
                continue;
            }

            int dx = next->x - curr->x;
            int dy = next->y - curr->y;

            string dir;
            if (dx == 0 && dy == 0)    dir = "Stay";
            else if (abs(dx) >= abs(dy))
                dir = (dx > 0) ? "East" : "West";
            else
                dir = (dy > 0) ? "South" : "North";

            outDirs[count++] = dir;
        }
        return count;
    }

    //  loadDefaultLayout — builds sample warehouse and assigns 2-D coordinates
    //
    //  Grid layout (row x col):
    //       0     1     2     3     4     5     6     7     8     9
    //  [0]                          WH
    //  [1]        ZA                      ZB               ZC
    //  [2]  A1         A2               AB1         AC1         AC2
    //  [3]  A1-1 A1-2 A1-3 A2-1 A2-2 B1-1 B1-2 C1-1 C2-1 C2-2
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

        // --- Assign 2-D grid coordinates ---
        // Row 0: warehouse root
        setCoords("Main Warehouse", 4, 0);
        // Row 1: zones
        setCoords("Zone A",         1, 1);
        setCoords("Zone B",         5, 1);
        setCoords("Zone C",         8, 1);
        // Row 2: aisles
        setCoords("Aisle A1",       0, 2);
        setCoords("Aisle A2",       2, 2);
        setCoords("Aisle B1",       5, 2);
        setCoords("Aisle C1",       7, 2);
        setCoords("Aisle C2",       9, 2);
        // Row 3: shelves
        setCoords("Shelf A1-1",     0, 3);
        setCoords("Shelf A1-2",     1, 3);
        setCoords("Shelf A1-3",     2, 3);
        setCoords("Shelf A2-1",     3, 3);
        setCoords("Shelf A2-2",     4, 3);
        setCoords("Shelf B1-1",     5, 3);
        setCoords("Shelf B1-2",     6, 3);
        setCoords("Shelf C1-1",     7, 3);
        setCoords("Shelf C2-1",     8, 3);
        setCoords("Shelf C2-2",     9, 3);

        cout << "\nDefault warehouse layout loaded (with 2D grid coordinates).\n";
    }

private:
    // Returns a fixed 4-char label for a node, used by displayGrid()
    static string nodeLabel(LocationNode *node)
    {
        string result;
        if (node->type == "warehouse")
        {
            result = " WH ";
        }
        else
        {
            size_t pos = node->name.rfind(' ');
            string last = (pos != string::npos) ? node->name.substr(pos + 1) : node->name;
            if (node->type == "zone")
                last = "Z" + last; // "Zone A" -> "ZA"
            result = last;
            while ((int)result.length() < 4) result += ' ';
            result = result.substr(0, 4);
        }
        return result; // exactly 4 chars
    }

    bool isObstacleAt(int x, int y) const
    {
        for (int i = 0; i < obstacleCount; i++)
            if (obstacles[i].x == x && obstacles[i].y == y) return true;
        return false;
    }

    bool isNodeBlocked(LocationNode *node) const
    {
        if (node->isBlocked) return true;
        if (node->x >= 0 && node->y >= 0)
            return isObstacleAt(node->x, node->y);
        return false;
    }

    void dfsHelper(LocationNode *node, int depth)
    {
        if (node == nullptr)
            return;

        for (int i = 0; i < depth; i++)
            cout << "    ";
        cout << "|-- [" << node->type << "] " << node->name << "\n";

        dfsHelper(node->firstChild, depth + 1);
        dfsHelper(node->nextSibling, depth);
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
        cout << "7. Display 2D Grid Map\n";
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
        case 7:
            warehouse.displayGrid();
            break;
        case 0:
            cout << "Returning to main menu...\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }

    } while (choice != 0);
}
