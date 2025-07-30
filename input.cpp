//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "forth.h"
#include "input.h"
#include "vm.h"
#include <cassert>
#include <cerrno>
#include <cstring>
using namespace std;

static void check_error(int error_number) {
	assert(error_number != 0);
	if (error_number == ENOENT)
		error(Error::NonExistentFile);
	else if (error_number == EMFILE || error_number == ENFILE)
		error(Error::TooManyOpenFiles);
	else
		error(Error::FileIOException);
}

//-----------------------------------------------------------------------------

void Pad::init() {
	memset(m_pad, BL, sizeof(m_pad));
}

//-----------------------------------------------------------------------------

void Tib::init() {
	memset(m_tib, BL, sizeof(m_tib));
}

//-----------------------------------------------------------------------------

InputFiles::InputFiles() {
	m_files.push_back(nullptr);		// source_id 0 is for cin
}

InputFiles::~InputFiles() {
	for (auto& file : m_files)
		delete file;
}

int InputFiles::open_file(const string& filename) {
	int source_id = static_cast<int>(m_files.size());
	errno = 0;
	ifstream* ifs = new ifstream(filename);
	int error_number = errno;
	if (!ifs->is_open()) {
		delete ifs;
		ifs = nullptr;
		check_error(error_number);
	}

	m_files.push_back(ifs);
	return source_id;
}

int InputFiles::open_file(const char* text, size_t size) {
	return open_file(string(text, text + size));
}

int InputFiles::open_file(const char* text, int size) {
	return open_file(string(text, text + size));
}

bool InputFiles::refill() {
	string line;
	bool ok = false;
	vm.user->TO_IN = vm.user->NR_IN = 0;

	int source_id = vm.user->SOURCE_ID;
	if (source_id < 0)
		return false;
	else if (source_id == 0)
		ok = static_cast<bool>(std::getline(cin, line));
	else if (source_id < static_cast<int>(m_files.size()) && m_files[source_id] != nullptr)
		ok = static_cast<bool>(std::getline(*m_files[source_id], line));
	else 
		error(Error::FileIOException);

	if (ok) {
		if (static_cast<int>(line.size()) > vm.tib->size()) {
			error(Error::InputBufferOverflow);
		}
		else {
			memcpy(vm.tib->tib(), line.c_str(), line.size());
			vm.user->TO_IN = 0;
			vm.user->NR_IN = static_cast<int>(line.size());
		}
	}
	else {
		delete m_files[source_id]; // release open file
		m_files[source_id] = nullptr;
	}

	return ok;
}

int InputFiles::tell() {
	int source_id = vm.user->SOURCE_ID;
	if (source_id < 1)
		return 0;
	else if (source_id < static_cast<int>(m_files.size()) && m_files[source_id] != nullptr)
		return static_cast<int>(m_files[source_id]->tellg());
	else {
		error(Error::FileIOException);
		return 0;
	}
}

void InputFiles::seek(int fpos) {
	int source_id = vm.user->SOURCE_ID;
	if (source_id < 1)
		return;
	else if (source_id < static_cast<int>(m_files.size()) && m_files[source_id] != nullptr)
		m_files[source_id]->seekg(fpos);
	else
		error(Error::FileIOException);
}

//-----------------------------------------------------------------------------

void Input::push_text(const string& text) {
	push_text(text.c_str(), text.size());
}

void Input::push_text(const char* text, size_t size) {
	push_text(text, static_cast<int>(size));
}

void Input::push_text(const char* text, int size) {
	push_remaining_buffer();
	if (size > BUFFER_SZ)
		error(Error::InputBufferOverflow);
	memcpy(vm.tib->tib(), text, size);
	vm.user->NR_IN = size;
	vm.user->SOURCE_ID = -1;
}

void Input::push_block(int blk) {
	push_remaining_buffer();
	if (blk < 1)
		error(Error::InvalidBlockNumber);
	else {
		errno = 0;
		ifstream ifs(BLOCK_FILENAME, ios::binary);
		int error_number = errno;
		if (!ifs.is_open())
			check_error(error_number);
		else {
			ifs.seekg(blk * BUFFER_SZ);
			ifs.read(vm.tib->tib(), BUFFER_SZ);
			vm.user->TO_IN = 0;
			vm.user->NR_IN = static_cast<int>(ifs.gcount());
			vm.user->BLK = blk;
		}
	}
}

