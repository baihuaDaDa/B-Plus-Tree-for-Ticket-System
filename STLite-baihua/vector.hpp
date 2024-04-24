#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>

namespace baihua {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
    template<typename T, typename Alloc = std::allocator<T>>
    class vector {
    public:
        /**
         * TODO
         * a type for actions of the elements of a vector, and you should write
         *   a class named const_iterator with same interfaces.
         */
        /**
         * you can see RandomAccessIterator at CppReference for help.
         */
        class const_iterator;

        class iterator {
            // The following code is written for the C++ type_traits library.
            // Type traits is a C++ feature for describing certain properties of a type.
            // For instance, for an iterator, iterator::value_type is the type that the
            // iterator points to.
            // STL algorithms and containers may use these type_traits (e.g. the following
            // typedef) to work properly. In particular, without the following code,
            // @code{std::sort(iter, iter1);} would not compile.
            // See these websites for more information:
            // https://en.cppreference.com/w/cpp/header/type_traits
            // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
            // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::output_iterator_tag;

        private:
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            T *pos;
            vector<T> *tag;

        public:
            iterator() = default;

            iterator(T *_pos, vector<T> *_tag) : pos(_pos), tag(_tag) {}

            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            iterator operator+(const int &n) const {
                //TODO
                return iterator(pos + n, tag);
            }

            iterator operator-(const int &n) const {
                //TODO
                return iterator(pos - n, tag);
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                //TODO
                if (tag != rhs.tag) throw invalid_iterator();
                return pos - rhs.pos;
            }

            iterator &operator+=(const int &n) {
                //TODO
                pos += n;
                return *this;
            }

            iterator &operator-=(const int &n) {
                //TODO
                pos -= n;
                return *this;
            }

            /**
             * TODO iter++
             */
            iterator operator++(int) {
                iterator origin = *this;
                pos++;
                return origin;
            }

            /**
             * TODO ++iter
             */
            iterator &operator++() {
                pos++;
                return *this;
            }

            /**
             * TODO iter--
             */
            iterator operator--(int) {
                iterator origin = *this;
                pos--;
                return origin;
            }

            /**
             * TODO --iter
             */
            iterator &operator--() {
                pos--;
                return *this;
            }

            /**
             * TODO *it
             */
            T &operator*() const {
                return *pos;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                return pos == rhs.pos;
            }

            bool operator==(const const_iterator &rhs) const {
                return pos == rhs.pos;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return pos != rhs.pos;
            }

            bool operator!=(const const_iterator &rhs) const {
                return pos != rhs.pos;
            }

            T *GetPointer() const {
                return pos;
            }

            friend const_iterator;
        };

        /**
         * TODO
         * has same function as iterator, just for a const object.
         */
        class const_iterator {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::output_iterator_tag;

        private:
            /*TODO*/
            const T *pos;
            const vector<T> *tag;

        public:
            const_iterator() = default;

            const_iterator(const T *_pos, const vector<T> *_tag) : pos(_pos), tag(_tag) {}

            const_iterator(const iterator &iter) : pos(iter.pos), tag(iter.tag) {}

            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */
            const_iterator operator+(const int &n) const {
                //TODO
                return const_iterator(pos + n, tag);
            }

            const_iterator operator-(const int &n) const {
                //TODO
                return const_iterator(pos - n, tag);
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const const_iterator &rhs) const {
                //TODO
                if (tag != rhs.tag) throw invalid_iterator();
                return *this - rhs.pos;
            }

            const_iterator &operator+=(const int &n) {
                //TODO
                pos += n;
                return *this;
            }

            const_iterator &operator-=(const int &n) {
                //TODO
                pos -= n;
                return *this;
            }

            /**
             * TODO iter++
             */
            const_iterator operator++(int) {
                const_iterator origin = *this;
                pos++;
                return origin;
            }

            /**
             * TODO ++iter
             */
            const_iterator &operator++() {
                pos++;
                return *this;
            }

            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                const_iterator origin = *this;
                pos--;
                return origin;
            }

            /**
             * TODO --iter
             */
            const_iterator &operator--() {
                pos--;
                return *this;
            }

            /**
             * TODO *it
             */
            const T &operator*() const {
                return *pos;
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                return pos == rhs.pos;
            }

