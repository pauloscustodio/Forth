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

	int tell();
	void seek(int fpos);

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

	void push_remaining_buffer();

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

};

bool f_refill();
void f_accept();
void f_expect();
void f_key();
void f_query();
void f_save_input();
void f_restore_input();
