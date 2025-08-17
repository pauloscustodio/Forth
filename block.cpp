//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "block.h"
#include "errors.h"
#include "vm.h"
#include <cassert>
#include <cctype>
#include <cstring>
using namespace std;

void Block::init(int blk) {
    memset(this->block, BL, BLOCK_SZ);
    this->blk = blk;
    this->dirty = false;
}

void Blocks::init() {
    f_empty_buffers();
}

void Blocks::deinit() {
    delete block_file_;
}

int Blocks::num_blocks() {
    fstream* fs = block_file();

    fs->seekg(0, ios::end);
    streampos end_pos = fs->tellg();
    int num_blocks = static_cast<int>(end_pos/ BLOCK_SZ);
    return num_blocks;
}

Block* Blocks::f_block(int blk) {
    if (blk < 1)
        error(Error::InvalidBlockNumber);

    int index = find_buffer_index(blk);     // already exists, do not init
    if (index < 0) {
        index = find_first_unused();
        read_block(index, blk);
    }
    if (index < 0) {
        index = find_first_not_dirty();
        read_block(index, blk);
    }
    if (index < 0) {
        index = (last_block_ + 1) % NUM_BLK_BUFFERS;;
        flush_block(index);
        read_block(index, blk);
    }

    last_block_ = index;
    return &blocks_[last_block_];
}

void Blocks::f_empty_buffers() {
    for (int i = 0; i < NUM_BLK_BUFFERS; ++i)
        blocks_[i].init(0);
}

void Blocks::f_save_buffers() {
    for (int i = 0; i < NUM_BLK_BUFFERS; ++i)
        flush_block(i);
}

void Blocks::f_flush() {
    f_save_buffers();
    f_empty_buffers();
}

void Blocks::f_list(int blk) {
    if (blk < 1)
        error(Error::InvalidBlockNumber);

    vm.user->SCR = blk;
    Block* block = f_block(blk);

    int save_base = vm.user->BASE;
    vm.user->BASE = 10;

    cout << endl << "Block ";
    print_number(blk);
    cout << endl;
    for (int row = 0; row < BLOCK_ROWS; ++row) {
        print_number(row + 1, 2);
        cout << BL;
        for (int col = 0; col < BLOCK_COLS; ++col) {
            char c = block->block[row * BLOCK_COLS + col];
            if (isprint(c))
                cout << c;
            else
                cout << "?";
        }
        cout << endl;
    }

    vm.user->BASE = save_base;
}

void Blocks::f_load(int blk) {
    if (blk < 1)
        error(Error::InvalidBlockNumber);

    Block* block = f_block(blk);

    // save input context
    vm.input->save_input();

    // parse string
    vm.input->set_block(block);
    f_execute(xtINTERPRET);

    // restore input context
    vm.input->restore_input();
}

void Blocks::f_thru(int first, int last) {
    for (int i = first; i <= last; ++i)
        f_load(i);
}

void Blocks::f_update() {
    blocks_[last_block_].dirty = true;
}

fstream* Blocks::block_file() {
    if (block_file_ == nullptr) {
        block_file_ = new fstream(BLOCKS_FILE, 
            ios::in | ios::out | ios::binary);

        // if the file doesn't exist, create it
        if (!block_file_->is_open()) {
            block_file_->open(BLOCKS_FILE, 
                ios::out | ios::binary); // create the file
            block_file_->close();
            block_file_->open(BLOCKS_FILE,
                ios::in | ios::out | ios::binary); // reopen for read/write
        }

        if (!*block_file_) 
            error(Error::OpenFileException, BLOCKS_FILE);
    }
    return block_file_;
}

bool Blocks::seek_block(int blk) {
    assert(blk > 0);

    fstream* fs = block_file();
    streampos fpos = blk * BLOCK_SZ;

    fs->clear(); // clear any error flags
    fs->seekg(fpos);
    if (!*fs)
        return false;   // failbit or badbit set
    
    // verify actual position
    if (fs->tellg() != fpos) 
        return false; // seek landed elsewhere (can happen on some devices)
    
    return true;    // seek successful
}

int Blocks::find_buffer_index(int blk) const {
    assert(blk > 0);

    for (int i = 0; i < NUM_BLK_BUFFERS; ++i) {
        if (blocks_[i].blk == blk)
            return i;
    }
    return -1;
}

int Blocks::find_first_unused() const {
    int j = last_block_;
    for (int i = 0; i < NUM_BLK_BUFFERS; ++i) {
        j = (j + 1) % NUM_BLK_BUFFERS;
        if (blocks_[j].blk == 0)
            return j;
    }
    return -1;
}

int Blocks::find_first_not_dirty() const {
    int j = last_block_;
    for (int i = 0; i < NUM_BLK_BUFFERS; ++i) {
        j = (j + 1) % NUM_BLK_BUFFERS;
        if (blocks_[j].blk != 0 && !blocks_[j].dirty)
            return j;
    }
    return -1;
}

void Blocks::flush_block(int index) {
    assert(index >= 0 && index < NUM_BLK_BUFFERS);

    if (blocks_[index].blk > 0) {
        if (blocks_[index].dirty) 
            write_block(index);
        blocks_[index].dirty = false;
    }
}

bool Blocks::read_block(int index, int blk) {
    assert(index >= 0 && index < NUM_BLK_BUFFERS);
    assert(blk > 0);

    blocks_[index].init(blk); // clear with blanks

    fstream* fs = block_file();
    if (!seek_block(blk))
        return false; // seek failed

    fs->clear(); // clear any error flags
    fs->read(blocks_[index].block, BLOCK_SZ);
    if (!*fs)
        return false; // read failed
    else
        return true;
}

bool Blocks::write_block(int index) {
    assert(index >= 0 && index < NUM_BLK_BUFFERS);
    
    int blk = blocks_[index].blk;
    assert(blk > 0);

    fstream* fs = block_file();
    if (!seek_block(blk))
        return false; // seek failed

    fs->clear(); // clear any error flags
    fs->write(blocks_[index].block, BLOCK_SZ);
    if (!*fs)
        return false; // read failed
    else
        return true;
}

void f_block() {
    int blk = pop();
    Block* block = vm.blocks->f_block(blk);
    const char* buffer = block->block;
    push(mem_addr(buffer));
}

void f_save_buffers() {
    vm.blocks->f_save_buffers();
}

void f_empty_buffers() {
    vm.blocks->f_empty_buffers();
}

void f_flush() {
    vm.blocks->f_flush();
}

void f_load() {
    int blk = pop();
    vm.blocks->f_load(blk);
}

void f_update() {
    vm.blocks->f_update();
}

void f_list() {
    int blk = pop();
    vm.blocks->f_list(blk);
}

void f_thru() {
    int last = pop();
    int first = pop();
    vm.blocks->f_thru(first, last);
}
