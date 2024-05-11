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
  - *Fixed:* the `head_pos` and `root_pos` aren't correctly initialized.

### 2024/5/2: `Insert`, `Delete`
- **Test** `Find`:
  - *Fixed:* the `BinarySearchFirstBigger`, `BinarySearchLastSmaller` cannot meet the objectives when there is more than one identical data.
- ***(Notice)*** In `Delete` the key in the node should be changed when the first element of the block is changed, and the node may not be the direct leaf node but someone above;
- **Work on** `Delete`:
  - ***(Change ideas)*** nodes can only adopt children from and merge with the nodes with the same parent.

### 2024/5/5: `Delete`
- **Work on** `Delete`:
  - *Finish* `LeafPreAdopt`, `LeafNextAdopt`, `LeafPreMerge`, `LeafNextMerge`;
  - ***(Change ideas)*** the adoption and merge of non-leaf nodes is not the same as leaf nodes;
  - **[Checked]** ***(To be improved)*** we can directly save the ancestor nodes instead of their addresses to reduce file reading times, but remember some node will be changed when `eleme_pos == 0`; 
  - ***(To be improved)*** we can equally divide the two blocks instead of adopt just one child when adopting.

### 2024/5/6: `Delete`
- **Work on** `Delete`:
  - **[Checked]** ***(To be improved)*** we can save the position of the node that includes the target key so that we needn't find the target key again in `UpdateKey`;
  - *Finish* `NodePreAdopt`, `NodeNextAdopt`, `NodePreMerge`, `NodeNextMerge`;
  - *Finish* `DeleteAdjust`.

### 2024/5/7: Test
- **Test** `Insert` and `Find`:
  - *Fixed*: fix the problem about appending data to the end of the file;
- **Test** `Delete`:
  - *Fixed*: the size of merged nodes is not correctly updated (one key less);
  - *Fixed*: `if_next` is not reset in `DeleteAdjust`;
- **Test** `baihua::vector`:
  - Temporarily use `std::vector` as an alternative.

### 2024/5/8: Test
- *Fixed*: problem in inputting the data;
- *Fixed*: problem in breaking leaf nodes, now changed into just inserting and then breaking;
- *Fixed*: the direction of moving the data in the nodes is converse in the function `InsertAdjust`;
- *Fixed*: `head_pos` is not updated when deleting the last element and then deleting the last block;
- *Fixed*: `node.size` and `next_node.size` aren't updated in `NodeNextAdopt`;
- *Fixed*: father nodes are not updated in `NodePreAdopt` and `NodeNextAdopt`, as they directly return in `DeleteAdjust`;
- *Fixed*: some naive mistake in `NodeNextAdopt`, `father_node.second` instead of `father_node.second - 1`;
- *Fixed*: there is misalignment when displacing data in `NodePreAdopt`.
- *Fixed*: mistake `L` for `M`;
- ***Congratulations!*** Pass all the test points!

### 2024/5/9: Optimisation
- Change the size of nodes and blocks;
- Add `Hash` for `std::string`;

### 2024/5/11: Optimisation
- Merge the block of data into leaf, i.e., record `Leaf` and `data` in just one `struct Leaf`;
- Remove the records of `Node::pre`, `Node::next`, `Leaf::pre`;