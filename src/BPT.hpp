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

        struct BptNode {
            value_type key[M - 1];
            int son[M];
            int size;
            int father;

        };

        struct BptLeafNode {
            int address;
            int size;
            int father;

        };

        int root_pos;
        Database<BptNode> node_memory;
        Database<BptLeafNode> leaf_node_memory;
        Database<value_type> database;

    public:
        BPT(const string &filename) {
            string node_filename = filename + "_BptNode";
            string leaf_filename = filename + "_BptLeafNode";
            if (!node_memory.isFileExist()) {
                node_memory.initialize();
            }
            if (!leaf_node_memory.isFileExist()) {
                leaf_node_memory.initialize();
            }
            if (!database.isFileExist()) {
                database.initialize();
            } // Check if the file exist. If not, then initialise it.
        }

        void Insert(const INDEX &index, const VALUE &value) {

        }

        void Delete(const INDEX &index, const VALUE &value) {

        }

        vector<value_type> &Find(const INDEX &index) {

        }

    private:
        int FindElement(const value_type &element) {
            BptLeafNode leaf_node;
            int pos = head_pos;
            memory_BlockNode.read(leaf_node, pos);
            while (pos != tail_pos) {
//            std::cout << leaf_node.size << leaf_node.pre << std::endl;
//            if (leaf_node.index == "") std::cout << 1 <<  std::endl;
                if (cmp_pair(leaf_node.element, element) == 1) {
                    break;
                }
                pos = leaf_node.next;
                memory_BlockNode.read(leaf_node, pos);
            }
            return leaf_node.pre;
        }

        int BinarySearchPair(const value_type &element, const value_type *data, const int size) {
            int l = 0, r = size - 1, mid;
            while (l <= r) {
                mid = l + (r - l) / 2;
                int flag = CmpPair(data[mid], element);
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

    };

}

#endif //B_PLUS_TREE_BPT_HPP
