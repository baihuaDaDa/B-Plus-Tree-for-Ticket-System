#include "../src/BPT.h"

int main() {
    Memory memory("BlockNode.bin", "element.bin");
    int n, value;
    string instruction;
    int find_count = 0;
    std::cin >> n;
    for (int i = 1; i <= n; i++) {
        char index[66] = {};
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
            memory.Find(index);
//            std::cout << memory.get_num_of_block() << std::endl;
        }
    }
    return 0;
}
