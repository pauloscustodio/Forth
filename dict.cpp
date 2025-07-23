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
#include <cstring>
using namespace std;

static char to_lower(char c) {
    return tolower(static_cast<unsigned char>(c));
}

ForthString* Header::name() const {
	char* name_ptr = vm.mem.char_ptr(name_addr);
	ForthString* name = reinterpret_cast<ForthString*>(name_ptr);
	return name;
}

int Header::name_size() const {
	return vm.mem.cfetch(name_addr);
}

int Header::xt() const {
	return vm.mem.addr(reinterpret_cast<const char*>(&this->code));
}

Header* Header::header(int xt) {
	int addr = xt - offsetof(Header, code);
	return reinterpret_cast<Header*>(vm.mem.char_ptr(addr));
}

//-----------------------------------------------------------------------------

void Dict::init(int lo_mem, int hi_mem) {
	m_here = lo_mem;
	m_names = hi_mem;
	m_latest = 0;
	check_free_space();

	//@@BEGIN: WordsCreateDictionary
	xtBASE = create("BASE", 0, idBASE);
	xtSTATE = create("STATE", 0, idSTATE);
	xtDPL = create("DPL", 0, idDPL);
	xtSTORE = create("!", 0, idSTORE);
	xtFETCH = create("@", 0, idFETCH);
	xtC_STORE = create("C!", 0, idC_STORE);
	xtC_FETCH = create("C@", 0, idC_FETCH);
	xtTYPE = create("TYPE", 0, idTYPE);
	xtEMIT = create("EMIT", 0, idEMIT);
	xtCR = create("CR", 0, idCR);
	xtSPACE = create("SPACE", 0, idSPACE);
	xtSPACES = create("SPACES", 0, idSPACES);
	xtLESS_HASH = create("<#", 0, idLESS_HASH);
	xtHASH = create("#", 0, idHASH);
	xtHASH_S = create("#S", 0, idHASH_S);
	xtHOLD = create("HOLD", 0, idHOLD);
	xtSIGN = create("SIGN", 0, idSIGN);
	xtHASH_GREATER = create("#>", 0, idHASH_GREATER);
	xtDOT = create(".", 0, idDOT);
	xtD_DOT = create("D.", 0, idD_DOT);
	xtD_DOT_R = create("D.R", 0, idD_DOT_R);
	xtU_DOT = create("U.", 0, idU_DOT);
	xtDOT_R = create(".R", 0, idDOT_R);
	xtU_DOT_R = create("U.R", 0, idU_DOT_R);
	xtPAD = create("PAD", 0, idPAD);
	xtTHROW = create("THROW", 0, idTHROW);
	xtDROP = create("DROP", 0, idDROP);
	xtDUP = create("DUP", 0, idDUP);
	xtPICK = create("PICK", 0, idPICK);
	xtPLUS = create("+", 0, idPLUS);
	xtENVIRONMENT_Q = create("ENVIRONMENT?", 0, idENVIRONMENT_Q);
	xtCOUNT = create("COUNT", 0, idCOUNT);
	xtS_QUOTE = create("S\"", 0, idS_QUOTE);
	xtDOT_S = create(".S", 0, idDOT_S);
	xtWORD = create("WORD", 0, idWORD);
	xtWORDS = create("WORDS", 0, idWORDS);
	xtFIND = create("FIND", 0, idFIND);
	xtCOMMA = create(",", 0, idCOMMA);
	xtC_COMMA = create("C,", 0, idC_COMMA);
	xtALIGN = create("ALIGN", 0, idALIGN);
	//@@END
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
    memcpy(vm.mem.char_ptr(m_names), name_str, alloc_size);

	// store header
	Header* header = reinterpret_cast<Header*>(vm.mem.char_ptr(m_here));
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

Header* Dict::find_word(const string& name) const {
    return find_word(name.c_str(), name.size());
}

Header* Dict::find_word(const char* name, size_t size) const {
	return find_word(name, static_cast<int>(size));
}

Header* Dict::find_word(const char* name, int size) const {
	int ptr = vm.dict->latest();
	while (ptr != 0) {
		Header* header = reinterpret_cast<Header*>(vm.mem.char_ptr(ptr));
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

void fFIND() {
	int addr = pop();
	CountedString* word = reinterpret_cast<CountedString*>(vm.mem.char_ptr(addr));
	Header* header = vm.dict->find_word(word->str(), word->size());
	if (header == nullptr) {
		push(addr);
		push(0);
	}
	else {
		int xt = header->xt();
		int is_immediate = header->flags.immediate ? F_TRUE : F_FALSE;
		if (is_immediate) {
			push(xt);
			push(1);
		}
		else {
			push(xt);
			push(-1);
		}
	}
}

vector<string> cWORDS() {
	vector<string> words;
	int ptr = vm.dict->latest();
	while (ptr != 0) {
		Header* header = reinterpret_cast<Header*>(vm.mem.char_ptr(ptr));
		ForthString* found_name = header->name();
        if (!header->flags.hidden && !header->flags.smudge) {
			words.push_back(found_name->to_string());
		}
		ptr = header->prev_addr;
	}
	return words;
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

void fWORDS() {
	vector<string> words = cWORDS();
	size_t col = 0;
	for (auto& word : words) {
		if (col + 1 + word.size() >= SCREEN_WIDTH) {
			cout << endl << word;
			col = word.size();
		}
		else if (col == 0) {
			cout << word;
			col += word.size();
		}
		else {
			cout << BL << word;
			col += 1 + word.size();
		}
	}
	cout << endl;
}

void execute(int xt) {
	if (xt < 0) {
		error(Error::InvalidWordId, std::to_string(xt));
    }
	else if (xt < MAX_WORD_ID) {
		switch (xt) {
		//@@BEGIN: WordsIdExecution
		case idBASE: fBASE(); break; // BASE
		case idSTATE: fSTATE(); break; // STATE
		case idDPL: fDPL(); break; // DPL
		case idSTORE: fSTORE(); break; // !
		case idFETCH: fFETCH(); break; // @
		case idC_STORE: fC_STORE(); break; // C!
		case idC_FETCH: fC_FETCH(); break; // C@
		case idTYPE: fTYPE(); break; // TYPE
		case idEMIT: fEMIT(); break; // EMIT
		case idCR: fCR(); break; // CR
		case idSPACE: fSPACE(); break; // SPACE
		case idSPACES: fSPACES(); break; // SPACES
		case idLESS_HASH: fLESS_HASH(); break; // <#
		case idHASH: fHASH(); break; // #
		case idHASH_S: fHASH_S(); break; // #S
		case idHOLD: fHOLD(); break; // HOLD
		case idSIGN: fSIGN(); break; // SIGN
		case idHASH_GREATER: fHASH_GREATER(); break; // #>
		case idDOT: fDOT(); break; // .
		case idD_DOT: fD_DOT(); break; // D.
		case idD_DOT_R: fD_DOT_R(); break; // D.R
		case idU_DOT: fU_DOT(); break; // U.
		case idDOT_R: fDOT_R(); break; // .R
		case idU_DOT_R: fU_DOT_R(); break; // U.R
		case idPAD: fPAD(); break; // PAD
		case idTHROW: fTHROW(); break; // THROW
		case idDROP: fDROP(); break; // DROP
		case idDUP: fDUP(); break; // DUP
		case idPICK: fPICK(); break; // PICK
		case idPLUS: fPLUS(); break; // +
		case idENVIRONMENT_Q: fENVIRONMENT_Q(); break; // ENVIRONMENT?
		case idCOUNT: fCOUNT(); break; // COUNT
		case idS_QUOTE: fS_QUOTE(); break; // S"
		case idDOT_S: fDOT_S(); break; // .S
		case idWORD: fWORD(); break; // WORD
		case idWORDS: fWORDS(); break; // WORDS
		case idFIND: fFIND(); break; // FIND
		case idCOMMA: fCOMMA(); break; // ,
		case idC_COMMA: fC_COMMA(); break; // C,
		case idALIGN: fALIGN(); break; // ALIGN
		//@@END
		default:
			error(Error::InvalidWordId, std::to_string(xt));
		}
	}
	else if (aligned(xt) != xt) {
		error(Error::InvalidWordId, std::to_string(xt));
	}
	else if (xt > vm.dict->here()) {
		error(Error::InvalidWordId, std::to_string(xt));
	}
	else {
		// TODO: execute Forth word
		//Header* header = Header::header(xt);
        //ForthString* name = header->name();
	}
}
