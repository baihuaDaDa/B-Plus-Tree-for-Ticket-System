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
            bool is_upon_leaf = false;
            int pre;
            int next;

            Node() = default;

            Node(int _size, bool _is_upon_leaf, int _pre, int _next)
                    : size(_size), is_upon_leaf(_is_upon_leaf), pre(_pre), next(_next) {}

        };

        struct Leaf {
            int address = -1; // block_num
            int size = 0;
            int next = -1; // block_num
            int pre = -1; // block_num

            Leaf() = default;

            Leaf(int _size, int _pre, int _next) : size(_size), pre(_pre), next(_next) {}

        };

        int root_pos;
        int head_pos;
        vector<pair<int, int>> father; // <position, index>
        Database<Node, 1, 1> memory_node; // record the position of the root, if none, return -1.
        Database<Leaf, 1, 1> memory_leaf; // record the position of the head node, if none, return -1.
        Database<value_type, L> database;

    public:
        BPT(const string &filename) : memory_node(filename + "_BptNode.bin"),
                                      memory_leaf(filename + "_BptLeafNode.bin"), database(filename + ".bin") {

            // Check if the file exist. If not, then initialise it.
            if (!memory_node.isFileExist()) memory_node.initialize(-1);
            if (!memory_leaf.isFileExist()) memory_leaf.initialize(-1);
            if (!database.isFileExist()) database.initialize();
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
            if (leaf_pos == -1) {
                Leaf new_leaf{1, -1, -1};
                value_type data[L];
                data[0] = element;
                new_leaf.address = head_pos = database.BlockAppend(data);
                memory_leaf.WriteInfo(head_pos, 1);
                memory_leaf.SingleAppend(new_leaf);
                return;
            }
            Leaf leaf;
            memory_leaf.SingleRead(leaf, leaf_pos);
            value_type data[L];
            database.BlockRead(data, leaf.address);
            int elem_pos = BinarySearchLastSmaller(element, data, leaf.size);
            if (elem_pos >= 0 && CmpPair(element, data[elem_pos]) == 0) return; // Case 2
            if (leaf.size < L) { // Case 1
                for (int i = leaf.size - 1; i >= elem_pos + 1; --i)
                    data[i + 1] = data[i];
                ++leaf.size;
                data[elem_pos + 1] = element;
                database.BlockUpdate(data, leaf.address);
                memory_leaf.SingleUpdate(leaf, leaf_pos);
            } else { // Case 3
                value_type new_block_data[L] = {};
                leaf.size = ((L + 1) >> 1);
                Leaf new_leaf{L + 1 - leaf.size, leaf_pos, leaf.next};
                if (elem_pos + 1 < ((L + 1) >> 1)) {
                    for (int i = leaf.size - 2; i >= elem_pos + 1; --i)
                        data[i + 1] = data[i];
                    data[elem_pos + 1] = element;
                    for (int i = leaf.size - 1, j = 0; i < L; ++i, ++j)
                        new_block_data[j] = data[i];
                } else {
                    int j = 0;
                    for (int i = leaf.size; i <= elem_pos; ++i, ++j)
                        new_block_data[j] = data[i];
                    new_block_data[j++] = element;
                    for (int i = elem_pos + 1; i < L; ++i, ++j)
                        new_block_data[j] = data[i];
                }
                new_leaf.address = database.BlockAppend(new_block_data);
                database.BlockUpdate(data, leaf.address);
                leaf.next = memory_leaf.SingleAppend(new_leaf);
                memory_leaf.SingleUpdate(leaf, leaf_pos);
                if (new_leaf.next != -1) {
                    Leaf next_leaf;
                    memory_leaf.SingleRead(next_leaf, new_leaf.next);
                    next_leaf.pre = leaf.next;
                    memory_leaf.SingleUpdate(new_leaf, new_leaf.next);
                }
                InsertAdjust(leaf_pos, leaf.next, new_block_data[0]); // Case 4
            }
        }

        /* Several cases:
         * - No.1 nothing to be deleted;
         * - No.2 simply delete;
         * - No.3 delete and adopt one child from neighbours;
         * - No.4 delete and merge with one neighbour (usually the one on the right);
         * - No.5 delete, merge and continue to adopt or merge, maybe till the root.
         **/
        void Delete(const Index &index, const Value &value) {
            value_type element{index, value};
            int leaf_pos = FindElement(element);
            Leaf leaf;
            memory_leaf.SingleRead(leaf, leaf_pos);
            value_type data[L];
            database.BlockRead(data, leaf.address);
            int elem_pos = BinarySearchLastSmaller(element, data, leaf.size);
            if (elem_pos < 0 || CmpPair(element, data[elem_pos]) != 0) return; // Case 1
            --leaf.size;
            for (int i = elem_pos; i < leaf.size; ++i)
                data[i] = data[i + 1];
            if (leaf.size >= ((L + 1) >> 1)) { // Case 2
                database.BlockUpdate(data, leaf.address);
                memory_leaf.SingleUpdate(leaf, leaf_pos);
            } else {
                // Case 3
                if (leaf.next != -1) {
                    Leaf next_leaf;
                    memory_leaf.SingleRead(next_leaf, leaf.next);
                    if (next_leaf.size > (L + 1) >> 1) {
                        value_type next_data[L];
                        database.BlockRead(next_data, next_leaf.address);
                        data[leaf.size++] = next_data[0];
                        UpdateKey(next_data[0], next_data[1]);
                        --next_leaf.size;
                        for (int i = 0; i < next_leaf.size; ++i)
                            next_data[i] = next_data[i + 1];
                        database.BlockUpdate(data, leaf.address);
                        memory_leaf.SingleUpdate(leaf, leaf_pos);
                        database.BlockUpdate(next_data, next_leaf.address);
                        memory_leaf.SingleUpdate(next_leaf, leaf.next);
                    }
                } else if (leaf.pre != -1) {
                    Leaf pre_leaf;
                    memory_leaf.SingleRead(pre_leaf,leaf.pre);
                } else { // Case 4, 5

                }
            }
        }

        vector<Value> Find(const Index &index) {
            vector<Value> result;
            int leaf_pos = FindIndex(index);
            if (leaf_pos == -1) return std::move(result);
            Leaf leaf;
            value_type data[L];
            bool flag = false;
            while (leaf_pos != -1) {
                memory_leaf.SingleRead(leaf, leaf_pos);
                database.BlockRead(data, leaf.address);
                int index_pos = BinarySearchFirstBigger(index, data, leaf.size);
                if (index_pos == leaf.size) {
                    if (flag) return std::move(result);
                    else flag = true;
                } else {
                    for (int i = index_pos; i < leaf.size; ++i) {
                        if (CmpIndex(data[i].first, index) == 0)
                            result.push_back(data[i].second);
                        else return std::move(result);
                    }
                }
                leaf_pos = leaf.next;
            }
            return std::move(result);
        }

    private:
        // Find the block that possibly contains the element.
        // the last key that is smaller than or equal to the element
        int BinarySearchLastSmaller(const value_type &element, const value_type *data, const int size) {
            int l = 0, r = size - 1, mid;
            while (l <= r) {
                mid = ((l + r) >> 1);
                int flag = CmpPair(data[mid], element);
                if (flag == -1) {
                    l = mid + 1;
                } else if (flag == 1) {
                    r = mid - 1;
                } else {
                    while (mid < size - 1 && CmpPair(data[mid + 1], element) != 1) ++mid;
                    return mid;
                }
            }
            return r;
        }

        // Find the block that possibly contains the smallest element with the index to be found.
        // the first index that is bigger or equal to the index
        int BinarySearchFirstBigger(const Index &index, const value_type *data, const int size) {
            int l = 0, r = size - 1, mid;
            while (l <= r) {
                mid = ((l + r) >> 1);
                int flag = CmpIndex(data[mid].first, index);
                if (flag == -1) {
                    l = mid + 1;
                } else if (flag == 1) {
                    r = mid - 1;
                } else {
                    while (mid > 0 && CmpIndex(data[mid - 1].first, index) != -1) --mid;
                    return mid;
                }
            }
            return l;
        }

        int FindElement(const value_type &element) {
            father.clear();
            Node node;
            if (root_pos == -1) return head_pos;
            int r_pos = root_pos;
            memory_node.SingleRead(node, r_pos);
            while (true) {
                int result = BinarySearchLastSmaller(element, node.key, node.size);
                if (node.is_upon_leaf) {
                    father.push_back(pair<int, int>(r_pos, result + 1));
                    return node.son[result + 1];
                } else {
                    father.push_back(pair<int, int>(r_pos, result + 1));
                    r_pos = node.son[result + 1];
                    memory_node.SingleRead(node, r_pos);
                }
            }
        }

        int FindIndex(const Index &index) {
            Node node;
            if (root_pos == -1) return head_pos;
            memory_node.SingleRead(node, root_pos);
            while (true) {
                int result = BinarySearchFirstBigger(index, node.key, node.size);
                if (node.is_upon_leaf) return node.son[result];
                else memory_node.SingleRead(node, node.son[result]);
            }
        }

        void InsertAdjust(int son_pos, int new_son_pos, value_type key) {
            Node node;
            pair<int, int> father_node{-1, -1};
            bool flag = false;
            while (!father.empty()) {
                father_node = father.back();
                father.pop_back();
                memory_node.SingleRead(node, father_node.first);
                for (int i = father_node.second + 1; i <= node.size; ++i) {
                    node.son[i + 1] = node.son[i];
                    node.key[i] = node.key[i - 1];
                }
                node.son[father_node.second + 1] = new_son_pos;
                node.key[father_node.second] = key;
                ++node.size;
                if (node.size == M) {
                    Node new_node{M >> 1, node.is_upon_leaf, father_node.first, node.next};
                    node.size = M - new_node.size - 1;
                    for (int i = node.size + 1, j = 0; i < M; ++i, ++j) {
                        new_node.key[j] = node.key[i];
                        new_node.son[j] = node.son[i];
                    }
                    new_node.son[new_node.size] = node.son[M];
                    key = node.key[node.size];
                    son_pos = father_node.first;
                    new_son_pos = memory_node.SingleAppend(new_node);
                    node.next = new_son_pos;
                    memory_node.SingleUpdate(node, father_node.first);
                    if (new_node.next != -1) {
                        Node next_node;
                        memory_node.SingleRead(next_node, new_node.next);
                        next_node.pre = node.next;
                        memory_node.SingleUpdate(next_node, new_node.next);
                    }
                } else {
                    memory_node.SingleUpdate(node, father_node.first);
                    flag = true;
                    break;
                }
            }
            if (flag) return;
            Node new_root{1, (father_node.first == -1), -1, -1};
            new_root.key[0] = key;
            new_root.son[0] = son_pos;
            new_root.son[1] = new_son_pos;
            root_pos = memory_node.SingleAppend(new_root);
            memory_node.WriteInfo(root_pos, 1);
        }

        void UpdateKey(const value_type &original_key, const value_type &new_key) {

        }

        void DeleteAdjust() {
        }

    };

}

#endif //B_PLUS_TREE_BPT_HPP
