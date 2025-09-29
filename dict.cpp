//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "dict.h"
#include "errors.h"
#include "forth.h"
#include "interp.h"
#include "locals.h"
#include "parser.h"
#include "vm.h"
#include <cassert>
#include <cstring>

CString* Header::name() const {
    CString* name = reinterpret_cast<CString*>(mem_char_ptr(name_addr));
    return name;
}

uint Header::xt() const {
    return mem_addr(&this->code);
}

uint Header::body() const {
    return xt() + CELL_SZ;
}

Header* Header::header(uint xt) {
    uint addr = xt - offsetof(Header, code);
    return reinterpret_cast<Header*>(mem_char_ptr(addr));
}

uint Header::get_size() const {
    if (size == 0) {
        return vm.here - body();
    }
    else {
        return size;
    }
}

void Dict::init() {
    clear();
    check_free_space();
    create_dictionary();
}

void Dict::clear() {
    vm.here = vm.dict_lo_mem;
    vm.names = vm.dict_hi_mem;
    vm.latest = 0;
}

void Dict::allot(int size) {
    check_free_space(size);
    vm.here += size;
}

int Dict::unused() const {
    return vm.names - vm.here;
}

uint Dict::parse_create(uint code, int flags) {
    const CString* name = parse_cword(BL);
    if (name->size() == 0) {
        error(Error::AttemptToUseZeroLengthStringAsName);
    }
    return create(name, flags, code);
}

uint Dict::create(const std::string& name, int flags, uint code) {
    return create(name.c_str(), static_cast<uint>(name.size()), flags, code);
}

uint Dict::create(const char* name, uint size, int flags, uint code) {
    align();
    uint name_addr = alloc_cstring(name, size);
    return create_cont(name_addr, flags, code);
}

uint Dict::create(const CString* name, int flags, uint code) {
    if (name->size() > MAX_NAME_SZ) {
        error(Error::DefinitionNameTooLong, name->to_string());
    }

    align();
    uint name_addr = alloc_cstring(name);
    return create_cont(name_addr, flags, code);
}

uint Dict::create_cont(uint name_addr, int flags, uint code) {
    // store header
    check_free_space(sizeof(Header));

    // fill previous header size
    if (vm.latest) {
        Header* latest_header = reinterpret_cast<Header*>(
                                    mem_char_ptr(vm.latest));
        uint latest_size = vm.here - latest_header->body();
        latest_header->size = latest_size; // fill size of previous header
    }

    // create new header
    Header* header = reinterpret_cast<Header*>(mem_char_ptr(vm.here));
    header->link = vm.latest;
    vm.latest = vm.here;

    header->name_addr = name_addr;

    header->flags.smudge = (flags & F_SMUDGE) ? true : false;
    header->flags.hidden = (flags & F_HIDDEN) ? true : false;
    header->flags.immediate = (flags & F_IMMEDIATE) ? true : false;

    header->size = 0; // size will be filled by next header
    header->creator_xt = 0; // filled by defining word
    header->does = 0;
    header->code = code;

    vm.here += aligned(sizeof(Header));
    return header->xt(); // return xt of word
}

uint Dict::alloc_cstring(const std::string& str) {
    return alloc_cstring(str.c_str(), static_cast<uint>(str.size()));
}

uint Dict::alloc_cstring(const char* str, uint size) {
    CString* str_str = vm.wordbuf.append_cstring(str, size);
    return alloc_cstring(str_str);
}

uint Dict::alloc_cstring(const CString* str) {
    uint alloc_size = CString::alloc_size(str->size());

    check_free_space(alloc_size);

    vm.names -= alloc_size;
    memcpy(mem_char_ptr(vm.names), str, alloc_size);

    return vm.names;
}

uint Dict::alloc_string(const std::string& str) {
    return alloc_string(str.c_str(), static_cast<uint>(str.size()));
}

uint Dict::alloc_string(const char* str, uint size) {
    uint alloc_size = LongString::alloc_size(size);

    check_free_space(alloc_size);

    vm.names -= alloc_size;
    LongString* lstring = reinterpret_cast<LongString*>(mem_char_ptr(vm.names));
    lstring->set_string(str, size);

    return vm.names;
}

uint Dict::alloc_string(const LongString* str) {
    return alloc_string(str->str(), str->size());
}

void Dict::ccomma(int value) {
    check_free_space(CHAR_SZ);
    cstore(vm.here++, value);
}

void Dict::comma(int value) {
    check_free_space(CELL_SZ);
    store(vm.here, value);
    vm.here += CELL_SZ;
}

void Dict::dcomma(dint value) {
    check_free_space(DCELL_SZ);
    dstore(vm.here, value);
    vm.here += DCELL_SZ;
}

