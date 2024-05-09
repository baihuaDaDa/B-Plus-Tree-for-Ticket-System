#include "../src/BPT.hpp"
#include "../STLite-baihua/constantLengthString.hpp"

namespace baihua {
    int CmpInt(const int &x, const int &y) {
        if (x < y) return -1;
        else if (x > y) return 1;
        else return 0;
    }
}

int main() {
//    freopen("../test/completeness/completeness.in", "r", stdin);
//    freopen("../test/ans.out", "w", stdout);
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    baihua::BPT<baihua::ConstLenStr<65>, int, baihua::CmpStr, baihua::CmpInt> memory("string64-int");
    int n, value;
    std::string instruction, buffer;
    baihua::vector<int> result;
    std::cin >> n;
    for (int i = 1; i <= n; i++) {
        std::cin >> instruction;
        if (instruction == "insert") {
            std::cin >> buffer >> value;
            baihua::ConstLenStr<65> index{buffer};
            memory.Insert(index, value);
//            std::cout << memory.get_num_of_block() << std::endl;
        }
        if (instruction == "delete") {
            std::cin >> buffer >> value;
            baihua::ConstLenStr<65> index{buffer};
            memory.Delete(index, value);
//            std::cout << memory.get_num_of_block() << std::endl;
        }
        if (instruction == "find") {
            std::cin >> buffer;
//            std::cout << ++find_count << '|';
            baihua::ConstLenStr<65> index{buffer};
            result = memory.Find(index);
            if (result.empty()) std::cout << "null";
            else for (int & elem : result) std::cout << elem << ' ';
            std::cout << '\n';
//            std::cout << memory.get_num_of_block() << std::endl;
        }
    }
    return 0;
}
