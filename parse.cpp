//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "parse.h"
#include "errors.h"
#include <cctype>
#include <cstring>
using namespace std;

void Pad::init() {
	memset(m_pad, BL, PAD_SZ);
}

void Tib::init() {
	memset(m_tib, BL, BUFFER_SZ);
	m_size = m_ptr = 0;
}

void Tib::refill(const char* str, int size) {
	if (size + 1 >= BUFFER_SZ) {
		error(Error::InputBufferOverflow);
		m_size = m_ptr = 0;
	}
	else {
		memcpy(m_tib, str, size);
		m_size = size;
		m_ptr = 0;
	}
	m_tib[m_size] = BL;
}

void Tib::skip_blanks() {
	while (m_ptr < m_size &&
		isspace(static_cast<unsigned char>(m_tib[m_ptr]))) {
		++m_ptr;
	}
}

void Tib::skip_blank() {
	if (m_ptr < m_size &&
		isspace(static_cast<unsigned char>(m_tib[m_ptr]))) {
		++m_ptr;
	}
}

int Tib::skip_to_delimiter(char delimiter) {
	int end = m_size;
	if (delimiter == BL) {
		while (m_ptr < m_size &&
			!isspace(static_cast<unsigned char>(m_tib[m_ptr])))
			++m_ptr;
		end = m_ptr;
	}
	else {
		while (m_ptr < m_size && m_tib[m_ptr] != delimiter)
			++m_ptr;
		end = m_ptr;
		if (m_ptr < m_size && m_tib[m_ptr] == delimiter)
			++m_ptr;		// skip delimiter
	}
	return end;
}

void Wordbuf::init() {
	memset(m_buffer, BL, BUFFER_SZ);
	m_ptr = 0;
}

CountedString* Wordbuf::append(const char* str, int size) {
	if (size > MAX_WORD_SZ) {
		error(Error::ParsedStringOverflow);
		return append("", 0);
	}
	else {
		if (m_ptr + 1 + size + 1 >= BUFFER_SZ)
			m_ptr = 0;
		CountedString* ret = reinterpret_cast<CountedString*>(m_buffer + m_ptr);
		m_buffer[m_ptr++] = size & MAX_WORD_SZ; // size
		memcpy(m_buffer + m_ptr, str, size);
		m_ptr += size;
		m_buffer[m_ptr++] = BL;		// blank after word, not included in size
		return ret;
	}
}

