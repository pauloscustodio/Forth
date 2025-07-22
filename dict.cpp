//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "dict.h"
#include "errors.h"
#include "forth.h"
#include "vm.h"
#include <cstring>
using namespace std;

string Header::name() const {
	char* name_ptr = vm.mem.char_ptr(name_addr);
	CountedString* name = reinterpret_cast<CountedString*>(name_ptr);
	return name->to_string();
}

int Header::name_size() const {
	return vm.mem.cfetch(name_addr);
}

int Header::xt() const {
	return vm.mem.addr(reinterpret_cast<const char*>(&this->f_word));
}

Header* Header::header(int xt) {
	int addr = xt - offsetof(Header, f_word);
	return reinterpret_cast<Header*>(vm.mem.char_ptr(addr));
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
	xtTYPE = create("TYPE", 0, fTYPE);
	xtEMIT = create("EMIT", 0, fEMIT);
	xtCR = create("CR", 0, fCR);
	xtSPACE = create("SPACE", 0, fSPACE);
	xtSPACES = create("SPACES", 0, fSPACES);
	xtLESS_HASH = create("<#", 0, fLESS_HASH);
	xtHASH = create("#", 0, fHASH);
	xtHASH_S = create("#S", 0, fHASH_S);
	xtHOLD = create("HOLD", 0, fHOLD);
	xtSIGN = create("SIGN", 0, fSIGN);
	xtHASH_GREATER = create("#>", 0, fHASH_GREATER);
	xtDOT = create(".", 0, fDOT);
	xtD_DOT = create("D.", 0, fD_DOT);
	xtD_DOT_R = create("D.R", 0, fD_DOT_R);
	xtU_DOT = create("U.", 0, fU_DOT);
	xtDOT_R = create(".R", 0, fDOT_R);
	xtU_DOT_R = create("U.R", 0, fU_DOT_R);
	xtPAD = create("PAD", 0, fPAD);
	xtTHROW = create("THROW", 0, fTHROW);
	xtDROP = create("DROP", 0, fDROP);
	xtDUP = create("DUP", 0, fDUP);
	xtPICK = create("PICK", 0, fPICK);
	xtPLUS = create("+", 0, fPLUS);
	xtENVIRONMENT_Q = create("ENVIRONMENT?", 0, fENVIRONMENT_Q);
	xtCOUNT = create("COUNT", 0, fCOUNT);
	xtS_QUOTE = create("S\"", 0, fS_QUOTE);
	xtDOT_S = create(".S", 0, fDOT_S);
	xtWORD = create("WORD", 0, fWORD);
	xtWORDS = create("WORDS", 0, fWORDS);
	xtFIND = create("FIND", 0, fFIND);
	xtCOMMA = create(",", 0, fCOMMA);
	xtC_COMMA = create("C,", 0, fC_COMMA);
	xtALIGN = create("ALIGN", 0, fALIGN);
	//@@END
}

int Dict::create(const char* name, int flags, func_ptr_t f_word) {
	return create(name, static_cast<int>(strlen(name)), flags, f_word);
}

int Dict::create(const char* name, int size, int flags, func_ptr_t f_word) {
	if (size > MAX_WORD_SZ)
		error(Error::ParsedStringOverflow);

	align();
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
	header->flags = flags;
	header->f_does = fVOID;
	header->f_word = f_word;

	m_here += aligned(sizeof(Header));
	return vm.mem.addr(reinterpret_cast<char*>(&header->f_word));
}

void Dict::ccomma(int value) {
	check_free_space(CHAR_SZ);
	vm.mem.cstore(m_here++, value);
}

void Dict::comma(int value) {
	check_free_space(CELL_SZ);
	vm.mem.store(m_here, value);
	m_here += CELL_SZ;
}

void Dict::dcomma(dint value) {
	check_free_space(DCELL_SZ);
	vm.mem.dstore(m_here, value);
	m_here += DCELL_SZ;
}

void Dict::align() {
	m_here = aligned(m_here);
}

void Dict::check_free_space(int size) const {
	if (m_here + size >= m_names)
		error(Error::DictionaryOverflow);
}

void fCOMMA() {
	int value = pop();
	vm.dict->comma(value);
}

void fC_COMMA() {
	int value = pop();
	vm.dict->ccomma(value);
}

void fALIGN() {
	vm.dict->align();
}
