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

//-----------------------------------------------------------------------------

void Pad::init() {
	memset(m_pad, BL, sizeof(m_pad));
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
	ifstream* ifs = new ifstream(filename, ios::binary);
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

void InputFiles::close_file(int source_id) {
	if (source_id < 1) {
	}
	else if (source_id < static_cast<int>(m_files.size())) {
		if (m_files[source_id] != nullptr) {
			delete m_files[source_id];
			m_files[source_id] = nullptr;
		}
	}
	else {
		error(Error::FileIOException);
	}
}

bool InputFiles::getline(int source_id, string& line) {
	line.clear();
	bool ok = false;

	if (source_id < 0) {
		ok = false;
	}
	else if (source_id == 0) {
		ok = static_cast<bool>(std::getline(cin, line));
	}
	else if (source_id < static_cast<int>(m_files.size())) {
		if (m_files[source_id] != nullptr) {
			ok = static_cast<bool>(std::getline(*m_files[source_id], line));
		}
		else {
			close_file(source_id);
			ok = false;
		}
	}		
	else {
		error(Error::FileIOException);
	}

	if (line.size() > BUFFER_SZ) {
		error(Error::InputBufferOverflow);
	}

	return ok;
}

int InputFiles::get_buffer(int source_id, int size, string& line) {
	line.clear();
	int num_read = 0;

	if (source_id < 1) {
		num_read = 0;
	}
	else if (source_id < static_cast<int>(m_files.size())) {
		if (m_files[source_id] != nullptr) {
			line.resize(size);
			m_files[source_id]->read(line.data(), size);
			num_read = static_cast<int>(m_files[source_id]->gcount());
			line.resize(num_read);
		}
		else {
			num_read = 0;
		}
	}
	else {
		error(Error::FileIOException);
	}

	return num_read;
}

int InputFiles::tellg(int source_id) {
	if (source_id < 1) {
		return 0;
	}
	else if (source_id < static_cast<int>(m_files.size())) {
		if (m_files[source_id] != nullptr) {
			return static_cast<int>(m_files[source_id]->tellg());
		}
		else {
			return 0;
		}
	}
	else {
		error(Error::FileIOException);
		return 0; // not reached
	}
}

void InputFiles::seekg(int source_id, int fpos) {
	if (source_id < 1) {
	}
	else if (source_id < static_cast<int>(m_files.size())) {
		if (m_files[source_id] != nullptr) {
			m_files[source_id]->seekg(fpos);
		}
	}
	else {
		error(Error::FileIOException);
	}
}

void InputFiles::check_error(int error_number) {
	if (error_number != 0) {
		if (error_number == ENOENT)
			error(Error::NonExistentFile);
		else if (error_number == EMFILE || error_number == ENFILE)
			error(Error::TooManyOpenFiles);
		else
			error(Error::FileIOException);
	}
}

//-----------------------------------------------------------------------------

/*
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
*/

void Input::init() {
	m_ptr = -1;
}

void Input::push_text(const string& text) {
	push_text(text.c_str(), text.size());
}

void Input::push_text(const char* text, size_t size) {
	push_text(text, static_cast<int>(size));
}

void Input::push_text(const char* text, int size) {
	if (size > BUFFER_SZ) {
		error(Error::InputBufferOverflow);
	}
	else if (m_ptr + 1 >= MAX_FILES) {
		error(Error::TooManyOpenFiles);
	}
	else {
		m_ptr++;
		memcpy(m_buffers[m_ptr].buffer, text, size);
		m_buffers[m_ptr].to_in = 0;
		m_buffers[m_ptr].nr_in = size;
		m_buffers[m_ptr].blk = 0;
		m_buffers[m_ptr].source_id = -1;
	}
}

void Input::push_block(int blk) {
	if (blk < 1)
		error(Error::InvalidBlockNumber);
	if (m_ptr + 1 >= MAX_FILES)
		error(Error::TooManyOpenFiles);

	string line;
	int source_id = vm.input_files->open_file(BLOCK_FILENAME);
	vm.input_files->seekg(source_id, blk * BUFFER_SZ);
	vm.input_files->get_buffer(source_id, BUFFER_SZ, line);
	vm.input_files->close_file(source_id);

	m_ptr++;
	memcpy(m_buffers[m_ptr].buffer, line.c_str(), line.size());
	m_buffers[m_ptr].to_in = 0;
	m_buffers[m_ptr].nr_in = static_cast<int>(line.size());
	m_buffers[m_ptr].blk = blk;
	m_buffers[m_ptr].source_id = 0;
}

void Input::push_cin() {
	if (m_ptr + 1 >= MAX_FILES)
		error(Error::TooManyOpenFiles);

	m_ptr++;
	m_buffers[m_ptr].to_in = 0;
	m_buffers[m_ptr].nr_in = 0;
	m_buffers[m_ptr].blk = 0;
	m_buffers[m_ptr].source_id = 0;
}

void Input::push_file(const string& filename) {
	if (m_ptr + 1 >= MAX_FILES)
		error(Error::TooManyOpenFiles);

	m_ptr++;
	m_buffers[m_ptr].to_in = 0;
	m_buffers[m_ptr].nr_in = 0;
	m_buffers[m_ptr].blk = 0;
	m_buffers[m_ptr].source_id = vm.input_files->open_file(filename);
}

void Input::push_file(const char* filename, size_t size) {
	push_file(string(filename, filename + size));
}

void Input::push_file(const char* filename, int size) {
	push_file(string(filename, filename + size));
}

void Input::pop_input() {
	if (m_ptr >= 0) {
		if (m_buffers[m_ptr].source_id > 0)
			vm.input_files->close_file(m_buffers[m_ptr].source_id);
		m_ptr--;
	}
}

bool Input::refill_current_buffer() {
	if (m_ptr < 0)
		return false;
	else {
		Buffer* buffer = &m_buffers[m_ptr];
		if (buffer->source_id < 0)		// input from string
			return false;
		else if (buffer->blk > 0)		// input from block
			return false;
		else {							// input from stream
			string line;
			if (vm.input_files->getline(buffer->source_id, line)) {
				memcpy(buffer->buffer, line.c_str(), line.size());
				buffer->to_in = 0;
				buffer->nr_in = static_cast<int>(line.size());
				return true;			// read a new line
			}
			else {
				return false;
			}
		}
	}
}

bool Input::refill() {
	while (m_ptr >= 0) {
		Buffer* buffer = &m_buffers[m_ptr];
		if (buffer->source_id < 0)		// input from string
			return false;
		else if (buffer->blk > 0)		// input from block
			return false;
		else {							// input from stream
			if (refill_current_buffer())
				return true;
			else 
				pop_input();			// get last buffer from stack and continue
		}
	}
	return false;
}

char* Input::source_ptr() {
	if (m_ptr >= 0)
		return m_buffers[m_ptr].buffer;
	else
		return m_empty;
}

int* Input::to_in_ptr() {
	m_empty_to_in = 0;
	if (m_ptr >= 0)
		return &m_buffers[m_ptr].to_in;
	else
		return &m_empty_to_in;
}

int Input::to_in() const {
	if (m_ptr >= 0)
		return m_buffers[m_ptr].to_in;
	else
		return 0;
}

int* Input::nr_in_ptr() {
	m_empty_nr_in = 0;
	if (m_ptr >= 0)
		return &m_buffers[m_ptr].nr_in;
	else
		return &m_empty_nr_in;
}

int Input::nr_in() const {
	if (m_ptr >= 0)
		return m_buffers[m_ptr].nr_in;
	else
		return 0;
}

int* Input::blk_ptr() {
	m_empty_blk = 0;
	if (m_ptr >= 0)
		return &m_buffers[m_ptr].blk;
	else
		return &m_empty_blk;
}

int Input::blk() const {
	if (m_ptr >= 0)
		return m_buffers[m_ptr].blk;
	else
		return 0;
}

int Input::source_id() const {
	if (m_ptr >= 0)
		return m_buffers[m_ptr].source_id;
	else
		return 0;
}

//-----------------------------------------------------------------------------

void f_to_in() {
	push(mem_addr(vm.input->to_in_ptr()));
}

void f_nr_in() {
	push(mem_addr(vm.input->nr_in_ptr()));
}

void f_blk() {
	push(mem_addr(vm.input->blk_ptr()));
}

void f_source_id() {
	push(vm.input->source_id());
}

void f_tib() {
	push(mem_addr(vm.input->source_ptr()));
}

void f_nr_tib() {
	push(vm.input->nr_in());
}

void f_source() {
	f_tib();
	f_nr_tib();
}

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
	vm.input->push_cin();
	vm.input->refill_current_buffer();
	vm.input->pop_input();
}

void f_save_input() {
	vm.input->push_text("");	// source_id=-1, is not popped by refill
}

void f_restore_input() {
	vm.input->pop_input();
	push(F_TRUE);
}
