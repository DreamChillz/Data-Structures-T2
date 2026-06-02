# Individual File Changelog

This document provides a detailed, file-by-file breakdown of the changes implemented in the Warehouse Robot Navigation System. The modifications encompass core database transitions, feature integration, memory management safety, and robust input validation.

---

### 1. [`main.cpp`](file:///c:/assingments/finish/Data%20Structures/ass/New%20folder/Data-Structures-T2/main.cpp)
*   **End-to-End Workflow Integration:** Tied together all modular components (order queueing, robot status lookup, BFS pathfinding, LIFO return-path execution, stock deduction, and database persistence) into a unified console automation sequence (Option 6).
*   **Menu Input Sanitation (Bug 1 & 2 Fix):** Patched the main menu loop to detect `cin.fail()`. Added recovery logic via `cin.clear()` and `cin.ignore()`, along with `cin.eof()` termination checks to prevent infinite loops when non-numeric inputs or closed stdin streams are encountered.
*   **Queue Overflow Prevention (Bug 3 Fix):** Added a capacity guard checking `if (rear >= 99)` prior to inline order registration in Case 6 to protect the fixed-size queue from stack/heap overflow.
*   **Global Registry References:** Reordered file inclusions and global BST instance references (`globalItemTree`) to support streamlined inventory validation and ID-based product searches.

---

### 2. [`item_management.cpp`](file:///c:/assingments/finish/Data%20Structures/ass/New%20folder/Data-Structures-T2/item_management.cpp)
*   **CSV Database Support:** Developed `loadItemsFromCSV()` and `saveAllItemsToCSV()` to load and persist item attributes (ID, name, category, location, price, and stock quantity) using `items.csv`.
*   **BST Search Optimization:** Replaced slow sequential iteration with a Binary Search Tree structure (`ItemTree`) indexed by `ItemID`. Added optimized ID lookup (`searchByID()`) and Name lookup (`searchByName()`) functions.
*   **Stock Deduction Logic:** Integrated post-retrieval stock deduction, decrementing the corresponding item quantity in memory and writing updates back to `items.csv`.
*   **Buffer Clearance & Menu Fail Protection:** Handled invalid input states in the management sub-menu and cleared trailing newlines with `cin.ignore()` to ensure standard text inputs with `getline()` read input fields correctly.

---

### 3. [`order_management.cpp`](file:///c:/assingments/finish/Data%20Structures/ass/New%20folder/Data-Structures-T2/order_management.cpp)
*   **Item ID Migration:** Updated the order processing schema to link orders to items via their unique string `itemID` instead of their names, matching the database architecture of the items.
*   **Uniqueness Guards:** Implemented `isOrderIDExists()` to verify new order IDs against the active in-memory queue and the historical transactions in `orders.csv` to ensure no duplicates.
*   **Queue Overflow Prevention (Bug 3 Fix):** Patched the `addOrder()` function to prevent array boundary violations by validating array bounds before executing `rear++`.
*   **CSV Database Support:** Implemented CSV parsing for importing incoming orders and appending completed/failed operations to `orders.csv`.
*   **Menu Input Sanitation:** Added fail-state recovery routines for menu choice reading.

---

### 4. [`robot_assignment.cpp`](file:///c:/assingments/finish/Data%20Structures/ass/New%20folder/Data-Structures-T2/robot_assignment.cpp)
*   **CSV Database Support:** Added parser and writer functions to load and save robot status records (ID, name, availability state, battery level, assigned zone, and cumulative task count) directly from/to `robots.csv`.
*   **Load-Balanced Task Assignment:** Developed `assignRobotProgrammatically()` to programmatically select available robots based on their current status and allocate tasks to the least-burdened robot (lowest task count) to achieve load-balanced operations.
*   **Menu Input Sanitation:** Injected input stream reset guards inside `robotAssignmentMenu()` to prevent infinite loops when non-integer values are input.

---

### 5. [`robot_navigation.cpp`](file:///c:/assingments/finish/Data%20Structures/ass/New%20folder/Data-Structures-T2/robot_navigation.cpp)
*   **Reverse Path Generation (LIFO Stack):** Built a LIFO stack processing engine that records a robot's forward movement steps (North, South, East, West, Left, Right) and pops them in reverse, applying cardinal opposite actions to guide the robot back to the packing station.
*   **Sanitized Navigation Logging (Bug 4 Fix):** Wrapped `recordMovement()` direction choice capture with a `cin.fail()` handler to isolate manual step entry and prevent corruption from spilling into the parent loop.
*   **Menu Input Sanitation:** Standardized selection checks and EOF exits within `robotNavigationMenu()`.

---

### 6. [`warehouse_navigation.cpp`](file:///c:/assingments/finish/Data%20Structures/ass/New%20folder/Data-Structures-T2/warehouse_navigation.cpp)
*   **Memory Management Cleanups:** Implemented recursive `clearTree()` utility functions and added a safe class destructor `~WarehouseTree()` to release dynamically allocated tree memory.
*   **Duplicate Location Prevention:** Added structural guards to prevent duplicate Zones, Aisles, or Shelves from being double-registered inside the warehouse layout tree.
*   **Menu Input Sanitation:** Patched menu options validation to protect against non-numeric menu commands.
