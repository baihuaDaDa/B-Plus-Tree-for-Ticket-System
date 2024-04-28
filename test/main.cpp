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
    baihua::BPT<baihua::ConstLenStr<65>, int, baihua::CmpStr, baihua::CmpInt> memory("string64-int");
    int n, value;
    std::string instruction;
    baihua::ConstLenStr<65> index;
    baihua::vector<int> result;
    std::cin >> n;
    for (int i = 1; i <= n; i++) {
        std::cin >> instruction;
        if (instruction == "insert") {
            std::cin >> index >> value;
            memory.Insert(index, value);
//            std::cout << memory.get_num_of_block() << std::endl;
        }
        if (instruction == "delete") {
            std::cin >> index >> value;
            memory.Delete(index, value);
//            std::cout << memory.get_num_of_block() << std::endl;
        }
        if (instruction == "find") {
            std::cin >> index;
//            std::cout << ++find_count << '|';
            result = memory.Find(index);
            for (int & elem : result) std::cout << elem << ' ';
            std::cout << '\n';
//            std::cout << memory.get_num_of_block() << std::endl;
        }
    }
    return 0;
}
