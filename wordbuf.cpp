//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "wordbuf.h"
#include <string>
using namespace std;

string CountedString::to_string() const {
	return string(str, str + size);
}

CountedString* Wordbuf::append(const string& str) {
	return append(str.c_str(), static_cast<int>(str.size()));
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
