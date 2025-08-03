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
#include "str.h"
#include "vm.h"
#include <cassert>
#include <cstring>
using namespace std;

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
	return mem_addr(&this->code);
}

int Header::body() const {
	return xt() + CELL_SZ;
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

int Dict::parse_create(int code, int flags) {
	const ForthString* name = parse_word(BL);
	if (name->size() == 0)
		error(Error::AttemptToUseZeroLengthStringAsName);
	return create(name, flags, code);
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

Header* Dict::find_word(const ForthString* name) const {
	return find_word(name->str(), name->size());
}

Header* Dict::parse_find_word() {
	const ForthString* name = parse_word(BL);
	if (name->size() == 0)
		error(Error::AttemptToUseZeroLengthStringAsName);
	return find_word(name);
}

Header* Dict::parse_find_existing_word() {
	const ForthString* name = parse_word(BL);
	if (name->size() == 0)
		error(Error::AttemptToUseZeroLengthStringAsName);
	Header* header = find_word(name);
	if (header == nullptr)
		error(Error::UndefinedWord, name->to_string());
	return header;
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

	header->does = 0;
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

void f_marker() {
	int save_latest = vm.dict->latest();
	int save_here = vm.dict->here();
	int save_names = vm.dict->names();

	vm.dict->parse_create(idXMARKER, 0);

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

void f_words() {
	vector<string> words = vm.dict->get_words();
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

void f_create() {
	vm.dict->parse_create(idXDOVAR, 0);
}

void f_colon() {
	if (vm.cs_stack->depth() > 0)
		error(Error::CompilerNesting);
	else {
		vm.cs_stack->push(idCOLON);
		vm.dict->parse_create(idXDOCOL, F_SMUDGE);
		vm.user->STATE = STATE_COMPILE;
	}
}

void f_colon_noname() {
	if (vm.cs_stack->depth() > 0)
		error(Error::CompilerNesting);
	else {
		vm.cs_stack->push(idCOLON);
		vm.dict->create("", 0, idXDOCOL);
		Header* header = reinterpret_cast<Header*>(mem_char_ptr(vm.dict->latest()));
		header->flags.smudge = true;
		vm.user->STATE = STATE_COMPILE;
		push(header->xt());
	}
}

void f_semicolon() {
	if (vm.cs_stack->depth() != 1 || vm.cs_stack->peek() != idCOLON)
		error(Error::CompilerNesting);
	else {
		vm.cs_stack->pop();
		comma(xtEXIT);
		Header* header = reinterpret_cast<Header*>(mem_char_ptr(vm.dict->latest()));
		header->flags.smudge = false;
		vm.user->STATE = STATE_INTERPRET;
	}
}

void f_variable() {
	vm.dict->parse_create(idXDOVAR, 0);
	comma(0);
}

void f_2variable() {
	vm.dict->parse_create(idXDOVAR, 0);
	dcomma(0);
}

void f_value() {
	vm.dict->parse_create(idXDOCONST, 0);
	comma(pop());
}

void f_to() {
	Header* header = vm.dict->parse_find_existing_word();
	assert(header != nullptr);
	if (vm.user->STATE == STATE_COMPILE) {
		comma(xtXLITERAL);
		comma(header->body());
		comma(xtSTORE);
	}
	else
		store(header->body(), pop());
}
void f_constant() {
	vm.dict->parse_create(idXDOCONST, 0);
	comma(pop());
}

void f_2constant() {
	vm.dict->parse_create(idXDO2CONST, 0);
	dcomma(dpop());
}

void f_does() {
	comma(xtXDOES_DEFINE);                  // set this word as having DOES>
	comma(vm.dict->here() + 2 * CELL_SZ);	// location of run code
	comma(xtEXIT);                          // exit from CREATE part
	// run code starts here
}

void f_xdoes_define() {
	int run_code = fetch(ip);
	ip += CELL_SZ;							// start of runtime code

	Header* def_word = reinterpret_cast<Header*>(mem_char_ptr(vm.dict->latest()));
	def_word->does = run_code;				// start of DOES> code
	def_word->code = idXDOES_RUN;			// new execution id
}

void f_xdoes_run(int body) {
	push(body);                     // store parameter field on the stack
	r_push(ip);                     // save current ip
	Header* header = Header::header(body - CELL_SZ);
	ip = header->does;				// call code after DOES>
}

int f_tick() {
	Header* header = vm.dict->parse_find_existing_word();
	assert(header != nullptr);
	return header->xt();
}

void f_bracket_tick() {
	int xt = f_tick();
	comma(xtXLITERAL);
	comma(xt);
}

void f_postpone() {
	int xt = f_tick();
	comma(xt);
}

void f_bracket_compile() {
	int xt = f_tick();
	comma(xt);
}

void f_immediate() {
	Header* header = reinterpret_cast<Header*>(mem_char_ptr(vm.dict->latest()));
	header->flags.immediate = true;
}

void f_hidden() {
	Header* header = reinterpret_cast<Header*>(mem_char_ptr(vm.dict->latest()));
	header->flags.hidden = true;
}
