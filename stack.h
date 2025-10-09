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
#include <vector>

template<typename T>
class Stack {
public:
    Stack(char prefix, Error err_underflow)
        : prefix_(prefix),
          err_underflow_(err_underflow),
          sp_(0) {
    }

    void clear() {
        sp_ = 0;    // keep the data for catch
    }

    uint size() const {
        return static_cast<uint>(sp_);
    }

    bool empty() const {
        return sp_ == 0;
    }

    void resize(uint new_size) {
        if (new_size > data_.size()) {
            data_.resize(new_size);
        }
        sp_ = new_size;
    }

    void push(const T& value) {
        if (sp_ < data_.size()) {
            data_[sp_++] = value;
        }
        else {
            data_.push_back(value);
            sp_ = data_.size();
        }
    }

    T pop() {
        if (empty()) {
            error(err_underflow_);
        }
        T value = data_[--sp_];
        return value;
    }

    const T& peek(uint depth = 0) const {
        static T empty{};
        if (depth >= sp_) {
            error(err_underflow_);
            return empty;
        }
        else {
            size_t idx = sp_ - 1 - depth;
            return data_[idx];
        }
    }

    void poke(uint depth, const T& value) {
        if (depth >= sp_) {
            error(err_underflow_);
        }
        else {
            size_t idx = sp_ - 1 - depth;
            data_[idx] = value;
        }
    }

    void roll(uint depth) {
        if (depth >= sp_) {
            error(err_underflow_);
        }
        if (depth == 0) {
            ; // ignore
        }
        else {
            size_t idx = sp_ - 1 - depth;
            T value = data_[idx];
            for (size_t i = idx + 1; i < sp_; ++i) {
                data_[i - 1] = data_[i];
            }
            data_[sp_ - 1] = value;
        }
    }

    void print() const {
        std::cout << "(";
        if (prefix_ != '\0') {
            std::cout << prefix_ << ":";
        }
        std::cout << BL;
        for (uint i = 0; i < sp_; ++i) {
            print_number(data_[i]);
        }
        std::cout << ") ";
    }

    void print_debug() const {
        std::cout << "(";
        if (prefix_ != '\0') {
            std::cout << prefix_ << ":";
        }
        std::cout << BL;
        for (uint i = 0; i < sp_; ++i) {
            std::cout << data_[i] << BL;
        }
        std::cout << ") ";
    }

private:
    char prefix_;
    Error err_underflow_;
    std::vector<T> data_;
    size_t sp_;
};
