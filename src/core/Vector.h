/*
 * Copyright 2019 Aurélien Gâteau <mail@agateau.com>
 *
 * This file is part of Lovi.
 *
 * Lovi is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef VECTOR_H
#define VECTOR_H

#include <vector>

namespace stdq {
/**
 * A vector with the features of an STL vector, but Qt-like types: using int instead of size_t.
 * This helps reduce warnings.
 */
template <class T> class Vector {
public:
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;

    void clear() {
        mVector.clear();
    }

    void resize(int size) {
        mVector.resize(size);
    }

    const T& at(int idx) const {
        return mVector.at(std::size_t(idx));
    }

    T& operator[](int idx) {
        return mVector[std::size_t(idx)];
    }

    const T& operator[](int idx) const {
        return at(idx);
    }

    int size() const {
        return int(mVector.size());
    }

    void push_back(const T& item) {
        mVector.push_back(item);
    }

    void push_back(T&& item) {
        mVector.push_back(std::move(item));
    }

    const T& back() const {
        return mVector.back();
    }

    T& back() {
        return mVector.back();
    }

    const_iterator erase(const_iterator it) {
        return mVector.erase(it);
    }

    iterator begin() {
        return mVector.begin();
    }

    iterator end() {
        return mVector.end();
    }

    const_iterator begin() const {
        return mVector.begin();
    }

    const_iterator end() const {
        return mVector.end();
    }

private:
    std::vector<T> mVector;
};

} // namespace stdq

#endif // VECTOR_H