void Dict::fcomma(double value) {
    check_free_space(FCELL_SZ);
    fstore(vm.here, value);
    vm.here += FCELL_SZ;
}

void Dict::align() {
    vm.here = aligned(vm.here);
}

Header* Dict::parse_find_word() {
    const CString* name = parse_cword(BL);
    if (name->size() == 0) {
        error(Error::AttemptToUseZeroLengthStringAsName);
    }
    return find_word(name);
}

Header* Dict::parse_find_existing_word() {
    const CString* name = parse_cword(BL);
    if (name->size() == 0) {
        error(Error::AttemptToUseZeroLengthStringAsName);
    }
    Header* header = find_word(name);
    if (header == nullptr) {
        error(Error::UndefinedWord, name->to_string());
    }
    return header;
}

Header* Dict::find_word(const std::string& name) const {
    return find_word(name.c_str(), static_cast<uint>(name.size()));
}

Header* Dict::find_word(const char* name, uint size) const {
    int ptr = vm.latest;
    while (ptr != 0) {
        Header* header = reinterpret_cast<Header*>(mem_char_ptr(ptr));
        CString* found_name = header->name();
        if (header->flags.hidden || header->flags.smudge)
            ; // skip hidden and smudged words
        else if (header->name()->size() == 0)
            ; // skip :NONAME
        else if (header->name()->size() != size)
            ; // skip words with different name size
        else if (case_insensitive_equal(name, size, found_name->str(),
                                        found_name->size())) {
            return header;
        }
        ptr = header->link;
    }

    return nullptr;
}

Header* Dict::find_word(const CString* name) const {
    return find_word(name->str(), name->size());
}

std::vector<std::string> Dict::get_words(uint wid) const {
    std::vector<uint> nts = get_word_nts(wid);
    std::vector<std::string> words;
    for (auto nt : nts) {
        Header* header = reinterpret_cast<Header*>(mem_char_ptr(nt));
        CString* found_name = header->name();
        words.push_back(found_name->to_string());
    }
    return words;
}

std::vector<uint> Dict::get_word_nts(uint wid) const {
    std::vector<uint> nts;
    int ptr = wid == 0 ? vm.latest : fetch(wid + CELL_SZ);
    while (ptr != 0) {
        Header* header = reinterpret_cast<Header*>(mem_char_ptr(ptr));
        if (header->flags.hidden || header->flags.smudge) {
            // skip hidden and smudged words
        }
        else {
            nts.push_back(ptr);
        }

        ptr = header->link;
    }
    return nts;
}

void Dict::check_free_space(int size) const {
    if (vm.here + size >= vm.names) {
        error(Error::DictionaryOverflow);
    }
}

