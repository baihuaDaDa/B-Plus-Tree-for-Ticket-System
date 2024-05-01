#ifndef BOOKSTORE_2023_CONSTANTLENGTHSTRING_HPP
#define BOOKSTORE_2023_CONSTANTLENGTHSTRING_HPP

#include <exception>
#include <cstring>
#include <iostream>
#include <string>

namespace baihua {

    class StringIndexingError : public std::exception {
    public:
        const char *what() noexcept {
            return "String indexing error";
        }
    };

    class StringSizeError : public std::exception {
    public:
        const char *what() noexcept {
            return "String size error";
        }
    };

    template<int length>
    class ConstLenStr;

    template<int length>
    int CmpStr(const ConstLenStr<length> &lhs, const ConstLenStr<length> &rhs);

    template<int length>
    std::ostream &operator<<(std::ostream &out, const ConstLenStr<length> &str1);

    template<int length>
    std::istream &operator>>(std::istream &in, ConstLenStr<length> &str1);

    template<int length>
    class ConstLenStr {
    private:
        char str[length] = {};
        size_t size = 0;

    public:
        friend int CmpStr<length>(const ConstLenStr<length> &lhs, const ConstLenStr<length> &rhs);

        ConstLenStr() = default;

        ConstLenStr(const ConstLenStr<length> &str1) {
            std::strcpy(str, str1.str);
            size = str1.size;
        }

        ConstLenStr(const char str1[length]) {
            size_t i;
            for (i = 0; str1[i] != '\0'; i++) {
                str[i] = str1[i];
            }
            size = i;
        }

        ConstLenStr(const std::string &str1) {
            if (str1.size() >= length) {
                throw StringSizeError();
            }
            for (size_t i = 0; i < str1.size(); i++) {
                str[i] = str1[i];
            }
            size = str1.size();
        }

        ~ConstLenStr() = default;

        size_t GetSize() const {
            return size;
        }

        const char *GetHead() const {
            return str;
        }

        void Append(const char ch) {
            if (size == length - 1) {
                throw StringSizeError();
            }
            str[size++] = ch;
        }

        void Clear() {
            memset(str, '\0', size);
            size = 0;
        }

        bool operator==(const ConstLenStr<length> &rhs) {
            if (size != rhs.size) return false;
            for (size_t i = 0; i < size; i++) {
                if (str[i] != rhs.str[i])
                    return false;
            }
            return true;
        }

        ConstLenStr &operator=(const ConstLenStr<length> &rhs) {
            if (this == &rhs) {
                return *this;
            }
            std::strcpy(str, rhs.str);
            size = rhs.size;
            return *this;
        }

        char &operator[](int index) {
            if (index >= length) {
                throw StringIndexingError();
            }
            return str[index];
        }

        friend std::ostream &operator<<<length>(std::ostream &out, const ConstLenStr<length> &str1);

        friend std::istream &operator>><length>(std::istream &in, ConstLenStr<length> &str1);
    };

    template<int length>
    std::ostream &operator<<(std::ostream &out, const ConstLenStr<length> &str1) {
        for (size_t i = 0; i < str1.size; i++) {
            out << str1.str[i];
        }
        return out;
    }

    template<int length>
    std::istream &operator>>(std::istream &in, ConstLenStr<length> &str1) {
        string str;
        in >> str;
        for (int i = 0; i < str.size(); ++i)
            str1.str[i] = str[i];
        str1.size = str.size();
        return in;
    }

    template<int length>
    int CmpStr(const ConstLenStr<length> &lhs, const ConstLenStr<length> &rhs) {
        int flag = std::strcmp(lhs.str, rhs.str);
        if (flag < 0) return -1;
        if (flag > 0) return 1;
        return 0;
    }

}
#endif //BOOKSTORE_2023_CONSTANTLENGTHSTRING_HPP
