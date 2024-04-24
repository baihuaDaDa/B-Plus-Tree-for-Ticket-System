#ifndef B_PLUS_TREE_DATABASE_HPP
#define B_PLUS_TREE_DATABASE_HPP

#include <iostream>
#include <fstream>

namespace baihua {

    using std::string;
    using std::fstream;
    using std::ifstream;
    using std::ofstream;

    template<class T, int info_len = 2>
    class Database {
    private:
        string filename;
        fstream file;
        int sizeofT = sizeof(T);

    public:
        Database() = default;

        ~Database() = default;

        Database(const string &_filename) : filename(_filename) {}

        bool isFileExist() {
            ifstream tmp_file(filename);
            return tmp_file.good();
        }

        void initialize(const string &FN = "") {
            if (FN != "") filename = FN;
            file.open(filename, std::ios::out | std::ios::binary);
            int tmp = 0;
            for (int i = 0; i < info_len; ++i)
                file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
            file.close();
        }

        // Read the @n-th info into @tmp.
        void ReadInfo(int &tmp, int n) {
            if (n > info_len) return;
            file.open(filename, std::ios::in | std::ios::binary);
            file.seekg((n - 1) * sizeof(int));
            file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
            file.close();
        }

        // Write @tmp into the @n-th info.
        void WriteInfo(int tmp, int n) {
            if (n > info_len) return;
            file.open(filename, std::ios::out | std::ios::in | std::ios::binary);
            file.seekp((n - 1) * sizeof(int));
            file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
            file.close();
        }

        // Update the data at @index with the value of @t.
        void SingleUpdate(T &t, const int index) {
            file.open(filename, std::ios::out | std::ios::in | std::ios::binary);
            file.seekp(index);
            file.write(reinterpret_cast<char *>(&t), sizeofT);
            file.close();
        }

        // Update the data at @index with the @size values after the pointer @t.
        void MultiUpdate(T *t, const int size, const int index) {
            file.open(filename, std::ios::out | std::ios::in | std::ios::binary);
            file.seekp(index);
            file.write(reinterpret_cast<char *>(t), size * sizeofT);
            file.close();
        }

        // Read the data at @index into @t
        void SingleRead(T &t, const int index) {
            file.open(filename, std::ios::in | std::ios::binary);
            file.seekg(index);
            file.read(reinterpret_cast<char *>(&t), sizeofT);
            file.close();
        }

        // Read the data at @index into the space the pointer @t points to, and the size of the data is @size.
        void MultiRead(T *t, const int size, const int index) {
            file.open(filename, std::ios::in | std::ios::binary);
            file.seekg(index);
            file.read(reinterpret_cast<char *>(t), size * sizeofT);
            file.close();
        }

    };

}

#endif //B_PLUS_TREE_DATABASE_HPP