void Input::push_cin() {
	push_remaining_buffer();
	vm.user->SOURCE_ID = 0;
}

void Input::push_file(const string& filename) {
	push_remaining_buffer();
	vm.user->SOURCE_ID = vm.input_files->open_file(filename);
}

void Input::push_file(const char* filename, size_t size) {
	push_file(string(filename, filename + size));
}

void Input::push_file(const char* filename, int size) {
	push_file(string(filename, filename + size));
}

void Input::pop_input() {
	if (!m_buffers.empty()) {
		Buffer buffer = m_buffers.back();
		m_buffers.pop_back();

		memcpy(vm.tib->tib(), buffer.text.c_str(), buffer.text.size());
		vm.user->TO_IN = buffer.to_in;
		vm.user->NR_IN = buffer.nr_in;
		vm.user->BLK = buffer.blk;
		vm.user->SOURCE_ID = buffer.source_id;
	}
	else {
		vm.user->TO_IN = vm.user->NR_IN = 0;
		vm.user->BLK = 0;
		vm.user->SOURCE_ID = 0;
	}
}

bool Input::refill() {
	push_remaining_buffer();
	while (true) {
		int source_id = vm.user->SOURCE_ID;
		int blk = vm.user->BLK;

		if (source_id < 0)				// input from string
			return false;
		else if (blk > 0)				// input from block
			return false;
		else {							// input from stream
			if (vm.input_files->refill())
				return true;			// read a new line
			else {
				if (m_buffers.empty())
					return false;		// no more input
				else
					pop_input();		// get last buffer from stack and continue
			}
		}
	}
}

void Input::push_remaining_buffer() {
	if (vm.user->TO_IN < vm.user->NR_IN) {
		Buffer buffer;
		buffer.text = string(vm.tib->tib(), vm.tib->tib() + vm.user->NR_IN);
		buffer.to_in = vm.user->TO_IN;
		buffer.nr_in = vm.user->NR_IN;
		buffer.blk = vm.user->BLK;
		buffer.source_id = vm.user->SOURCE_ID;
		m_buffers.push_back(buffer);

		vm.user->TO_IN = vm.user->NR_IN = 0;
		vm.user->BLK = 0;
		vm.user->SOURCE_ID = 0;
	}
}

//-----------------------------------------------------------------------------

bool f_refill() {
	return vm.input->refill();
}

void f_accept() {
	int max_size = pop();
	int addr = pop();
	char* text = mem_char_ptr(addr);

	string line;
	if (std::getline(cin, line)) {
		if (static_cast<int>(line.size()) > max_size)
			line.resize(max_size);

		while (!line.empty() && line.back() == '\n')
			line.pop_back();					// remove newline

		memcpy(text, line.c_str(), line.size());

		push(static_cast<int>(line.size()));	// length received
	}
	else {
		push(0);								// zero length
	}
}

void f_expect() {
	f_accept();
	vm.user->SPAN = pop();
}

void f_key() {
	char c;
	cin >> c;
	push(c);
}

void f_query() {
	int old_source_id = vm.user->SOURCE_ID;    // save in case it's -1
	vm.input->push_cin();
	f_refill();
	vm.input->pop_input();
	vm.user->SOURCE_ID = old_source_id;
}

enum { INPUT_STR, INPUT_KBD, INPUT_FILE };

void f_save_input() {
	if (vm.user->SOURCE_ID < 0) {
		push(INPUT_STR);
		vm.input->push_remaining_buffer();
	}
	else if (vm.user->SOURCE_ID == 0) {
		push(INPUT_KBD);
		vm.input->push_remaining_buffer();
	}
	else {
		push(vm.input_files->tell());
		push(INPUT_FILE);
		vm.input->push_remaining_buffer();
	}
}

void f_restore_input() {
	switch (pop()) {
	case INPUT_STR:
		vm.input->pop_input();
		push(F_TRUE);
		break;

	case INPUT_KBD:
		vm.input->pop_input();
		push(F_TRUE);
		break;

	case INPUT_FILE:
		vm.input->pop_input();
		vm.input_files->seek(pop());
		push(F_TRUE);
		break;

	default:
		push(F_FALSE);
	}
}
