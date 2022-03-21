//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2022
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "dict.h"
#include "errors.h"
#include "interp.h"
#include "io.h"
#include "memory.h"
#include "parser.h"
#include "stack.h"
#include <stdio.h>
#include <string.h>

void init_dict(void) {
    // first pass - define xtWORD of all words; second pass: use them
    for (int pass = 1; pass <= 2; pass++) {
        user->here = ALIGN(sizeof(User));
        user->latest = 0;

        // init user variables
#define VAR(word, name, flags, value)	\
            user->name = value;
#include "words.def"

        // init dictionary
#define CONST(word, name, flags, value)	\
            xt##name = create(word, strlen(word), flags, idXDOCONST); \
			comma(value);
#define VAR(word, name, flags, value)	\
            xt##name = create(word, strlen(word), flags, idXDOCONST); \
			comma((char*)&user->name - mem);
#define CODE(word, name, flags, c_code)	\
            xt##name = create(word, strlen(word), flags, id##name);
#include "words.def"
    }

    // build Forth words
#define FORTH(word, name, flags, text)	\
        xt##name = create(word, strlen(word), flags, idXDOCOL); \
		compile(text);
#include "words.def"
}

void comma(int value) {
    if (user->here + CELL_SZ > DICT_SZ)
        error(ErrorMemoryOverflow);
    store(user->here, value);
    user->here += CELL_SZ;
}

void dcomma(dint value) {
    comma(DCELL_HI(value));
    comma(DCELL_LO(value));
}

void ccomma(int value) {
    if (user->here + 1 > DICT_SZ)
        error(ErrorMemoryOverflow);
    cstore(user->here, value);
    user->here++;
}

void counted_str_comma(const char* text, int len) {
    counted_str_store(user->here, text, len);
    user->here += counted_str_size(len);
}

void dict_name_comma(const char* text, int len, int flags) {
    dict_name_store(user->here, text, len, flags);
    user->here += dict_name_size(len);
}

void long_str_comma(const char* text, int len) {
    long_str_store(user->here, text, len);
    user->here += long_str_size(len);
}

void align(void) {
    user->here = ALIGN(user->here);
}

// create a word, return xt
int create(const char* name, int len, int flags, int id) {
    if (len > F_LENMASK)
        error_arg(ErrorNameTooLong, fstr_to_cstr(name, len));

    align();

    // link
    int link = user->here;
    comma(user->latest);
    user->latest = link;

    // name
    dict_name_comma(name, len, flags);

    // DOES> code
    comma(0);

    // code
    int xt = user->here;
    comma(id);
    return xt;
}

void parse_create(int id) {
    int len = 0;
    const char* name = c_parse_word(&len);
    if (len == 0)
        error(ErrorNameExpected);
    else
        create(name, len, 0, id);
}

void f_create(void) {
    parse_create(idXDOVAR);
}

void f_colon(void) {
    if (user->in_colon)
        error(ErrorNestedColonDefinition);
    else {
        user->in_colon = true;
        parse_create(idXDOCOL);
        toggle_hidden();
        user->STATE = STATE_COMPILE;
    }
}

void f_colon_noname(void) {
    if (user->in_colon)
        error(ErrorNestedColonDefinition);
    else {
        user->in_colon = true;
        create("", 0, 0, idXDOCOL);
        toggle_hidden();
        user->STATE = STATE_COMPILE;
        push(link_to_xt(user->latest));
    }
}

void f_semicolon(void) {
    if (!user->in_colon)
        error(ErrorSemicolonWithoutColon);
    else {
        comma(xtEXIT);
        user->in_colon = false;
        toggle_hidden();
        user->STATE = STATE_INTERPRET;
    }
}

void f_constant(void) {
    parse_create(idXDOCONST);
    comma(pop());
}

void f_2constant() {
    parse_create(idXDO2CONST);
    dcomma(dpop());
}

void f_variable(void) {
    parse_create(idXDOVAR);
    comma(0);
}

void f_2variable(void) {
    parse_create(idXDOVAR);
    dcomma(0);
}

void f_value(void) {
    parse_create(idXDOCONST);
    comma(pop());
}

void f_to(void) {
    int data = xt_to_body(f_tick());
    if (user->STATE == STATE_COMPILE) {
        comma(xtXLITERAL);
        comma(data);
        comma(xtSTORE);
    }
    else
        store(data, pop());
}

