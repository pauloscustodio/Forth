//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "dict.h"
#include "errors.h"
#include "forth.h"
#include "math.h"
#include "vm.h"
#include <cassert>
#include <cstring>
using namespace std;

static char to_lower(char c) {
    return tolower(static_cast<unsigned char>(c));
}

ForthString* Header::name() const {
	char* name_ptr = mem_char_ptr(name_addr);
	ForthString* name = reinterpret_cast<ForthString*>(name_ptr);
	return name;
}

int Header::name_size() const {
	ForthString* word_name = name();
	return word_name->size();
}

int Header::xt() const {
	return mem_addr(reinterpret_cast<const char*>(&this->code));
}

Header* Header::header(int xt) {
	int addr = xt - offsetof(Header, code);
	return reinterpret_cast<Header*>(mem_char_ptr(addr));
}

//-----------------------------------------------------------------------------

void Dict::init(int lo_mem, int hi_mem) {
	m_here = lo_mem;
	m_names = hi_mem;
	m_latest = 0;
	check_free_space();
	create_dictionary();
}

int Dict::create(const char* name, int flags, int code) {
	return create(name, strlen(name), flags, code);
}

int Dict::create(const char* name, size_t size, int flags, int code) {
	return create(name, static_cast<int>(size), flags, code);
}

int Dict::create(const char* name, int size, int flags, int code) {
	align();

	// store name
    ForthString* name_str = vm.wordbuf->append(name, size);
    int alloc_size = name_str->alloc_size(size);

	check_free_space(sizeof(Header) + alloc_size);

	m_names -= alloc_size;
    memcpy(mem_char_ptr(m_names), name_str, alloc_size);

	// store header
	Header* header = reinterpret_cast<Header*>(mem_char_ptr(m_here));
	header->prev_addr = m_latest; 
	m_latest = m_here;

	header->name_addr = m_names;

    header->flags.smudge = (flags & F_SMUDGE) ? true : false;
    header->flags.hidden = (flags & F_HIDDEN) ? true : false;
    header->flags.immediate = (flags & F_IMMEDIATE) ? true : false;

	header->code = code;

	m_here += aligned(sizeof(Header));
    return header->xt(); // return xt of word
}

vector<string> Dict::get_words() const {
	vector<string> words;
	int ptr = vm.dict->latest();
	while (ptr != 0) {
		Header* header = reinterpret_cast<Header*>(mem_char_ptr(ptr));
		ForthString* found_name = header->name();
		if (!header->flags.hidden && !header->flags.smudge) {
			words.push_back(found_name->to_string());
		}
		ptr = header->prev_addr;
	}
	return words;
}

void Dict::ccomma(int value) {
	check_free_space(CHAR_SZ);
	cstore(m_here++, value);
}

void Dict::comma(int value) {
	check_free_space(CELL_SZ);
	store(m_here, value);
	m_here += CELL_SZ;
}

void Dict::dcomma(dint value) {
	check_free_space(DCELL_SZ);
	dstore(m_here, value);
	m_here += DCELL_SZ;
}

void Dict::align() {
	m_here = aligned(m_here);
}

Header* Dict::find_word(const string& name) const {
    return find_word(name.c_str(), name.size());
}

Header* Dict::find_word(const char* name, size_t size) const {
	return find_word(name, static_cast<int>(size));
}

Header* Dict::find_word(const char* name, int size) const {
	int ptr = vm.dict->latest();
	while (ptr != 0) {
		Header* header = reinterpret_cast<Header*>(mem_char_ptr(ptr));
		ForthString* found_name = header->name();
		if (header->flags.hidden || header->flags.smudge)
			; // skip hidden or smudged words
		else if (size != header->name_size())
            ; // skip words with different name size
		else if (case_insensitive_equal(name, size, found_name->str(), found_name->size())) {
			return header;
		}
		ptr = header->prev_addr;
	}

	return nullptr;
}

void Dict::check_free_space(int size) const {
	if (m_here + size >= m_names)
		error(Error::DictionaryOverflow);
}

bool case_insensitive_equal(const char* a_str, int a_size, const char* b_str, int b_size) {
	if (a_size != b_size)
		return false;
	for (int i = 0; i < a_size; ++i) {
		if (to_lower(a_str[i]) != to_lower(b_str[i]))
			return false;
	}
	return true;
}

