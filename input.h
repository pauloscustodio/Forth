//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include "wordbuf.h"
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

class Buffer {
public:
	void init();

    int blk() const { return m_blk; }
    int source_id() const { return m_source_id; }
    const char* buffer() const { return m_buffer; }
    int buffer_ptr() const { return m_ptr; }
    int buffer_size() const { return m_size; }

	void read_text(const char* text, int size);
	void read_file(int source_id, const char* filename, int size);
	void close_file();
	void read_block(int blk);
	void read_cin();

	CountedString* parse_word(char delimiter = BL);		// nullptr at EOF

private:
	ifstream* m_ifs;				// input stream, if any
	int m_blk;					// block number, if any
	int m_source_id;				// source-id (-1 string, 0 cin, 1..N file)
	char m_buffer[BUFFER_SZ];	// input buffer, in other cases
	int m_size;					// input buffer size
	int m_ptr;					// input buffer index

	void check_error(int error_number);
	bool getline();
	void set_buffer(const string& text);
	void set_buffer(const char* text, int size);
	void skip_blank();
	void skip_blanks();
	int skip_to_delimiter(char delimiter);
	static string block_filename();
};

//-----------------------------------------------------------------------------

class Input {
public:
	void init();
	void deinit();

	int blk() const { return m_buffers[m_size - 1].blk(); }
    int source_id() const { return m_buffers[m_size - 1].source_id(); }
    const char* tib() const { return m_buffers[m_size - 1].buffer(); }
    int tib_ptr() const { return m_buffers[m_size - 1].buffer_ptr(); }
    int tib_size() const { return m_buffers[m_size - 1].buffer_size(); }

	void push_text(const string& text);
	void push_text(const char* text, int size);
	void push_file(const string& filename);
	void push_file(const char* filename, int size);
    void push_block(int blk);
	void push_cin();
	void pop_input();

	CountedString* parse_word(char delimiter = BL);		// nullptr at EOF

private:
	Buffer m_buffers[MAX_FILES];		// input buffers
	int m_size;						// number of used input buffer
};


CountedString* cWORD(char delimiter = BL);
