//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2023
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "forth.h"
#include "errors.h"
#include "io.h"
#include "math.h"
#include "str.h"

#define BLANKS	" \t\r\n\v\f"

void f_words(void) {
    size_t col = 0;
#define X(word) \
    if (col + 1 + strlen(word) >= SCREEN_WIDTH) { printf("\n%s", word); col = strlen(word); } \
    else if (col == 0) { printf("%s", word); col += strlen(word); } \
    else { printf(" %s", word); col += 1 + strlen(word); }
#define CONST(word, name, flags, value) X(word)
#define VAR(word, name, flags, value)   X(word)
#define CODE(word, name, flags, c_code) X(word)
#define FORTH(word, name, flags, text)  X(word)
#include "words.def"
#undef X
    printf("\n");
}

static int char_digit(char c) {
    if (isdigit(c))
        return c - '0';
    else if (isalpha(c))
        return toupper(c) - 'A' + 10;
    else
        return -1;			// no digit
}

static bool parse_number(const char* str, size_t len, dint* value, bool *is_double) {
    // init output vars
	int sign = 1;
	int base = vm.BASE;
	vm.DPL = 0;
	*value = 0;
	*is_double = false;
	
	const char* p = str;
    const char* end = str + len;

    if (p >= end) return false;			// no number

    // check sign
    switch (*p) {
    case '-':
        sign = -1;
        p++;
        break;
    case '+':
        sign = 1;
        p++;
        break;
    }
    if (p >= end) return 0;			// no number

    // check base prefix
    switch (*p) {
    case '#':
    case '$':
        base = 16;
        p++;
        break;
    case '%':
        base = 2;
        p++;
        break;
    case '\'':
        if (end - p == 3 && p[2] == '\'') {
            *value = sign * (unsigned char)p[1];
            return true;
        }
        break;
    }
    if (p >= end) return 0;			// no number

    // collect digits
    int num_digits = 0;
	int digit;
    while (p < end) {
        char c = *p++;
        switch (c) {
        case ',':
        case '.':
        case '+':
        case '-':
        case '/':
        case ':':
            vm.DPL = 0;
            *is_double = true;
            break;
        default:
            digit = char_digit(c);
            if (digit < 0 || digit >= base)
                return false;		// digit not in BASE

            *value = *value * base + digit;
            num_digits++;
            if (*is_double)
                vm.DPL++;
        }
    }
    if (num_digits == 0) return false;		// no digits found

    *value *= sign;
	return true;
}

static void parse_text(char* text) {
	dint value;
	bool is_double;
	const char* got_word = strtok(text, BLANKS);
	while (got_word != NULL) {
		if (false) {}
#define CONST(word, name, flags, value) else if (strcasecmp(got_word, word) == 0) { push(value); }
#define VAR(word, name, flags, value)   else if (strcasecmp(got_word, word) == 0) { push(ptr_to_addr(&vm.name)); }
#define CODE(word, name, flags, c_code) else if (strcasecmp(got_word, word) == 0) { c_code; }
#define FORTH(word, name, flags, text)  else if (strcasecmp(got_word, word) == 0) { error_arg(ErrorUndefinedWord, word); }
#include "words.def"
		else if (parse_number(got_word, strlen(got_word), &value, &is_double)) {
			if (is_double) 
				dpush(value);
			else
				push(DCELL_LO(value));
		}
		else {
			error_arg(ErrorUndefinedWord, got_word);
		}
        got_word = strtok(NULL, BLANKS);
	}
}

static void parse_file(const char* filename) {
	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		error_arg(ErrorNonExistentFile, filename);
	}
	else {
		char line[BUFSIZ];
		while (fgets(line, sizeof(line), fp)) {
			parse_text(line);
		}
		
		fclose(fp);
	}
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fputs("usage: forth file.fs args...\n", stderr);
		return EXIT_FAILURE;
	}
	vm_init();
	parse_file(argv[1]);
}
