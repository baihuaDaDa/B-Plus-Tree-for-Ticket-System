#include "../src/BPT.hpp"

namespace baihua {
    int CmpInt(const int &x, const int &y) {
        if (x < y) return -1;
        else if (x > y) return 1;
        else return 0;
    }

    int CmpLongLong(const  long long &x, const long long &y) {
        if (x < y) return -1;
        else if (x > y) return 1;
        else return 0;
    }

    long long Hash(const std::string &str) {
        const long long BASE = 197, MOD = 1e9 + 7;
        long long hash = 0;
        for (char i : str) hash = (hash * BASE + i) % MOD;
        return hash;
    }
}

int main() {
//    freopen("../test/completeness/completeness.in", "r", stdin);
//    freopen("../test/ans.out", "w", stdout);
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    baihua::BPT<long long, int, baihua::CmpLongLong, baihua::CmpInt> memory("string64hash-int");
    int n, value;
    std::string instruction, index;
    baihua::vector<int> result;
    std::cin >> n;
    for (int i = 1; i <= n; i++) {
        std::cin >> instruction;
        if (instruction == "insert") {
            std::cin >> index >> value;
            memory.Insert(baihua::Hash(index), value);
//            std::cout << memory.get_num_of_block() << std::endl;
        }
        if (instruction == "delete") {
            std::cin >> index >> value;
            memory.Delete(baihua::Hash(index), value);
//            std::cout << memory.get_num_of_block() << std::endl;
        }
        if (instruction == "find") {
            std::cin >> index;
//            std::cout << ++find_count << '|';
            result = memory.Find(baihua::Hash(index));
            if (result.empty()) std::cout << "null";
            else for (int & elem : result) std::cout << elem << ' ';
            std::cout << '\n';
//            std::cout << memory.get_num_of_block() << std::endl;
        }
    }
    return 0;
}
