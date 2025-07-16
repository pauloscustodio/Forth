//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "vm.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

static bool case_insensitive_equal(const string& a, const string& b) {
	return a.size() == b.size() &&
		equal(a.begin(), a.end(), b.begin(), [](char c1, char c2) {
		return tolower(static_cast<uchar>(c1)) ==
			tolower(static_cast<uchar>(c2));
			});
}

static void exec_word(const string& word) {
	if (!word.empty()) {
		if (false) {}
		//@@BEGIN: WordsImplementation
		else if (case_insensitive_equal(word, ".S")) { vm.stack->print(); }
		else if (case_insensitive_equal(word, "S\"")) { vm.f_s_quote(); }
		else if (case_insensitive_equal(word, "COUNT")) { f_count(); }
		else if (case_insensitive_equal(word, "TYPE")) { vm.f_type(); }
		else if (case_insensitive_equal(word, "ENVIRONMENT?")) { vm.f_environment_q(); }
		else if (case_insensitive_equal(word, "WORDS")) { f_words(); }
		else if (case_insensitive_equal(word, "+")) { push(pop()+pop()); }
		else if (case_insensitive_equal(word, "PICK")) { push(peek(pop())); }
		else if (case_insensitive_equal(word, "DUP")) { push(peek(0)); }
		else if (case_insensitive_equal(word, "DROP")) { pop(); }
		else if (case_insensitive_equal(word, "THROW")) { error(static_cast<Error>(pop())); }
		else if (case_insensitive_equal(word, ".")) { cout << pop() << BL; }
		else if (case_insensitive_equal(word, "C@")) { f_c_fetch(); }
		else if (case_insensitive_equal(word, "C!")) { f_c_store(); }
		else if (case_insensitive_equal(word, "@")) { f_fetch(); }
		else if (case_insensitive_equal(word, "!")) { f_store(); }
		else if (case_insensitive_equal(word, "PAD")) { f_pad(); }
		//@@END
		else if (isdigit(static_cast<uchar>(word[0])) ||
			(word.size() >= 2 && word[0] == '-' && isdigit(static_cast<uchar>(word[1])))) {
			push(stoi(word));
		}
		else if (word.size() == 3 && word[0] == '\'' && word[2] == '\'') {
			push(word[1]);
		}
		else {
			error(Error::UndefinedWord, word);
		}
	}
}

static void exec_text(const string& str) {
	vm.tib->refill(str.c_str(), static_cast<int>(str.size()));
	char* word;
	while ((word = c_word()) != nullptr)
		exec_word(string(word + 1, word + 1 + *reinterpret_cast<uchar*>(word)));
}

static void exec_file(const string& filename) {
	ifstream ifs(filename);
	if (!ifs.is_open()) {
		error(Error::NonExistentFile, filename);
	}
	else {
		string line;
		while (getline(ifs, line))
			exec_text(line);
	}
}

static void die_usage() {
	cerr << "Usage: forth [-e forth] [source [args...]]" << endl;
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
	argc--; argv++;
	while (argc > 0 && argv[0][0] == '-') {
		switch (argv[0][1]) {
		case 'e':
			if (argc == 1)
				die_usage();
			else {
				argc--; argv++;
				exec_text(argv[0]);
			}
			break;
		default:
			die_usage();
		}
		argc--; argv++;
	}

	// get script, if any
	if (argc > 0) 
		exec_file(argv[0]);
	return EXIT_SUCCESS;
}
