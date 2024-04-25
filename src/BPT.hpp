#ifndef B_PLUS_TREE_BPT_HPP
#define B_PLUS_TREE_BPT_HPP

#include "Database.hpp"
#include "../STLite-baihua/utility.hpp"
#include "../STLite-baihua/vector.hpp"

namespace baihua {

    template<class INDEX, class VALUE, int (*CmpIndex)(const INDEX &, const INDEX &), int (*CmpValue)(const VALUE &, const VALUE &)>
    class BPT {
    public:
        using value_type = pair<INDEX, VALUE, CmpIndex, CmpValue>;

    private:
        static constexpr int M = 5;
        static constexpr int L = 5;

        struct Node {
            value_type key[M - 1]; // 0-base
            int son[M]; // 0-base
            int size; // the number of the keys
            int father;
            bool is_upon_leaf;

        };

        struct Leaf {
            value_type key;
            int address;
            int father;
            int size;
            int next;
            int pre;

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

        void Insert(const INDEX &index, const VALUE &value) {

        }

        void Delete(const INDEX &index, const VALUE &value) {

        }

        vector<value_type> &Find(const INDEX &index) {

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
        int BinarySearchIndex(const INDEX &index, const value_type *data, const int size) {
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

        int FindIndex(const INDEX &index) {
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
