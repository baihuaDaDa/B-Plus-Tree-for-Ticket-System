Developer Log
===

### 2024/4/24: Start
- Review my `memory.h` in Bookstore-2023 and come up with the initial framework of my B-Plus-Tree;
- **Add:**
  - `utility.hpp`*(pair)*,
  - `exception.hpp`,
  - `vector.hpp`,
  - `constantLengthString`*(string with length limit)*,
  - `Database.hpp`*(memory reading and writing class)*;
- ***(To be improved)*** Find something to improve in my `vector.hpp`. That is, deallocating as soon as the size equals to half of the storage has low efficiency. However, that doesn't matter here.
- **Add** `test.cpp` and some sample data for test.

### 2024/4/25: `BinarySearch`, `BptFind`, and the function `Find`
- ***(To be improved)*** Maybe we can use `hash_table` to record strings;
- ***(Change ideas)*** Decide to use `Node` and `Leaf` to record non-leaf nodes and leaf nodes separately;
- **Finish** `search` part, including `BinarySearchLastSmaller`, `BinarySearchFirstBigger`, `FindElement`, `FindIndex`;
- **Finish** the function `Find`;
- **Add** `SingleAppend` and `BlockAppend` functions to the class `Database`.

### 2024/4/26: `Insert` and `Delete`
- ***(To be improved)*** Is it necessary to try to place a child in the "care" of the neighbour node when the present node has too many children in the function `Insert`?
- ***(Change ideas)*** Avoid recording the father's address in every node and leaf; instead, use a stack to temporarily record fathers we have visited when initially searching the target key.

### 2024/4/27: `main.cpp`
- **Finish** the main function.

### 2024/4/28: `Insert`
- **Finish** `Insert`
- **Test** `Insert`: 
  - *Fixed:* the first leaf isn't correctly created;
  - *Fixed:* the operator `>>` isn't correctly overloaded;
  - *Fixed:* the `head_pos` and `root_pos` aren't correctly initialized;

### 2024/5/2: `Insert`, `Delete`
- **Test** `Find`
  - *Fixed:* the `BinarySearchFirstBigger`, `BinarySearchLastSmaller` cannot meet the objectives when there is more than one identical data;
- 