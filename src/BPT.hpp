#ifndef B_PLUS_TREE_BPT_HPP
#define B_PLUS_TREE_BPT_HPP

#include "Database.hpp"
#include "../STLite-baihua/utility.hpp"
#include "../STLite-baihua/vector.hpp"

namespace baihua {

    template<class Index, class Value, int (*CmpIndex)(const Index &, const Index &), int (*CmpValue)(const Value &,
                                                                                                      const Value &)>
    class BPT {
    public:
        using value_type = pair<Index, Value, CmpIndex, CmpValue>;

    private:
        static constexpr int M = 5;
        static constexpr int L = 5;

        struct Node {
            // one more for buffering
            value_type key[M] = {}; // 0-base
            int son[M + 1] = {}; // 0-base, index instead of address (when upon the leaf, index in memory_leaf)
            int size = 0; // the number of the keys
            int father = -1;
            bool is_upon_leaf = false;

            Node() = default;

        };

        struct Leaf {
            int address = -1; // block_num
            int father = -1; // index in memory_node
            int index = -1; // index in its father's sons
            int size = 0;
            int next = -1; // block_num

            Leaf() = default;

            Leaf(int _address, int _father, int _index, int _size, int _next)
                    : address(_address), father(_father), index(_index), size(_size), next(_next) {}

        };

        int root_pos;
        int head_pos;
        Database<Node, 1, 1> memory_node; // record the position of the root, if none, return -1.
        Database<Leaf, 1, 1> memory_leaf; // record the position of the head node, if none, return -1.
        Database<value_type, L> database;

    public:
        BPT(const string &filename) {
            string node_filename = filename + "_BptNode";
            string leaf_filename = filename + "_BptLeafNode";
            if (!memory_node.isFileExist()) {
                memory_node.initialize(node_filename);
            }
            if (!memory_leaf.isFileExist()) {
                memory_leaf.initialize(leaf_filename);
            }
            if (!database.isFileExist()) {
                database.initialize(filename);
            } // Check if the file exist. If not, then initialise it.
            memory_node.ReadInfo(root_pos, 1);
            memory_leaf.ReadInfo(head_pos, 1);
        }

        /* Several cases:
         * - No.1 simply insert;
         * - No.2 already exist;
         * - No.3 insert and break the block;
         * - No.4 insert, break the block and break BPT-node, and maybe more till the root.
         **/
        void Insert(const Index &index, const Value &value) {
            value_type element{index, value};
            int leaf_pos = FindElement(element);
            Leaf leaf;
            memory_leaf.SingleRead(leaf, leaf_pos);
            value_type data[L];
            database.MultiRead(data, leaf.address);
            int elem_pos = BinarySearchPair(element, data, leaf.size);
            if (CmpPair(element, data[elem_pos]) == 0) return; // Case 2
            if (leaf.size < M) { // Case 1
                for (int i = elem_pos + 1; i < leaf.size; ++i)
                    data[i + 1] = data[i];
                data[elem_pos + 1] = element;
                ++leaf.size;
                database.MultiUpdate(data, leaf.address);
                memory_leaf.SingleUpdate(leaf, leaf_pos);
            } else { // Case 3
                value_type new_block_data[L] = {};
                Leaf new_leaf;
            }
        }

        void Delete(const Index &index, const Value &value) {

        }

        vector<value_type> Find(const Index &index) {
            vector<value_type> result;
            int leaf_pos = FindIndex(index);
            if (leaf_pos == -1) return std::move(result);
            Leaf leaf;
            value_type data[L];
            bool flag = false;
            while (leaf_pos != -1) {
                memory_leaf.SingleRead(leaf, leaf_pos);
                database.MultiRead(data, leaf.address);
                int index_pos = BinarySearchIndex(index, data, leaf.size);
                if (index_pos == leaf.size) {
                    if (flag) return std::move(result);
                    else flag = true;
                } else {
                    for (int i = index_pos; i < leaf.size; ++i) {
                        if (CmpIndex(data[i].first, index) == 0)
                            result.push_back(data[i]);
                        else return std::move(result);
                    }
                }
                leaf_pos = leaf.next;
            }
        }

    private:
        // Find the block that possibly contains the element.
        // the last key that is smaller than or equal to the element
        int BinarySearchPair(const value_type &element, const value_type *data, const int size) {
            int l = 0, r = size - 1, mid;
            while (l <= r) {
                mid = ((l + r) >> 1);
                int flag = CmpPair(data[mid], element);
                if (flag == -1) {
                    l = mid + 1;
                } else if (flag == 1) {
                    r = mid - 1;
                } else {
                    return mid;
                }
            }
            return r;
        }

        // Find the block that possibly contains the smallest element with the index to be found.
        // the first index that is bigger or equal to the index
        int BinarySearchIndex(const Index &index, const value_type *data, const int size) {
            int l = 0, r = size - 1, mid;
            while (l <= r) {
                mid = ((l + r) >> 1);
                int flag = CmpIndex(data[mid].first, index);
                if (flag == -1) {
                    l = mid + 1;
                } else if (flag == 1) {
                    r = mid - 1;
                } else {
                    return mid;
                }
            }
            return l;
        }

        int FindElement(const value_type &element) {
            Node node;
            if (root_pos == -1) return head_pos;
            memory_node.SingleRead(node, root_pos);
            while (true) {
                int result = BinarySearchPair(element, node.key, node.size);
                if (node.is_upon_leaf) return node.son[result + 1];
                else memory_node.SingleRead(node, node.son[result + 1]);
            }
        }

        int FindIndex(const Index &index) {
            Node node;
            if (root_pos == -1) return head_pos;
            memory_node.SingleRead(node, root_pos);
            while (true) {
                int result = BinarySearchIndex(index, node.key, node.size);
                if (node.is_upon_leaf) return node.son[result];
                else memory_node.SingleRead(node, node.son[result]);
            }
        }

    };

}

#endif //B_PLUS_TREE_BPT_HPP
