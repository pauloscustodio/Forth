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

// Forth ignores all control characters
static bool is_space(char c) {
	return c >= 0 && c <= BL;
}

void Buffer::init() {
	m_ifs = nullptr;
	m_blk = 0;
	m_source_id = 0;
	m_size = 0;
	m_ptr = 0;
}

void Buffer::read_text(const char* text, int size) {
	close_file();
	init();
	m_source_id = -1;
	set_buffer(text, size);
}

void Buffer::read_file(int source_id, const char* filename_, int size) {
	assert(source_id > 0);

	string filename(filename_, filename_ + size);
	close_file();
	init();

	m_source_id = source_id;

	errno = 0;
	m_ifs = new ifstream(filename);
	int error_number = errno;
	if (!m_ifs->is_open()) {
		delete m_ifs; m_ifs = nullptr;
		check_error(error_number);
	}
}

void Buffer::read_block(int blk) {
	close_file();
	init();
	if (blk < 1)
		error(Error::InvalidBlockNumber);
	m_blk = blk;

	errno = 0;
	ifstream ifs(BLOCK_FILENAME, ios::binary);
	int error_number = errno;
	if (!ifs.is_open())
		check_error(error_number);
	else {
		ifs.seekg(blk * BUFFER_SZ);
		ifs.read(m_buffer, BUFFER_SZ);
		m_size = static_cast<int>(ifs.gcount());
	}
}

void Buffer::read_cin() {
	close_file();
	init();
	m_source_id = 0;
}

void Buffer::close_file() {
	if (m_ifs != nullptr)
		delete m_ifs;
	m_ifs = nullptr;
}

void Buffer::check_error(int error_number) {
	if (error_number == 0)
		return;
	else if (error_number == ENOENT)
		error(Error::NonExistentFile);
	else if (error_number == EMFILE || error_number == ENFILE)
		error(Error::TooManyOpenFiles);
	else
		error(Error::FileIOException);
}

void Buffer::skip_blank() {
	if (m_ptr < m_size &&
		is_space(m_buffer[m_ptr])) {
		++m_ptr;
	}
}

void Buffer::skip_blanks() {
	while (m_ptr < m_size &&
		is_space(m_buffer[m_ptr])) {
		++m_ptr;
	}
}

int Buffer::skip_to_delimiter(char delimiter) {
	int end = m_size;
	if (delimiter == BL) {
		while (m_ptr < m_size &&
			!is_space(m_buffer[m_ptr]))
			++m_ptr;
		end = m_ptr;
	}
	else {
		while (m_ptr < m_size && m_buffer[m_ptr] != delimiter)
			++m_ptr;
		end = m_ptr;
		if (m_ptr < m_size && m_buffer[m_ptr] == delimiter)
			++m_ptr;		// skip delimiter
	}
    return end;	// end of word, char before delimiter
}

CountedString* Buffer::parse_word(char delimiter) {
	while (true) {
		if (m_ptr >= m_size) {		// end of buffer
			if (!getline())			// try to read next line
				return nullptr;		// EOF
			else
				continue;			// try again
		}

		if (delimiter == BL)
			skip_blanks();	// skip blanks before word
		else
			skip_blank();	// skip space after quote

		int start = m_ptr;
		int end = skip_to_delimiter(delimiter);

		if (end > start) {
			int size = end - start;
			CountedString* ret = vm.wordbuf->append(m_buffer + start, size);
			return ret;
		}
	}
}

bool Buffer::getline() {
	if (m_source_id < 0)				// input from string
		return false;
	else if (m_blk > 0)				// input from block
		return false;
	else {							// input from stream
		string line;
		bool ok = true;
		if (m_source_id == 0) {		// input from stdin
			if (!std::getline(cin, line))
				ok = false;
		}
		else {						// input from file
			if (!std::getline(*m_ifs, line))
				ok = false;
		}

		if (line.size() > BUFFER_SZ)
			error(Error::InputBufferOverflow);

		if (ok)
			set_buffer(line);
		else
			set_buffer("");

		return ok;
	}
}

void Buffer::set_buffer(const string& text) {
	set_buffer(text.c_str(), static_cast<int>(text.size()));
}

void Buffer::set_buffer(const char* text, int size) {
	if (size > BUFFER_SZ)
		error(Error::InputBufferOverflow);

	memcpy(m_buffer, text, size);
	m_ptr = 0;
	m_size = size;
}
	
string Buffer::block_filename() {
	return BLOCK_FILENAME;
}

//-----------------------------------------------------------------------------

void Input::init() {
	for (int i = 0; i < MAX_FILES; ++i) {
		m_buffers[i].init();
    }
    m_size = 0;
}

void Input::deinit() {
	for (int i = 0; i < MAX_FILES; ++i) {
		m_buffers[i].close_file();
	}
}

void Input::push_text(const string& text) {
    push_text(text.c_str(), static_cast<int>(text.size()));
}

void Input::push_text(const char* text, int size) {
	if (m_size >= MAX_FILES) {
		error(Error::TooManyOpenFiles);
    }
	else {
		m_buffers[m_size].read_text(text, size);
		++m_size;
	}
}

void Input::push_file(const string& filename) {
	push_file(filename.c_str(), static_cast<int>(filename.size()));
}

void Input::push_file(const char* filename, int size) {
	if (m_size >= MAX_FILES) {
		error(Error::TooManyOpenFiles);
	}
	else {
        int source_id = m_size + 1;	// source_id starts at 1
		m_buffers[m_size].read_file(source_id, filename, size);
		++m_size;
	}
}

void Input::push_block(int blk) {
	if (m_size >= MAX_FILES) {
		error(Error::TooManyOpenFiles);
	}
	else {
		m_buffers[m_size].read_block(blk);
		++m_size;
	}
}

void Input::push_cin() {
	if (m_size >= MAX_FILES) {
		error(Error::TooManyOpenFiles);
	}
	else {
		m_buffers[m_size].read_cin();
		++m_size;
	}
}

void Input::pop_input() {
	if (m_size > 0) {
		m_buffers[m_size - 1].close_file();
		m_buffers[m_size - 1].init();
		--m_size;		// remove last buffer
	}
}

CountedString* Input::parse_word(char delimiter) {
	while (m_size > 0) {
		CountedString* word = m_buffers[m_size - 1].parse_word(delimiter);
		if (word != nullptr)			// found a word
			return word;
		else if (source_id() < 0)	// input from string
			return nullptr;
		else if (m_size == 0)		// no more buffers
			return nullptr;
		else {
			pop_input();					// remove last buffer
		}
	}
	return nullptr;
}

//-----------------------------------------------------------------------------

CountedString* cWORD(char delimiter) {
	CountedString* word = vm.input->parse_word(delimiter);
	if (word == nullptr)
		exit(EXIT_SUCCESS);		// no more input
	else
		return word;				// valid word
}

void fWORD() {
	char delimiter = pop();
	CountedString* word = cWORD(delimiter);
	assert(word != nullptr);

	push(vm.mem.addr(word->str));			// address of word
	push(word->size);						// length of word
}
