//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "block.h"
#include "interp.h"
#include "io.h"
#include "stack.h"
#include <string.h>
#include <stdio.h>

static const char* block_filename(int blk) {
    static char buffer[13];
    sprintf(buffer, "scr%05d.blk", blk);
    return buffer;
}

static void init_block_buff(Buffer* buff, int blk) {
    input->last_block = buff;
    buff->file = NULL;
    buff->blk = blk;
    buff->dirty = false;
    buff->nr_in = BUFFER_SZ;
    buff->to_in = 0;
    memset(buff->tib, BL, BUFFER_SZ);
}

static bool read_block_buf(Buffer* buff, int blk) {
    init_block_buff(buff, blk);
    const char* filename = block_filename(blk);
    FILE* fp = fopen(filename, "rb");
    if (fp) {
        buff->nr_in = fread(buff->tib, 1, BUFFER_SZ, fp);
        fclose(fp);
        return true;
    }
    else
        return false;
}

static void save_block_buff(Buffer* buff) {
    input->last_block = buff;
    if (buff->blk != 0 && buff->dirty) {
        const char* filename = block_filename(buff->blk);
        FILE* fp = fopen(filename, "wb");
        if (fp) {
            fwrite(buff->tib, 1, BUFFER_SZ, fp);
            fclose(fp);
        }
    }
}

static void flush_block_buff(Buffer* buff) {
    save_block_buff(buff);
    init_block_buff(buff, 0);
}

static Buffer* find_block_buff(int blk) {
    int i = input->block_p;
    for (int j = 0; j < MAX_BLOCKS; j++) {
        i = (i + 1) % MAX_BLOCKS;
        if (input->block[i].blk == blk) {
            input->block_p = i;
            input->last_block = &input->block[i];
            return input->last_block;
        }
    }
    return NULL;
}

static Buffer* find_free_block(void) {
    Buffer* buff = find_block_buff(0);
    if (!buff) {
        input->block_p = (input->block_p + 1) % MAX_BLOCKS;
        buff = &input->block[input->block_p];
    }
    flush_block_buff(buff);
    return buff;
}

Buffer* get_block(int blk) {
    Buffer* buff = find_block_buff(blk);
    if (buff)
        return buff;
    buff = find_free_block();
    buff->blk = blk;
    return buff;
}

bool get_file_block(int blk) {
    Buffer* buff = find_block_buff(blk);
    if (!buff) {
        buff = find_free_block();
        if (read_block_buf(buff, blk))
            return true;
    }
    return false;
}

int f_block(int blk) {
    Buffer* buff = find_block_buff(blk);
    if (!buff) {
        buff = find_free_block();
        read_block_buf(buff, blk);
    }
    return buff->tib - mem;
}

int f_buffer(int blk) {
    Buffer* buff = find_block_buff(blk);
    if (!buff)
        buff = find_free_block();
    buff->blk = blk;
    return buff->tib - mem;
}

void f_save_buffers(void) {
    for (int i = 0; i < MAX_BLOCKS; i++)
        save_block_buff(&input->block[i]);
}

void f_empty_buffers(void) {
    for (int i = 0; i < MAX_BLOCKS; i++)
        init_block_buff(&input->block[i], 0);
}

void f_flush(void) {
    for (int i = 0; i < MAX_BLOCKS; i++)
        flush_block_buff(&input->block[i]);
}

void f_update(void) {
    input->last_block->dirty = true;
}

void f_list(int blk) {
    user->scr = blk;
    f_block(blk);
    Buffer* buff = find_block_buff(blk);
    for (int row = 0; row < BLOCK_ROWS; row++) {
        printf("%2d  %.*s\n", row + 1,
               BLOCK_COLS, buff->tib + row * BLOCK_COLS);
    }
}

void f_thru(void) {
    int last = pop();
    int first = pop();
    for (int blk = first; blk <= last; blk++)
        f_load(blk);
}