void f_does(void) {
    comma(xtXDOES_DEFINE);                  // set this word as having DOES>
    comma(user->here + 2 * CELL_SZ);        // location of run code
    comma(xtEXIT);                          // exit from CREATE part
    // run code starts here
}

void f_xdoes_define(void) {
    int run_code = fetch(ip);
    ip += CELL_SZ;    // start of runtime code

    int def_word = user->latest;
    store(link_to_does(def_word), run_code);    // start of DOES> code
    store(link_to_xt(def_word), idXDOES_RUN);   // new execution id
}

void f_xdoes_run(int body) {
    push(body);                     // store parameter field on the stack
    rpush(ip);                      // save current ip
    ip = fetch(body_to_does(body)); // call code after DOES>
}

// convert addresses
int link_to_name(int link) {
    if (link == 0) return 0;
    return link + CELL_SZ;
}

int link_to_does(int link) {
    if (link == 0) return 0;
    int name = link_to_name(link);
    return name + ALIGN(1 + (cfetch(name) & F_LENMASK));
}

int link_to_xt(int link) {
    if (link == 0) return 0;
    return link_to_does(link) + CELL_SZ;
}

int xt_to_link(int xt) {
    for (int link = user->latest; link; link = fetch(link)) {
        if (link_to_xt(link) == xt)
            return link;
    }
    return 0;
}

int xt_to_body(int xt) {
    if (xt == 0) return 0;
    return xt + CELL_SZ;
}

int body_to_does(int body) {
    return body - 2 * CELL_SZ;
}

void toggle_hidden(void) {
    char* lenp = mem + link_to_name(user->latest);
    *lenp ^= F_HIDDEN;
}

void toggle_immediate(void) {
    char* lenp = mem + link_to_name(user->latest);
    *lenp ^= F_IMMEDIATE;
}

int c_find_link(const char* word, int len) {
    for (int link = user->latest; link; link = fetch(link)) {
        int namep = link_to_name(link);
        int got_len = cfetch(namep);
        if ((got_len & F_HIDDEN) != 0) continue;			// hidden
        if ((got_len & F_LENMASK) != len) continue;         // not same length
        if (strncasecmp(word, mem + namep + 1, len) == 0)
            return link;
    }
    return 0;
}

void f_find(int cstr) {
    int len = 0;
    const char* word = counted_str_fetch(cstr, &len);
    int link = c_find_link(word, len);
    if (!link) {			// not found
        push(cstr);
        push(0);
    }
    else {
        int xt = link_to_xt(link);
        push(xt);

        int flags = cfetch(link_to_name(link));
        if ((flags & F_IMMEDIATE) != 0)
            push(1);		// immediate
        else
            push(-1);		// normal
    }
}

int f_tick(void) {
    int len = 0;
    const char* word = c_parse_word(&len);
    int link = c_find_link(word, len);
    if (!link) 				// not found
        error_arg(ErrorWordNotDefined, fstr_to_cstr(word, len));

    return link_to_xt(link);
}

void f_bracket_tick(void) {
    int xt = f_tick();
    comma(xtXLITERAL);
    comma(xt);
}

void f_postpone(void) {
    int xt = f_tick();
    comma(xt);
}

void f_bracket_compile(void) {
    int xt = f_tick();
    comma(xt);
}

void f_marker(void) {
    parse_create(idXMARKER);
}

void f_xmarker(int body) {
    int xt = body - CELL_SZ;
    int link = xt_to_link(xt);
    user->here = link;
    user->latest = fetch(link);
}

static void print_word(int namep) {
    if (namep == 0)
        printf("??? ");
    else {
        const char* name = mem + namep + 1;
        int len = cfetch(namep) & F_LENMASK;
        printf("%.*s ", len, name);
    }
}

void print_words(void) {
    int width = 0;
    for (int link = user->latest; link; link = fetch(link)) {
        int namep = link_to_name(link);
        int flags = cfetch(namep);
        if ((flags & F_HIDDEN) != 0) continue;			// hidden
        int len = flags & F_LENMASK;
        width += len + 1;
        if (width > SCREEN_WIDTH) {
            printf("\n");
            width = len + 1;
        }
        print_word(namep);
    }
    printf("\n");
}
