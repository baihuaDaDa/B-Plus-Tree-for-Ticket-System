#ifndef B_PLUS_TREE_BPT_HPP
#define B_PLUS_TREE_BPT_HPP

#include "Database.hpp"
#include "../STLite-baihua/utility.hpp"
//#include "../STLite-baihua/vector.hpp"
#include <vector>

namespace baihua {

    using std::vector;

    template<class Index, class Value, int (*CmpIndex)(const Index &, const Index &), int (*CmpValue)(const Value &,
                                                                                                      const Value &)>
    class BPT {
    public:
        using value_type = pair<Index, Value, CmpIndex, CmpValue>;

    private:
        static constexpr int M = 250;
        static constexpr int L = 1270;

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
        vector<Node> father; // father_node
        int target_index = -1; // the position of the node that includes the target key
        vector<pair<int, int>> father_info; // <position, index>
        Database<Node, 1, 1> memory_node; // record the position of the root, if none, return -1.
        Database<Leaf, 1, 1> memory_leaf; // record the position of the head node, if none, return -1.
        Database<value_type, L + 1> database;

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
                value_type data[L + 1];
                data[0] = element;
                new_leaf.address = head_pos = database.BlockAppend(data);
                memory_leaf.WriteInfo(head_pos, 1);
                memory_leaf.SingleAppend(new_leaf);
                return;
            }
            Leaf leaf;
            memory_leaf.SingleRead(leaf, leaf_pos);
            value_type data[L + 1];
            database.BlockRead(data, leaf.address);
            int elem_pos = BinarySearchLastSmaller(element, data, leaf.size);
            if (elem_pos >= 0 && CmpPair(element, data[elem_pos]) == 0) return; // Case 2
            for (int i = leaf.size - 1; i >= elem_pos + 1; --i)
                data[i + 1] = data[i];
            ++leaf.size;
            data[elem_pos + 1] = element;
            if (leaf.size <= L) { // Case 1
                database.BlockUpdate(data, leaf.address);
                memory_leaf.SingleUpdate(leaf, leaf_pos);
            } else { // Case 3
                value_type new_block_data[L + 1] = {};
                leaf.size = ((L + 1) >> 1);
                Leaf new_leaf{L + 1 - leaf.size, leaf_pos, leaf.next};
                for (int i = leaf.size, j = 0; i < L + 1; ++i, ++j)
                    new_block_data[j] = data[i];
                new_leaf.address = database.BlockAppend(new_block_data);
                database.BlockUpdate(data, leaf.address);
                leaf.next = memory_leaf.SingleAppend(new_leaf);
                memory_leaf.SingleUpdate(leaf, leaf_pos);
                if (new_leaf.next != -1) {
                    Leaf next_leaf;
                    memory_leaf.SingleRead(next_leaf, new_leaf.next);
                    next_leaf.pre = leaf.next;
                    memory_leaf.SingleUpdate(next_leaf, new_leaf.next);
                }
                // Case 4
                InsertAdjust(leaf_pos, leaf.next, new_block_data[0]);
            }
        }

        /* Several cases:
         * - No.1 nothing to be deleted;
         * - No.2 simply delete;
         * - No.3 delete and the block is too small, but there is no block to adopt children from or merge;
         * - No.4 delete and adopt one child from neighbours;
         * - No.5 delete and merge with one neighbour (usually the one on the right);
         * - No.6 delete, merge and continue to adopt or merge, maybe till the root.
         **/
        void Delete(const Index &index, const Value &value) {
            value_type element{index, value};
            int leaf_pos = FindElement(element);
            Leaf leaf;
            memory_leaf.SingleRead(leaf, leaf_pos);
            value_type data[L + 1];
            database.BlockRead(data, leaf.address);
            int elem_pos = BinarySearchLastSmaller(element, data, leaf.size);
            if (elem_pos < 0 || CmpPair(element, data[elem_pos]) != 0) return; // Case 1
            --leaf.size;
            if (elem_pos == 0) UpdateKey(data[1]);
            for (int i = elem_pos; i < leaf.size; ++i)
                data[i] = data[i + 1];
            if (leaf.size >= ((L + 1) >> 1)) { // Case 2
                database.BlockUpdate(data, leaf.address);
                memory_leaf.SingleUpdate(leaf, leaf_pos);
            } else {
                // Case 3
                if (leaf.pre == -1 && leaf.next == -1) {
                    if (leaf.size == 0) {
                        head_pos = -1;
                        memory_leaf.WriteInfo(head_pos, 1);
                    } else {
                        database.BlockUpdate(data, leaf.address);
                        memory_leaf.SingleUpdate(leaf, leaf_pos);
                    }
                    return;
                }
                // Case 4
                bool if_pre = true, if_next = true;
                if (LeafPreAdopt(leaf, leaf_pos, data, if_pre)) return;
                if (LeafNextAdopt(leaf, leaf_pos, data, if_next)) return;
                // Case 5
                if (if_next) LeafNextMerge(leaf, leaf_pos, data);
                else LeafPreMerge(leaf, leaf_pos, data);
                // Case 6
                DeleteAdjust(if_next);
            }
        }

        vector<Value> Find(const Index &index) {
            vector<Value> result;
            int leaf_pos = FindIndex(index);
            if (leaf_pos == -1) return std::move(result);
            Leaf leaf;
            value_type data[L + 1];
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
            target_index = -1;
            father.clear();
            father_info.clear();
            Node node;
            if (root_pos == -1) return head_pos;
            int r_pos = root_pos;
            memory_node.SingleRead(node, r_pos);
            while (true) {
                int result = BinarySearchLastSmaller(element, node.key, node.size);
                if (target_index == -1 && result >= 0 && CmpPair(node.key[result], element) == 0)
                    target_index = father.size();
                if (node.is_upon_leaf) {
                    father_info.push_back(pair<int, int>(r_pos, result + 1));
                    father.push_back(node);
                    return node.son[result + 1];
                } else {
                    father_info.push_back(pair<int, int>(r_pos, result + 1));
                    father.push_back(node);
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
            while (!father_info.empty()) {
                father_node = father_info.back();
                node = father.back();
                father_info.pop_back();
                father.pop_back();
                for (int i = node.size; i > father_node.second; --i) {
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

        // When changing the first element of the block, we need to change the corresponding key in the tree.
        // @if_already is to tell the function whether the ancestors of the element are already pushed into the stack.
        void UpdateKey(const value_type &new_key) {
            if (target_index == -1) return;
            Node &node = father[target_index];
            pair<int, int> father_node = father_info[target_index];
            node.key[father_node.second - 1] = new_key;
            memory_node.SingleUpdate(node, father_node.first);
        }

        // Adopt children from the appropriate neighbour blocks, if not, return false.
        bool LeafPreAdopt(Leaf &leaf, const int leaf_pos, value_type *data, bool &if_pre) {
            if (leaf.pre == -1) return if_pre = false;
            pair<int, int> father_node = father_info.back();
            if (father_node.second == 0) return if_pre = false;
            Leaf pre_leaf;
            memory_leaf.SingleRead(pre_leaf, leaf.pre);
            if (pre_leaf.size > (L + 1) >> 1) {
                value_type pre_data[L + 1];
                database.BlockRead(pre_data, pre_leaf.address);
                --pre_leaf.size;
                memory_leaf.SingleUpdate(pre_leaf, leaf.pre);
                ++leaf.size;
                for (int i = leaf.size - 1; i >= 1; --i)
                    data[i] = data[i - 1];
                data[0] = pre_data[pre_leaf.size];
                Node &_father = father[father.size() - 1];
                _father.key[father_node.second - 1] = data[0];
                database.BlockUpdate(data, leaf.address);
                memory_leaf.SingleUpdate(leaf, leaf_pos);
                memory_node.SingleUpdate(_father, father_node.first);
                return true;
            } else return false;
        }

        bool LeafNextAdopt(Leaf &leaf, const int leaf_pos, value_type *data, bool &if_next) {
            if (leaf.next == -1) return if_next = false;
            pair<int, int> father_node = father_info.back();
            Node &_father = father[father.size() - 1];
            if (father_node.second == _father.size) return if_next = false;
            Leaf next_leaf;
            memory_leaf.SingleRead(next_leaf, leaf.next);
            if (next_leaf.size > (L + 1) >> 1) {
                value_type next_data[L + 1];
                database.BlockRead(next_data, next_leaf.address);
                data[leaf.size++] = next_data[0];
                _father.key[father_node.second] = next_data[1];
                --next_leaf.size;
                for (int i = 0; i < next_leaf.size; ++i)
                    next_data[i] = next_data[i + 1];
                database.BlockUpdate(data, leaf.address);
                memory_leaf.SingleUpdate(leaf, leaf_pos);
                database.BlockUpdate(next_data, next_leaf.address);
                memory_leaf.SingleUpdate(next_leaf, leaf.next);
                memory_node.SingleUpdate(_father, father_node.first);
                return true;
            } else return false;
        }

        /* Merge two half blocks.
         * Pay attention to merge the next node into the previous node
         * so that we needn't update @head_pos in the file.
         **/
        void LeafPreMerge(Leaf &leaf, const int leaf_pos, value_type *data) {
            Leaf pre_leaf;
            memory_leaf.SingleRead(pre_leaf, leaf.pre);
            value_type pre_data[L + 1];
            database.BlockRead(pre_data, pre_leaf.address);
            for (int i = 0, j = pre_leaf.size; i < leaf.size; ++i, ++j)
                pre_data[j] = data[i];
            pre_leaf.size += leaf.size;
            pre_leaf.next = leaf.next;
            if (leaf.next != -1) {
                Leaf next_leaf;
                memory_leaf.SingleRead(next_leaf, leaf.next);
                next_leaf.pre = leaf.pre;
                memory_leaf.SingleUpdate(next_leaf, leaf.next);
            }
            memory_leaf.SingleUpdate(pre_leaf, leaf.pre);
            database.BlockUpdate(pre_data, pre_leaf.address);
        }

        void LeafNextMerge(Leaf &leaf, const int leaf_pos, value_type *data) {
            Leaf next_leaf;
            memory_leaf.SingleRead(next_leaf, leaf.next);
            value_type next_data[L + 1];
            database.BlockRead(next_data, next_leaf.address);
            for (int i = leaf.size, j = 0; j < next_leaf.size; ++i, ++j)
                data[i] = next_data[j];
            leaf.size += next_leaf.size;
            leaf.next = next_leaf.next;
            if (leaf.next != -1) {
                Leaf _next_leaf;
                memory_leaf.SingleRead(_next_leaf, leaf.next);
                _next_leaf.pre = leaf_pos;
                memory_leaf.SingleUpdate(_next_leaf, leaf.next);
            }
            memory_leaf.SingleUpdate(leaf, leaf_pos);
            database.BlockUpdate(data, leaf.address);
        }

        // Adopt keys from the appropriate neighbour nodes, if not, return false.
        bool NodePreAdopt(Node &node, const int node_pos, bool &if_pre) {
            if (node.pre == -1) return if_pre = false;
            Node &_father = father[father.size() - 1];
            pair<int, int> father_node = father_info.back();
            if (father_node.second == 0) return if_pre = false;
            Node pre_node;
            memory_node.SingleRead(pre_node, node.pre);
            if (pre_node.size + 1 > (M + 1) >> 1) {
                for (int i = node.size; i >= 0; --i) {
                    node.son[i + 1] = node.son[i];
                    if (i > 0) node.key[i] = node.key[i - 1];
                }
                node.son[0] = pre_node.son[pre_node.size];
                node.key[0] = _father.key[father_node.second - 1];
                _father.key[father_node.second - 1] = pre_node.key[pre_node.size - 1];
                --pre_node.size;
                ++node.size;
                memory_node.SingleUpdate(pre_node, node.pre);
                memory_node.SingleUpdate(node, node_pos);
                memory_node.SingleUpdate(_father, father_node.first);
                return true;
            } else return false;
        }

        bool NodeNextAdopt(Node &node, const int node_pos, bool &if_next) {
            if (node.next == -1) return if_next = false;
            Node &_father = father[father.size() - 1];
            pair<int, int> father_node = father_info.back();
            if (father_node.second == _father.size) return if_next = false;
            Node next_node;
            memory_node.SingleRead(next_node, node.next);
            if (next_node.size + 1 > (M + 1) >> 1) {
                node.key[node.size] = _father.key[father_node.second];
                node.son[node.size + 1] = next_node.son[0];
                _father.key[father_node.second] = next_node.key[0];
                for (int i = 0; i < next_node.size; ++i) {
                    next_node.son[i] = next_node.son[i + 1];
                    if (i < next_node.size - 1) next_node.key[i] = next_node.key[i + 1];
                }
                ++node.size;
                --next_node.size;
                memory_node.SingleUpdate(node, node_pos);
                memory_node.SingleUpdate(next_node, node.next);
                memory_node.SingleUpdate(_father, father_node.first);
                return true;
            } else return false;
        }


        /* Merge two half nodes.
         * Pay attention to merge the present node into the next or the previous node
         * so that we can easily find the ancestors of the erased node to modify the corresponding key.
         **/
        void NodePreMerge(Node &node, const int node_pos) {
            Node pre_node;
            memory_node.SingleRead(pre_node, node.pre);
            Node &_father = father[father.size() - 1];
            pair<int, int> father_node = father_info.back();
            for (int i = 0, j = pre_node.size + 1; i <= node.size; ++i, ++j) {
                pre_node.son[j] = node.son[i];
                if (i < node.size) pre_node.key[j] = node.key[i];
            }
            pre_node.key[pre_node.size] = _father.key[father_node.second - 1];
            pre_node.size += node.size + 1;
            pre_node.next = node.next;
            if (node.next != -1) {
                Node next_node;
                memory_node.SingleRead(next_node, node.next);
                next_node.pre = node.pre;
                memory_node.SingleUpdate(next_node, node.next);
            }
            memory_node.SingleUpdate(pre_node, node.pre);
        }

        void NodeNextMerge(Node &node, const int node_pos) {
            Node next_node;
            memory_node.SingleRead(next_node, node.next);
            Node &_father = father[father.size() - 1];
            pair<int, int> father_node = father_info.back();
            for (int i = node.size + 1, j = 0; j <= next_node.size; ++i, ++j) {
                node.son[i] = next_node.son[j];
                if (j < next_node.size) node.key[i] = next_node.key[j];
            }
            node.key[node.size] = _father.key[father_node.second];
            node.size += next_node.size + 1;
            node.next = next_node.next;
            if (node.next != -1) {
                Node _next_node;
                memory_node.SingleRead(_next_node, node.next);
                _next_node.pre = node_pos;
                memory_node.SingleUpdate(_next_node, node.next);
            }
            memory_node.SingleUpdate(node, node_pos);
        }

        void DeleteAdjust(bool if_next) {
            bool if_pre = true, flag = false;
            while (father_info.size() > 1) {
                int father_pos = father_info.back().first;
                int father_index = father_info.back().second + if_next;
                if_pre = if_next = true;
                father_info.pop_back();
                Node node = father[father.size() - 1];
                father.pop_back();
                for (int i = father_index; i < node.size; ++i) {
                    node.son[i] = node.son[i + 1];
                    node.key[i - 1] = node.key[i];
                }
                --node.size;
                if (node.size + 1 < (M + 1) >> 1) {
                    if (NodePreAdopt(node, father_pos, if_pre)) return;
                    if (NodeNextAdopt(node, father_pos, if_next)) return;
                    if (if_next) NodeNextMerge(node, father_pos);
                    else NodePreMerge(node, father_pos);
                } else {
                    memory_node.SingleUpdate(node, father_pos);
                    flag = true;
                    break;
                }
            }
            if (flag) return;
            int root_index = father_info.back().second + if_next;
            Node &root = father[father.size() - 1];
            for (int i = root_index; i < root.size; ++i) {
                root.son[i] = root.son[i + 1];
                root.key[i - 1] = root.key[i];
            }
            --root.size;
            if (root.size == 0) {
                if (root.is_upon_leaf) root_pos = -1;
                else root_pos = root.son[0];
                memory_node.WriteInfo(root_pos, 1);
            } else memory_node.SingleUpdate(root, root_pos);
        }

    };

}

#endif //B_PLUS_TREE_BPT_HPP
