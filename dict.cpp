//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "dict.h"
#include "errors.h"
#include "vm.h"
#include <cstring>
using namespace std;

string Header::name() const {
	char* name_ptr = vm.mem.char_ptr(name_addr);
	CountedString* name = reinterpret_cast<CountedString*>(name_ptr);
	return name->to_string();
}

int Header::len() const {
	return len_flags & MAX_WORD_SZ;
}

int Header::xt() const {
	return vm.mem.addr(reinterpret_cast<const char*>(&this->f_word));
}

void Dict::init(int lo_mem, int hi_mem) {
	m_lo_mem = m_here = lo_mem;
	m_hi_mem = m_names = hi_mem;
	m_latest = 0;
	check_free_space();

	//@@BEGIN: WordsCreateDictionary
	xtBASE = create("BASE", 0, fBASE);
	xtSTATE = create("STATE", 0, fSTATE);
	xtSTORE = create("!", 0, fSTORE);
	xtFETCH = create("@", 0, fFETCH);
	xtC_STORE = create("C!", 0, fC_STORE);
	xtC_FETCH = create("C@", 0, fC_FETCH);
	xtPAD = create("PAD", 0, fPAD);
	xtDOT = create(".", 0, fDOT);
	xtTHROW = create("THROW", 0, fTHROW);
	xtDROP = create("DROP", 0, fDROP);
	xtDUP = create("DUP", 0, fDUP);
	xtPICK = create("PICK", 0, fPICK);
	xtPLUS = create("+", 0, fPLUS);
	xtENVIRONMENT_Q = create("ENVIRONMENT?", 0, fENVIRONMENT_Q);
	xtTYPE = create("TYPE", 0, fTYPE);
	xtCOUNT = create("COUNT", 0, fCOUNT);
	xtS_QUOTE = create("S\"", 0, fS_QUOTE);
	xtDOT_S = create(".S", 0, fDOT_S);
	xtWORD = create("WORD", 0, fWORD);
	xtWORDS = create("WORDS", 0, fWORDS);
	xtFIND = create("FIND", 0, fFIND);
	//@@END
}

int Dict::create(const char* name, int flags, func_ptr_t f_word) {
	return create(name, static_cast<int>(strlen(name)), flags, f_word);
}

int Dict::create(const char* name, int size, int flags, func_ptr_t f_word) {
	if (size > MAX_WORD_SZ)
		error(Error::ParsedStringOverflow);
	check_free_space(sizeof(Header) + 1 + size + 1);

	// store name
	m_names -= 1 + size + 1;
	char* name_dst = vm.mem.char_ptr(m_names);
	*reinterpret_cast<uchar*>(name_dst) = size;
	memcpy(name_dst + 1, name, size);
	name_dst[1 + size] = BL;			// terminator

	// store header
	Header* header = reinterpret_cast<Header*>(vm.mem.char_ptr(m_here));
	header->prev_addr = m_latest; m_latest = m_here;
	header->name_addr = m_names;
	header->len_flags = (size & MAX_WORD_SZ) | flags;
	header->f_word = f_word;

	m_here += align(sizeof(Header));
	return vm.mem.addr(reinterpret_cast<char*>(&header->f_word));
}

void Dict::check_free_space(int size) const {
	if (m_here + size >= m_names)
		error(Error::DictionaryOverflow);
}

