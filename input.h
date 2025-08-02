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

class InputFiles {
public:
	InputFiles();
	virtual ~InputFiles();

	// return source_id
	int open_file(const string& filename); 
	int open_file(const char* text, size_t size);
	int open_file(const char* text, int size);

	void close_file(int source_id);

	// read text
	bool getline(int source_id, string& line);
	int get_buffer(int source_id, int size, string& line);

	int tellg(int source_id);
	void seekg(int source_id, int fpos);

private:
	vector<ifstream*> m_files; // indexed by SOURCE_ID

	static void check_error(int error_number);
};

//-----------------------------------------------------------------------------

class Input {
public:
	void init();

	bool has_input() const;
	int input_level() const;

	void push_text(const string& text);
	void push_text(const char* text, size_t size);
	void push_text(const char* text, int size);

	bool push_block(int blk);

	void push_cin();
	void push_cin_single_line();
	bool is_single_line_buffer() const;

	void push_file(const string& filename);
	void push_file(const char* filename, size_t size);
	void push_file(const char* filename, int size);

	void pop_input();

	bool refill();

	char* source_ptr();
	int* to_in_ptr();
	int to_in() const;
	int* nr_in_ptr();
	int nr_in() const;
	int* blk_ptr();
	int blk() const;
	int source_id() const;

private:
	struct Buffer {
		char buffer[BUFFER_SZ];
		int to_in;
		int nr_in;
		int blk;
		int source_id;
		int max_read_lines;
	};

	Buffer m_buffers[MAX_FILES];
	int m_ptr;
	char m_empty[1];
	int m_empty_to_in;
	int m_empty_nr_in;
	int m_empty_blk;
	
	void _push_cin(int max_read_lines);
};

void f_to_in();
void f_nr_in();
void f_blk();
void f_source_id();
void f_tib();
void f_source();
bool f_refill();
void f_accept();
void f_expect();
void f_key();
void f_query();
void f_save_input();
void f_restore_input();
