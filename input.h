//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include "wordbuf.h"
#include <fstream>
#include <fstream>
#include <iostream>
#include <string>
#include <string>
#include <vector>
using namespace std;

//-----------------------------------------------------------------------------

class Pad {
public:
	void init();
	char* pad() { return m_pad; }
private:
	char m_pad[PAD_SZ];
};

//-----------------------------------------------------------------------------

class Tib {
public:
	void init();
	char* tib() { return m_tib; }
	int size() const { return BUFFER_SZ; }
private:
	char m_tib[BUFFER_SZ];
};

//-----------------------------------------------------------------------------

class InputFiles {
public:
	InputFiles();
	virtual ~InputFiles();

	// return SOURCE_ID
	int open_file(const string& filename); 
	int open_file(const char* text, size_t size);
	int open_file(const char* text, int size);

	// read from SOURCE_ID into TIB
	bool refill();

private:
	vector<ifstream*> m_files; // indexed by SOURCE_ID
};

//-----------------------------------------------------------------------------

class Input {
public:
	void push_text(const string& text);
	void push_text(const char* text, size_t size);
	void push_text(const char* text, int size);

	void push_block(int blk);

	void push_cin();
	
	void push_file(const string& filename);
	void push_file(const char* filename, size_t size);
	void push_file(const char* filename, int size);

	void pop_input();

	bool refill();

private:
	struct Buffer {
		string text;
		int to_in{ 0 };
		int nr_in{ 0 };
		int blk{ 0 };
		int source_id{ 0 };
	};

	vector<Buffer> m_buffers;

	void push_remaining_buffer();
};

bool f_refill();

/*
class Buffer {
public:
	void init();

    int blk() const { return m_blk; }
    int source_id() const { return m_source_id; }
    const char* buffer() const { return m_buffer; }
    int buffer_ptr() const { return m_ptr; }
    int buffer_size() const { return m_size; }
    void set_buffer_ptr(int ptr);

	void read_text(const char* text, int size);
	void read_file(int source_id, const char* filename, int size);
	void close_file();
	void read_block(int blk);
	void read_cin();
	bool refill();

private:
	ifstream* m_ifs;			// input stream, if any
	int m_blk;					// block number, if any
	int m_source_id;			// source-id (-1 string, 0 cin, 1..N file)
	char m_buffer[BUFFER_SZ];	// input buffer, in other cases
	int m_size;					// input buffer size
	int m_ptr;					// input buffer index

	void check_error(int error_number);
	void set_buffer(const string& text);
	void set_buffer(const char* text, size_t size);
	void set_buffer(const char* text, int size);

	static string block_filename();
};

//-----------------------------------------------------------------------------

class Input {
public:
	void init();
	void deinit();

	int blk() const;
    int source_id() const;
    const char* buffer() const;
    int buffer_ptr() const;
    int buffer_size() const;
	void set_buffer_ptr(int ptr);

	void push_text(const string& text);
	void push_text(const char* text, size_t size);
	void push_text(const char* text, int size);
	void push_file(const string& filename);
	void push_file(const char* filename, size_t size);
	void push_file(const char* filename, int size);
    void push_block(int blk);
	void push_cin();
    bool has_input() const;
    bool refill();
	void pop_input();

private:
	Buffer m_buffers[MAX_FILES];	// input buffers
	int m_size;						// number of used input buffer
};
*/
