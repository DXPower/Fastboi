#pragma once

#include <vector>

template<typename T>
class circular_vector : public std::vector<T> {
    public:
    circular_vector() : std::vector<T>() { };
    // explicit circular_vector(uint64_t count) : std::vector<T>(count) { };
    circular_vector(const circular_vector<T>& copy) : std::vector<T>(copy) { };
    circular_vector(circular_vector<T>&& copy) noexcept : std::vector<T>(copy) { };

    circular_vector<T>& operator=(const circular_vector<T>& other) {
        std::vector<T>::operator=(other);

        return *this;
    }

    circular_vector<T>& operator=(circular_vector<T>&& other) noexcept {
        std::vector<T>::operator=(other);

        return *this;
    }

    circular_vector<T>& operator=(std::initializer_list<T> ilist) {
        std::vector<T>::operator=(ilist);

        return *this;
    }

    struct pair_iterator;

    // Iterator for every pair of elements (including the pair (last, first))
    pair_iterator pair_begin() const {
        return pair_iterator(this, 0);
    }

    // Iterator for every pair of elements (including the pair (last, first))
    pair_iterator pair_end() const {
        return pair_iterator(this, this->size());
    }

    // Iterator for every pair of elements (including the pair (last, first))
    struct pair_iterator {
        private:
        const circular_vector<T>* v;
        T* data;
        typename std::vector<T>::size_type i;

        public:
        T* first;
        T* second;

        pair_iterator(const circular_vector<T>* v, typename std::vector<T>::size_type startIndex)
        : v(v), data(const_cast<T*>(v->data())), i(startIndex) {
            if (v->size() > 1) {
                first = &data[0];
                second = &data[1];
            } else {
                first = nullptr;
                second = nullptr;
            }
        }

        public:
        // Post increment
        void operator++(int) {
            i++;

            if (i < v->size()) {
                first = &data[i];
                second = &data[i + 1 != v->size() ? i + 1 : 0];
            } else {
                first = nullptr;
                second = nullptr;
            }
        }

        bool operator==(const pair_iterator& other) const {
            return i == other.i
                && v == other.v
                && data == other.data;
        }

        bool operator!=(const pair_iterator& other) const {
            return !(*this == other);
        }

        pair_iterator operator+(typename std::vector<T>::size_type offset) const {
            return pair_iterator(v, std::min(i + offset, v->size());)
        }
    };
};

