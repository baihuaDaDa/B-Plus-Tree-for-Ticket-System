Developer Log
===

### 2024/4/24: Start
- Review my `memory.h` in Bookstore-2023 and come up with the initial framework of my B-Plus-Tree;
- Add:
  - `utility.hpp`(pair),
  - `exception.hpp`,
  - `vector.hpp`,
  - `constantLengthString`(string with length limit),
  - `Database.hpp`(memory reading and writing class);
- Find something to improve in my `vector.hpp`. That is, deallocating as soon as the size equals to half of the storage has low efficiency. However, that doesn't matter here.
- Add `test.cpp` and some sample data for test.

### 2024/4/25: 
- Maybe we can use `hash_table` to record strings;
- (Finally still) Decide to use `Node` and `Leaf` to record non-leaf nodes and leaf nodes separately;
- Finish `search` part, including `BinarySearchPair`, `BinarySearchIndex`, `FindElement`, `FindIndex`;
