# Walkthrough — Testing & Bug Fix Session

## Summary

Performed comprehensive automated testing across all 6 modules of the Warehouse Robot Navigation System. Found and fixed **4 critical bugs** during testing.

---

## Bugs Found & Fixed

### Bug 1 — Infinite Loop on Non-Numeric Menu Input (CRITICAL)
**File(s):** `main.cpp`, `robot_assignment.cpp`, `order_management.cpp`, `item_management.cpp`, `robot_navigation.cpp`, `warehouse_navigation.cpp`

**Root Cause:** All menu loops used `cin >> choice` without handling `cin`'s fail state. When a user typed a non-integer (e.g., `abc`), `cin` entered a failed state and would re-read the same bad bytes on every loop iteration, causing an infinite loop.

**Fix:** Added `cin.fail()` check after every `cin >> choice`:
```cpp
cin >> choice;
if (cin.fail()) {
    if (cin.eof()) { choice = 0; break; } // EOF: exit menu gracefully
    cin.clear();
    cin.ignore(1000, '\n');
    choice = -1; // triggers "Invalid choice" message
}
```
Also added `cin.ignore()` in the `else` branch of sub-menus to consume the trailing `\n` before `getline()` calls.

---

### Bug 2 — Infinite Loop on EOF / Pipe Exhaustion (CRITICAL)
**File(s):** Same 6 menu files

**Root Cause:** When stdin was closed (e.g., piped input exhausted), `cin.fail()` returned `true` AND `cin.eof()` returned `true`. The original fix set `choice = -1`, which kept the `do { ... } while (choice != 0)` loop alive forever.

**Fix:** Added explicit `cin.eof()` check inside the fail handler. If EOF, `choice = 0` and `break` exits the loop immediately.

---

### Bug 3 — Buffer Overflow on orderQueue (CRITICAL)
**File(s):** `order_management.cpp`, `main.cpp`

**Root Cause:** `addOrder()` and the inline order creation in Case 6 of `main.cpp` performed `rear++; orderQueue[rear] = newOrder;` without first checking if `rear >= 99`. With a 100-element fixed array (`orderQueue[100]`), this caused a heap/stack buffer overflow if more than 100 pending orders were added.

**Fix:** Added a capacity guard before enqueuing:
```cpp
if (rear >= 99) {
    cout << "Order queue is full (max 100 orders). Cannot add more orders.\n";
    return;
}
```
Applied in both `addOrder()` and `main.cpp`'s case 6 inline path.

---

### Bug 4 — recordMovement() Direction Input Not Validated
**File:** `robot_navigation.cpp`

**Root Cause:** `cin >> dirChoice` in `recordMovement()` had no fail-state recovery. Invalid text input would corrupt `cin` state and propagate to the outer menu loop.

**Fix:** Added `cin.fail()` check with `cin.clear()` + `cin.ignore()` and early `return`.

---

## Tests Performed

| # | Test | Result |
|---|------|--------|
| 1 | Order: non-int OrderID, duplicate OrderID, invalid ItemID, zero quantity | ✅ All rejected with proper error messages |
| 2 | Order: add valid order, view pending, view current, process, view completed | ✅ All work correctly |
| 3 | Robot Assignment: duplicate Robot ID, assign to non-existent robot, invalid status value | ✅ All rejected correctly |
| 4 | Item Management: search by valid ID, search by name, display all sorted | ✅ Works correctly |
| 5 | Warehouse Navigation: path to valid shelf, path to non-existent location, BFS layout, 2D grid | ✅ Works correctly |
| 6 | Full System Run: insufficient stock → fails gracefully; valid order → robot assigned, path generated, item picked, stock deducted, return path executed | ✅ End-to-end success |
| 7 | Robot Navigation: manual steps (Forward/Backward/Turn Left/Turn Right), display forward path, execute reverse return, display log | ✅ All correct |
| 8 | Invalid text input at any menu (non-numeric) | ✅ "Invalid choice" printed, no loop |
| 9 | CSV Persistence: items.csv quantity deducted after orders, orders.csv appended, robots.csv taskCount updated | ✅ All files updated correctly |

---

## Verification of System Requirements

| Requirement | Status |
|-------------|--------|
| Manage incoming orders (FIFO queue, max 100) | ✅ Enforced with capacity check |
| Unique Order IDs across active queue and history | ✅ Enforced via `isOrderIDExists()` |
| Robot assignment (round-robin) | ✅ Working with `assignRobotProgrammatically()` |
| BFS pathfinding through warehouse tree | ✅ Verified for all zones/aisles/shelves |
| LIFO stack-based reverse return path | ✅ Verified with cardinal direction reversal |
| Item stock deduction after retrieval | ✅ Verified with CSV save |
| CSV persistence for items, robots, and orders | ✅ All three files load/save correctly |
| Custom data structures only (no STL) | ✅ Confirmed: custom Stack, Queue, BST, Tree used |
