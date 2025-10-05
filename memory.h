//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

class Mem {
public:
    Mem();

    // pointer - address conversion
    uint addr(const char* ptr) const;
    uint addr(const int* ptr) const;
    char* char_ptr(uint addr, uint size = 0);
    int* int_ptr(uint addr, uint size = 0);
    double* float_ptr(uint addr, uint size = 0);
    float* sfloat_ptr(uint addr, uint size = 0);

    // access memory
    int fetch(uint addr);
    void store(uint addr, int value);
    dint dfetch(uint addr);
    void dstore(uint addr, dint value);
    void fstore(uint addr, double value);
    double ffetch(uint addr);
    void sfstore(uint addr, float value);
    float sffetch(uint addr);
    int cfetch(uint addr);
    void cstore(uint addr, int value);

    // block operations
    void fill(uint addr, uint size, char c);
    void erase(uint addr, uint size);
    void move(int src, int dst, uint size);

    // allocate memory for data structures
    char* alloc_bottom(uint size);
    char* alloc_top(uint size);

private:
    char data_[MEM_SZ];
    uint top_;
    uint bottom_;

    int check_addr(uint addr, uint size = 0) const;
};

void f_fill();
void f_erase();
void f_move();

class Heap {
public:
    void init();
    uint allocate(uint size);
    void free(uint ptr);
    uint resize(uint, uint new_size);

private:
    struct Block {
        uint size;
        int free;
        uint next;
    };

    uint pool_{ 0 };
    uint size_{ 0 };
};

void f_allocate();
void f_free();
void f_resize();
