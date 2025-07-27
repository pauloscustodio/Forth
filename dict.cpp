//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "dict.h"
#include "errors.h"
#include "forth.h"
#include "math.h"
#include "parser.h"
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
	int name_addr = alloc_name(name, size);
	return create_cont(name_addr, flags, code);
}

int Dict::create(const ForthString* name, int flags, int code) {
	align();
	int name_addr = alloc_name(name);
	return create_cont(name_addr, flags, code);
}

int Dict::parse_create(int code) {
	const ForthString* name = f_parse_word(BL);
	if (name->size() == 0)
		error(Error::AttemptToUseZeroLengthStringAsName);
	return create(name, 0, code);
}

void Dict::allot(int size) {
	check_free_space(size);
	m_here += size;
}

int Dict::unused() const {
	return (m_names - m_here) / CELL_SZ;
}

int Dict::alloc_name(const char* name) {
	return alloc_name(name, strlen(name));
}

int Dict::alloc_name(const char* name, size_t size) {
	return alloc_name(name, static_cast<int>(size));
}

int Dict::alloc_name(const char* name, int size) {
	ForthString* name_str = vm.wordbuf->append(name, size);
	return alloc_name(name_str);
}

int Dict::alloc_name(const ForthString* name) {
	int alloc_size = name->alloc_size(name->size());

	check_free_space(alloc_size);

	m_names -= alloc_size;
	memcpy(mem_char_ptr(m_names), name, alloc_size);

	return m_names;
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
		ptr = header->link;
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
		ptr = header->link;
	}

	return nullptr;
}

void Dict::check_free_space(int size) const {
	if (m_here + size >= m_names)
		error(Error::DictionaryOverflow);
}

int Dict::create_cont(int name_addr, int flags, int code) {
	// store header
	check_free_space(sizeof(Header));
	Header* header = reinterpret_cast<Header*>(mem_char_ptr(m_here));
	header->link = m_latest;
	m_latest = m_here;

	header->name_addr = name_addr;

	header->flags.smudge = (flags & F_SMUDGE) ? true : false;
	header->flags.hidden = (flags & F_HIDDEN) ? true : false;
	header->flags.immediate = (flags & F_IMMEDIATE) ? true : false;

	header->code = code;

	m_here += aligned(sizeof(Header));
	return header->xt(); // return xt of word
}

void f_find(int addr) {
	CountedString* word = reinterpret_cast<CountedString*>(mem_char_ptr(addr));
	Header* header = vm.dict->find_word(word->str(), word->size());
	if (header == nullptr) {
		push(addr);
		push(0);
	}
	else {
		int xt = header->xt();
		if (header->flags.immediate) {
			push(xt);
			push(1);
		}
		else {
			push(xt);
			push(-1);
		}
	}
}

void f_create() {
	vm.dict->parse_create(idXDOVAR);
}

void f_marker() {
	int save_latest = vm.dict->latest();
	int save_here = vm.dict->here();
	int save_names = vm.dict->names();

	vm.dict->parse_create(idXMARKER);

	comma(save_latest);
	comma(save_here);
	comma(save_names);
}

void f_xmarker(int body) {
	int save_latest = fetch(body);
	int save_here = fetch(body + CELL_SZ);
	int save_names = fetch(body + 2 * CELL_SZ);
	
	vm.dict->set_latest(save_latest);
	vm.dict->set_here(save_here);
	vm.dict->set_names(save_names);
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

