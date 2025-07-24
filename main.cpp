//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "dict.h"
#include "errors.h"
#include "math.h"
#include "parser.h"
#include "vm.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

static void exec_word(const string& word) {
	if (!word.empty()) {
		bool is_double = false;
		dint value = 0;
		Header* header = vm.dict->find_word(word.c_str(), word.size());
		if (header) {
			execute(header->xt());
		}
		else if (parse_number(word.c_str(), word.size(), is_double, value)) {
			if (is_double)
				dpush(value);
			else
				push(dcell_lo(value));
		}
		else {
			error(Error::UndefinedWord, word);
		}
	}
}

static void exec_buffers() {
	while (true) {
		const ForthString* word = parse_word(BL);
		if (word == nullptr)
			break;
		exec_word(word->to_string());
	}
}

static void die_usage() {
	cerr << "Usage: forth [-e forth] [source [args...]]" << endl;
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
	bool did_forth = false;
	argc--; argv++;
	while (argc > 0 && argv[0][0] == '-') {
		switch (argv[0][1]) {
		case 'e':
			if (argc == 1)
				die_usage();
			else {
				argc--; argv++;
				vm.input->push_text(argv[0]);
				exec_buffers();
				vm.input->pop_input();
				did_forth = true;
			}
			break;
		default:
			die_usage();
		}
		argc--; argv++;
	}

	// get script, if any
	if (argc == 0) {
		if (!did_forth) {
			vm.input->push_cin();
			exec_buffers();
		}
	}
	else {
		const char* source = argv[0];
		argc--; argv++;

		vm.input->push_file(source);
		exec_buffers();
	}

	return EXIT_SUCCESS;
}
