//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "dict.h"
#include "errors.h"
#include "forth.h"
#include "parser.h"
#include "vm.h"
#include <cassert>
#include <cstring>
using namespace std;

CString* Header::name() const {
	CString* name = reinterpret_cast<CString*>(mem_char_ptr(name_addr));
	return name;
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


void Dict::init(int lo_mem, int hi_mem) {
	lo_mem_ = lo_mem;
	hi_mem_ = hi_mem;
	clear();
	check_free_space();
	create_dictionary();
}

void Dict::clear() {
	here_ = lo_mem_;
	names_ = hi_mem_;
	latest_ = 0;
}

void Dict::allot(int size) {
	check_free_space(size);
	here_ += size;
}

int Dict::unused() const {
	return names_ - here_;
}

int Dict::parse_create(int code, int flags) {
	const CString* name = parse_cword(BL);
	if (name->size() == 0)
		error(Error::AttemptToUseZeroLengthStringAsName);
	return create(name, flags, code);
}

int Dict::create(const string& name, int flags, int code) {
	return create(name.c_str(), name.size(), flags, code);
}

int Dict::create(const char* name, size_t size, int flags, int code) {
	return create(name, static_cast<int>(size), flags, code);
}

int Dict::create(const char* name, int size, int flags, int code) {
	align();
	int name_addr = alloc_cstring(name, size);
	return create_cont(name_addr, flags, code);
}

int Dict::create(const CString* name, int flags, int code) {
	if (name->size() > MAX_NAME_SZ)
		error(Error::DefinitionNameTooLong, name->to_string());

	align();
	int name_addr = alloc_cstring(name);
	return create_cont(name_addr, flags, code);
}

int Dict::create_cont(int name_addr, int flags, int code) {
	// store header
	check_free_space(sizeof(Header));
	Header* header = reinterpret_cast<Header*>(mem_char_ptr(here_));
	header->link = latest_;
	latest_ = here_;

	header->name_addr = name_addr;

	header->flags.smudge = (flags & F_SMUDGE) ? true : false;
	header->flags.hidden = (flags & F_HIDDEN) ? true : false;
	header->flags.immediate = (flags & F_IMMEDIATE) ? true : false;

	header->does = 0;
	header->code = code;

	here_ += aligned(sizeof(Header));
	return header->xt(); // return xt of word
}

int Dict::alloc_cstring(const string& str) {
	return alloc_cstring(str.c_str(), str.size());
}

int Dict::alloc_cstring(const char* str, size_t size) {
	return alloc_cstring(str, static_cast<int>(size));
}

int Dict::alloc_cstring(const char* str, int size) {
	CString* str_str = vm.wordbuf->append_cstring(str, size);
	return alloc_cstring(str_str);
}

int Dict::alloc_cstring(const CString* str) {
	int alloc_size = CString::alloc_size(str->size());

	check_free_space(alloc_size);

	names_ -= alloc_size;
	memcpy(mem_char_ptr(names_), str, alloc_size);

	return names_;
}

int Dict::alloc_string(const string& str) {
	return alloc_string(str.c_str(), str.size());
}

int Dict::alloc_string(const char* str, size_t size) {
	return alloc_string(str, static_cast<int>(size));
}

int Dict::alloc_string(const char* str, int size) {
	int alloc_size = LongString::alloc_size(size);

	check_free_space(alloc_size);

	names_ -= alloc_size;
	LongString* lstring = reinterpret_cast<LongString*>(mem_char_ptr(names_));
	lstring->set_string(str, size);

	return names_;
}

int Dict::alloc_string(const LongString* str) {
	return alloc_string(str->str(), str->size());
}

void Dict::ccomma(int value) {
	check_free_space(CHAR_SZ);
	cstore(here_++, value);
}

void Dict::comma(int value) {
	check_free_space(CELL_SZ);
	store(here_, value);
	here_ += CELL_SZ;
}

void Dict::dcomma(dint value) {
	check_free_space(DCELL_SZ);
	dstore(here_, value);
	here_ += DCELL_SZ;
}

void Dict::align() {
	here_ = aligned(here_);
}

Header* Dict::parse_find_word() {
	const CString* name = parse_cword(BL);
	if (name->size() == 0)
		error(Error::AttemptToUseZeroLengthStringAsName);
	return find_word(name);
}

Header* Dict::parse_find_existing_word() {
	const CString* name = parse_cword(BL);
	if (name->size() == 0)
		error(Error::AttemptToUseZeroLengthStringAsName);
	Header* header = find_word(name);
	if (header == nullptr)
		error(Error::UndefinedWord, name->to_string());
	return header;
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
		CString* found_name = header->name();
		if (header->flags.hidden || header->flags.smudge)
			; // skip hidden and smudged words
		else if (header->name()->size() == 0)
			; // skip :NONAME
		else if (header->name()->size() != size)
			; // skip words with different name size
		else if (case_insensitive_equal(name, size, found_name->str(), found_name->size())) {
			return header;
		}
		ptr = header->link;
	}

	return nullptr;
}

