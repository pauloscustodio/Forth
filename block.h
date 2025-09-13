//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include <iostream>
#include <fstream>

struct Block {
    uint index;         // sequence number of block
    int blk;            // block numnber mapped to this buffer, 0 if none
    bool dirty;         // true if needs to be written to file

    void init(uint index, int blk);
    char* data() const;

    // buffer stored in vm.block_data
};

class Blocks {
public:
    void init();
    void deinit();

    int num_blocks();
    Block* f_block(int blk);
    void f_empty_buffers();
    void f_save_buffers();
    void f_flush();
    void f_list(int blk);
    void f_load(int blk);
    void f_thru(int first, int last);
    void f_update();

private:
    std::fstream* block_file_;          // file handle
    Block blocks_[NUM_BLK_BUFFERS];     // block buffers
    int last_block_;                    // index of last block referenced

    std::fstream* block_file();         // get block file handle
    bool seek_block(int blk);          // seek to block position

    int find_buffer_index(int blk) const;   // -1 if not found
    int find_first_unused() const;      // -1 if not found
    int find_first_not_dirty() const;   // -1 if not found
    void flush_block(int index);        // flush to file if dirty, init

    bool read_block(int index, int blk);
    bool write_block(int index);
};

void f_block();
void f_save_buffers();
void f_empty_buffers();
void f_flush();
void f_load();
void f_update();
void f_list();
void f_thru();