void f_find(uint addr) {
    CString* word = reinterpret_cast<CString*>(mem_char_ptr(addr));
    Header* header = vm.dict.find_word(word->str(), word->size());
    if (header == nullptr) {
        push(addr);
        push(0);
    }
    else {
        uint xt = header->xt();
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
    Header* header = vm.dict.parse_find_existing_word();
    assert(header != nullptr);
    return header->xt();
}

void f_bracket_tick() {
    uint xt = f_tick();
    comma(xtXLITERAL);
    comma(xt);
}

void f_postpone() {
    Header* header = vm.dict.parse_find_existing_word();
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
    uint xt = f_tick();
    comma(xt);
}

void f_compile_comma() {
    comma(xtCOMMA);
}

void f_immediate() {
    Header* header = reinterpret_cast<Header*>(mem_char_ptr(vm.latest));
    header->flags.immediate = true;
}

void f_hidden() {
    Header* header = reinterpret_cast<Header*>(mem_char_ptr(vm.latest));
    header->flags.hidden = true;
}

void f_create() {
    vm.dict.parse_create(idXDOVAR, 0);
}

void f_buffer_colon() {
    vm.dict.parse_create(idXDOVAR, 0);
    uint size = pop();
    vm.dict.allot(size);
}

void f_variable() {
    vm.dict.parse_create(idXDOVAR, 0);
    comma(0);
}

void f_2variable() {
    vm.dict.parse_create(idXDOVAR, 0);
    dcomma(0);
}

void f_fvariable() {
    vm.dict.parse_create(idXDOFVAR, 0);
    fcomma(0);
}

void f_value() {
    vm.dict.parse_create(idXDOCONST, 0);
    comma(pop());
}

void f_two_value() {
    vm.dict.parse_create(idXDO2CONST, 0);
    dcomma(dpop());
}

void f_fvalue() {
    vm.dict.parse_create(idXDOFCONST, 0);
    fcomma(fpop());
}

void f_to() {
    const CString* name = parse_cword(BL);
    if (name->size() == 0) {
        error(Error::AttemptToUseZeroLengthStringAsName);
    }

    uint index = 0;
    if (find_local(name->str(), name->size(), index)) { // local found
        if (vm.user->STATE == STATE_COMPILE) {
            comma(xtXLITERAL);
            comma(index);
            comma(xtPAREN_SET_LOCAL);
        }
        else {
            error(Error::InterpretingACompileOnlyWord, name->to_string());
        }
        return;
    }

    Header* header = vm.dict.find_word(name);
    if (!header) {
        error(Error::UndefinedWord, name->to_string());
    }

    uint code = fetch(header->xt());
    if (code == idXDOCONST) {			// single cell value
        if (vm.user->STATE == STATE_COMPILE) {
            comma(xtXLITERAL);
            comma(header->body());
            comma(xtSTORE);
        }
        else {
            store(header->body(), pop());
        }
    }
    else if (code == idXDO2CONST) {		// double cell value
        if (vm.user->STATE == STATE_COMPILE) {
            comma(xtXLITERAL);
            comma(header->body());
            comma(xtTWO_STORE);
        }
        else {
            dstore(header->body(), dpop());
        }
    }
    else if (code == idXDOFCONST) {		// float cell value
        if (vm.user->STATE == STATE_COMPILE) {
            comma(xtXLITERAL);
            comma(header->body());
            comma(xtF_STORE);
        }
        else {
            fstore(header->body(), fpop());
        }
    }
    else {
        error(Error::InvalidNameArgument);
    }
}


void f_constant() {
    vm.dict.parse_create(idXDOCONST, 0);
    comma(pop());
}

void f_2constant() {
    vm.dict.parse_create(idXDO2CONST, 0);
    dcomma(dpop());
}

void f_fconstant() {
    vm.dict.parse_create(idXDOFCONST, 0);
    fcomma(fpop());
}

void f_does() {
    clear_locals();

    Header* header = reinterpret_cast<Header*>(mem_char_ptr(vm.latest));
    comma(xtXDOES_DEFINE);                  // set this word as having DOES>
    comma(header->xt());					// xt of creater word
    comma(vm.here + 2 * CELL_SZ);	// location of run code
    comma(xtEXIT);                          // exit from CREATE part
    // run code starts here
}

void f_xdoes_define() {
    int creator_xt = fetch(vm.ip);
    vm.ip += CELL_SZ;
    int run_code = fetch(vm.ip);
    vm.ip += CELL_SZ;						// start of runtime code

    Header* def_word = reinterpret_cast<Header*>(mem_char_ptr(vm.latest));
    def_word->creator_xt = creator_xt;		// store xt of creator word
    def_word->does = run_code;				// start of DOES> code
    def_word->code = idXDOES_RUN;			// new execution id
}

void f_xdoes_run(uint body) {
    push(body);							// store parameter field on the stack
    Header* header = Header::header(body - CELL_SZ);
    enter_func(header->does);           // call code after DOES>
}

void f_marker() {
    int save_latest = vm.latest;
    int save_here = vm.here;
    int save_names = vm.names;

    vm.dict.parse_create(idXMARKER, 0);

    comma(save_latest);
    comma(save_here);
    comma(save_names);
}

void f_xmarker(uint body) {
    int save_latest = fetch(body);
    int save_here = fetch(body + CELL_SZ);
    int save_names = fetch(body + 2 * CELL_SZ);

    vm.latest = save_latest;
    vm.here = save_here;
    vm.names = save_names;
}

void f_words() {
    std::vector<std::string> words = vm.dict.get_words();
    size_t col = 0;
    for (auto& word : words) {
        if (col + 1 + word.size() >= SCREEN_WIDTH) {
            std::cout << std::endl << word;
            col = word.size();
        }
        else if (col == 0) {
            std::cout << word;
            col += word.size();
        }
        else {
            std::cout << BL << word;
            col += 1 + word.size();
        }
    }
    std::cout << std::endl;
}

void f_defer() {
    vm.dict.parse_create(idXDEFER, 0);
    comma(xtABORT);
}

void f_xdefer(uint body) {
    uint xt = fetch(body);
    f_execute(xt);
}

void f_defer_fetch() {
    uint xt = pop();
    f_defer_fetch(xt);
}

void f_defer_fetch(uint xt) {
    uint body = xt + CELL_SZ;
    push(fetch(body));
}

void f_defer_store() {
    uint xt_self = pop();
    uint xt_action = pop();
    uint body = xt_self + CELL_SZ;
    store(body, xt_action);
}

void f_action_of() {
    Header* header = vm.dict.parse_find_existing_word();
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
    Header* header = vm.dict.parse_find_existing_word();
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