Header* Dict::find_word(const CString* name) const {
	return find_word(name->str(), name->size());
}

vector<string> Dict::get_words() const {
	vector<string> words;
	int ptr = vm.dict->latest();
	while (ptr != 0) {
		Header* header = reinterpret_cast<Header*>(mem_char_ptr(ptr));
		CString* found_name = header->name();
		if (header->flags.hidden || header->flags.smudge)
			; // skip hidden and smudged words
		else
			words.push_back(found_name->to_string());

		ptr = header->link;
	}
	return words;
}

void Dict::check_free_space(int size) const {
	if (here_ + size >= names_)
		error(Error::DictionaryOverflow);
}

void f_find(int addr) {
	CString* word = reinterpret_cast<CString*>(mem_char_ptr(addr));
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
	Header* header = vm.dict->parse_find_existing_word();
    assert(header != nullptr);
	if (header->flags.immediate) {
		// compile imediate words immediately
        comma(header->xt());	
	}
	else {
		// make code to compile worder later
		comma(xtXLITERAL);		// xt of word in stack
		comma(header->xt());
		comma(xtCOMMA);
	}
}

void f_bracket_compile() {
	int xt = f_tick();
	comma(xt);
}

void f_compile_comma() {
	comma(xtCOMMA);
}

void f_immediate() {
	Header* header = reinterpret_cast<Header*>(mem_char_ptr(vm.dict->latest()));
	header->flags.immediate = true;
}

void f_hidden() {
	Header* header = reinterpret_cast<Header*>(mem_char_ptr(vm.dict->latest()));
	header->flags.hidden = true;
}

void f_create() {
	vm.dict->parse_create(idXDOVAR, 0);
}

void f_buffer_colon() {
	vm.dict->parse_create(idXDOVAR, 0);
	int size = pop();
	vm.dict->allot(size);
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

void f_two_value() {
	vm.dict->parse_create(idXDO2CONST, 0);
	dcomma(dpop());
}

void f_to() {
	Header* header = vm.dict->parse_find_existing_word();
	assert(header != nullptr);
	int code = fetch(header->xt());
	if (code == idXDOCONST) {			// single cell value
		if (vm.user->STATE == STATE_COMPILE) {
			comma(xtXLITERAL);
			comma(header->body());
			comma(xtSTORE);
		}
		else
			store(header->body(), pop());
	}
	else if (code == idXDO2CONST) {		// double cell value
		if (vm.user->STATE == STATE_COMPILE) {
			comma(xtXLITERAL);
			comma(header->body());
			comma(xtTWO_STORE);
		}
		else
			dstore(header->body(), dpop());
	}
	else {
		error(Error::InvalidNameArgument);
	}
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
	int run_code = fetch(vm.ip);
	vm.ip += CELL_SZ;							// start of runtime code

	Header* def_word = reinterpret_cast<Header*>(mem_char_ptr(vm.dict->latest()));
	def_word->does = run_code;				// start of DOES> code
	def_word->code = idXDOES_RUN;			// new execution id
}

void f_xdoes_run(int body) {
	push(body);                     // store parameter field on the stack
	r_push(vm.ip);                     // save current ip
	Header* header = Header::header(body - CELL_SZ);
	vm.ip = header->does;				// call code after DOES>
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

void f_defer() {
	vm.dict->parse_create(idXDEFER, 0);
	comma(xtABORT);
}

void f_xdefer(int body) {
	int xt = fetch(body);
	f_execute(xt);
}

void f_defer_fetch() {
	int xt = pop();
	f_defer_fetch(xt);
}

void f_defer_fetch(int xt) {
	int body = xt + CELL_SZ;
	push(fetch(body));
}

void f_defer_store() {
	int xt_self = pop();
	int xt_action = pop();
	int body = xt_self + CELL_SZ;
	store(body, xt_action);
}

void f_action_of() {
	Header* header = vm.dict->parse_find_existing_word();
	assert(header != nullptr);
	if (vm.user->STATE == STATE_INTERPRET) {
		f_defer_fetch(header->xt());
	}
	else {
		comma(xtXLITERAL);
		comma(header->xt());
		comma(xtDEFER_FETCH);
	}
}

void f_is() {
	Header* header = vm.dict->parse_find_existing_word();
	assert(header != nullptr);
	if (vm.user->STATE == STATE_INTERPRET) {
		store(header->body(), pop());
	}
	else {
		comma(xtXLITERAL);
		comma(header->xt());
		comma(xtDEFER_STORE);
	}
}

