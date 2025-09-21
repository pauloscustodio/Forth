//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "errors.h"
#include "forth.h"
#include <iostream>
#include <memory>
#include <string>

template<typename T>
class DownwardStack {
public:
    DownwardStack(char prefix, Error err_underflow, Error err_overflow,
                  uint initial_capacity = 8)
        : prefix_(prefix),
          err_underflow_(err_underflow),
          err_overflow_(err_overflow),
          buffer_(new T[initial_capacity]),
          capacity_(initial_capacity),
          size_(0) {
    }

    void clear() {
        size_ = 0;
    }

    bool is_empty() const {
        return size_ == 0;
    }

    int depth() const {
        return size_;
    }

    uint sp() const {
        return STACK_SZ - size_;
    }

    void set_sp(uint new_sp) {
        if (new_sp < STACK_SZ - capacity_ || new_sp > STACK_SZ) {
            error(Error::InvalidMemoryAddress);
        }
        size_ = STACK_SZ - new_sp;
    }

    void push(const T& value) {
        if (size_ == capacity_) {
            grow();
        }
        ++size_;
        buffer_[index()] = value;
    }

    T pop() {
        if (size_ == 0) {
            error(err_underflow_);
        }
        T value = buffer_[index()];
        --size_;
        return value;
    }

    const T& peek(int depth = 0) const {
        static T empty{};
        if (depth < 0) {
            error(Error::InvalidMemoryAddress);
            return empty;
        }
        else if (static_cast<uint>(depth) >= size_) {
            error(err_underflow_);
            return empty;
        }
        else {
            uint idx = index() + depth;
            return buffer_[idx];
        }
    }

    void roll(int depth) {
        if (depth < 0) {
            error(Error::InvalidMemoryAddress);
        }
        else if (static_cast<uint>(depth) >= size_) {
            error(err_underflow_);
        }
        if (depth == 0) {
            ; // ignore
        }
        else {
            uint idx = index() + depth;
            T value = buffer_[idx];
            for (uint i = idx; i > index(); --i) {
                buffer_[i] = buffer_[i - 1];
            }
            buffer_[index()] = value;
        }
    }

    void print() const {
        std::cout << "(";
        if (prefix_ != '\0') {
            std::cout << prefix_ << ":";
        }
        std::cout << BL;
        for (uint i = 0; i < size_; ++i) {
            print_number(buffer_[capacity_ - 1 - i]);
        }
        std::cout << ") ";
    }

    void print_debug() const {
        std::cout << "(";
        if (prefix_ != '\0') {
            std::cout << prefix_ << ":";
        }
        std::cout << BL;
        for (uint i = 0; i < size_; ++i) {
            // do not print -0
            T v = buffer_[capacity_ - 1 - i];
            if (v == 0) {
                v = 0;
            }
            std::cout << v << BL;
        }
        std::cout << ") ";
    }

private:
    char prefix_;
    Error err_underflow_;
    Error err_overflow_;
    std::unique_ptr<T[]> buffer_;
    uint capacity_;
    uint size_;

    uint index() const {
        return capacity_ - size_;
    }

    void grow() {
        uint new_capacity = capacity_ * 2;
        std::unique_ptr<T[]> new_buffer(new T[new_capacity]);

        for (uint i = 0; i < size_; ++i) {
            new_buffer[new_capacity - 1 - i] =
                buffer_[capacity_ - 1 - i];
        }

        buffer_ = std::move(new_buffer);
        capacity_ = new_capacity;
    }
};
