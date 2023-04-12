//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2023
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "forth.h"
#include "errors.h"

#define BLANKS	" \t\r\n\v\f"

static void parse_text(char* text) {
	const char* word = strtok(text, BLANKS);
	while (word != NULL) {
		if (false) {}
#define X(name, id, flags, code)	else if (strcasecmp(word, name) == 0) { code; }
#include "words.def"
		else if (isdigit(*word) || (*word == '-' && isdigit(word[1]))) {
			push(atoi(word));
		}
		else {
			error_arg(ErrorUndefinedWord, word);
		}
		word = strtok(NULL, BLANKS);
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