            bool operator==(const const_iterator &rhs) const {
                return pos == rhs.pos;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return pos != rhs.pos;
            }

            bool operator!=(const const_iterator &rhs) const {
                return pos != rhs.pos;
            }

            const T *GetPointer() const {
                return pos;
            }
        };

    private:
        iterator _begin;
        iterator _end;
        Alloc alloc;
        size_t _size;
        size_t _storage;

    public:
        /**
         * TODO Constructs
         * At least two: default constructor, copy constructor
         */
        vector() : _size(0), _storage(0) {
            T *ptr = alloc.allocate(1);
            _begin = _end = iterator(ptr, this);
        }

        vector(const vector &other) : _size(other._size), _storage(other._storage) {
            T *ptr = alloc.allocate(_storage + 1);
            _begin = iterator(ptr, this);
            _end = iterator(ptr + _size, this);
            for (size_t i = 0; i < _size; i++) {
                std::construct_at(ptr + i, other[i]);
            }
        }

        /**
         * TODO Destructor
         */
        ~vector() {
            std::destroy(_begin, _end);
            alloc.deallocate(_begin.GetPointer(), _storage + 1);
        }

        /**
         * TODO Assignment operator
         */
        vector &operator=(const vector &other) {
            if (&other == this) return *this;
            std::destroy(_begin, _end);
            alloc.deallocate(_begin.GetPointer(), _storage + 1);
            _size = other._size;
            _storage = other._storage;
            T *ptr = alloc.allocate(_storage + 1);
            _begin = iterator(ptr, this);
            _end = iterator(ptr + _size, this);
            for (size_t i = 0; i < _size; i++) {
                std::construct_at(ptr + i, other[i]);
            }
            return *this;
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         */
        T &at(const size_t &pos) {
            if (pos < 0 || pos >= _size) throw index_out_of_bound();
            return *(_begin + pos);
        }

        const T &at(const size_t &pos) const {
            if (pos < 0 || pos >= _size) throw index_out_of_bound();
            return *(_begin + pos);
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         * !!! Pay attentions
         *   In STL this operator does not check the boundary but I want you to do.
         */
        T &operator[](const size_t &pos) {
            if (pos < 0 || pos >= _size) throw index_out_of_bound();
            return at(pos);
        }

        const T &operator[](const size_t &pos) const {
            if (pos < 0 || pos >= _size) throw index_out_of_bound();
            return at(pos);
        }

        /**
         * access the first element.
         * throw container_is_empty if size == 0
         */
        const T &front() const {
            if (_size == 0) throw container_is_empty();
            return *_begin;
        }

        /**
         * access the last element.
         * throw container_is_empty if size == 0
         */
        const T &back() const {
            if (_size == 0) throw container_is_empty();
            return *(_end - 1);
        }

        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {
            return _begin;
        }

        const_iterator cbegin() const {
            return _begin;
        }

        /**
         * returns an iterator to the end.
         */
        iterator end() {
            return _end;
        }

        const_iterator cend() const {
            return _end;
        }

        /**
         * checks whether the container is empty
         */
        bool empty() const {
            return _size == 0;
        }

        /**
         * returns the number of elements
         */
        size_t size() const {
            return _size;
        }

        /**
         * clears the contents
         */
        void clear() {
            std::destroy(_begin, _end);
            alloc.deallocate(_begin.GetPointer(), _storage + 1);
            _size = 0;
            _storage = 0;
            T *ptr = alloc.allocate(1);
        }

        /**
         * inserts value before pos
         * returns an iterator pointing to the inserted value.
         */
        iterator insert(iterator pos, const T &value) {
            if (_size == _storage) {
                T *new_ptr = alloc.allocate((_storage == 0 ? 2 : 2 * _storage + 1));
                for (size_t i = 0; i < pos - _begin; i++) {
                    std::construct_at(new_ptr + i, at(i));
                }
                for (size_t i = pos - _begin + 1; i <= _size; i++) {
                    std::construct_at(new_ptr + i, at(i - 1));
                }
                pos = iterator(std::construct_at(new_ptr + (pos - _begin), value), this);
                std::destroy(_begin, _end);
                alloc.deallocate(_begin.GetPointer(), _storage + 1);
                if (_storage == 0) _storage = 1;
                else _storage *= 2;
                _size++;
                _begin = iterator(new_ptr, this);
                _end = iterator(new_ptr + _size, this);
            } else {
                std::construct_at(_begin.GetPointer() + _size, at(_size - 1));
                for (size_t i = _size - 1; i >= pos - _begin + 1; i--) {
                    at(i) = at(i - 1);
                }
                *pos = value;
                _size++;
                _end++;
            }
            return pos;
        }

        /**
         * inserts value at index ind.
         * after inserting, this->at(ind) == value
         * returns an iterator pointing to the inserted value.
         * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
         */
        iterator insert(const size_t &ind, const T &value) {
            if (ind > _size) throw index_out_of_bound();
            if (_size == _storage) {
                T *new_ptr = alloc.allocate((_storage == 0 ? 2 : 2 * _storage + 1));
                for (size_t i = 0; i < ind; i++) {
                    std::construct_at(new_ptr + i, at(i));
                }
                for (size_t i = ind + 1; i <= _size; i++) {
                    std::construct_at(new_ptr + i, at(i - 1));
                }
                std::construct_at(new_ptr + ind, value);
                std::destroy(_begin, _end);
                alloc.deallocate(_begin.GetPointer(), _storage + 1);
                if (_storage == 0) _storage = 1;
                else _storage *= 2;
                _size++;
                _begin = iterator(new_ptr, this);
                _end = iterator(new_ptr + _size, this);
            } else {
                std::construct_at(_begin.GetPointer() + _size, at(_size - 1));
                for (size_t i = _size - 1; i >= ind + 1; i--) {
                    at(i) = at(i - 1);
                }
                *(_begin + ind) = value;
                _size++;
                _end++;
            }
            return _begin + ind;
        }

        /**
         * removes the element at pos.
         * return an iterator pointing to the following element.
         * If the iterator pos refers the last element, the end() iterator is returned.
         */
        iterator erase(iterator pos) {
            if (pos == _end) return _end;
            if (_storage / 2 == _size - 1) {
                T *new_ptr = alloc.allocate(_storage / 2 + 1);
                for (size_t i = 0 ; i < pos - _begin; i++) {
                    std::construct_at(new_ptr + i, at(i));
                }
                for (size_t i = pos - _begin; i < _size - 1; i++) {
                    std::construct_at(new_ptr + i, at(i + 1));
                }
                size_t ind = pos - _begin;
                std::destroy(_begin, _end);
                alloc.deallocate(_begin.GetPointer(), _storage + 1);
                _storage /= 2;
                _size--;
                _begin = iterator(new_ptr, this);
                _end = iterator(new_ptr + _size, this);
                pos = _begin + ind;
            } else {
                for (size_t i = pos - _begin; i < _size - 1; i++) {
                    at(i) = at(i + 1);
                }
                _size--;
                std::destroy_at(_begin.GetPointer() + _size);
                _end--;
            }
            return pos;
        }

        /**
         * removes the element with index ind.
         * return an iterator pointing to the following element.
         * throw index_out_of_bound if ind >= size
         */
        iterator erase(const size_t &ind) {
            if (ind >= _size) throw index_out_of_bound();
            if (_storage == 2 * (_size - 1)) {
                T *new_ptr = alloc.allocate(_storage / 2 + 1);
                for (size_t i = 0 ; i < ind; i++) {
                    std::construct_at(new_ptr + i, at(i));
                }
                for (size_t i = ind; i < _size - 1; i++) {
                    std::construct_at(new_ptr + i, at(i + 1));
                }
                std::destroy(_begin, _end);
                alloc.deallocate(_begin.GetPointer(), _storage + 1);
                _storage /= 2;
                _size--;
                _begin = iterator(new_ptr, this);
                _end = iterator(new_ptr + _size, this);
            } else {
                for (size_t i = ind; i < _size - 1; i++) {
                    at(i) = at(i + 1);
                }
                _size--;
                std::destroy_at(_begin.GetPointer() + _size);
                _end--;
            }
            return _begin + ind;
        }

        /**
         * adds an element to the end.
         */
        void push_back(const T &value) {
            insert(_end, value);
        }

        /**
         * remove the last element from the end.
         * throw container_is_empty if size() == 0
         */
        void pop_back() {
            if (_size == 0) throw container_is_empty();
            erase(_end - 1);
        }
    };
}

#endif
