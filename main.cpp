//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "env.h"
#include "forth.h"
#include "vm.h"
#include <iostream>
#include <cstring>
using namespace std;

const char* FORTH_ENV = "FORTH";

static void die_usage() {
	cerr << "Usage: forth [-e forth] [-t] [source [args...]]" << endl;
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
	// parse env variable
	const char* envp = getenv(FORTH_ENV);
	if (envp != nullptr) {
		vm.input->set_text(envp, strlen(envp));
		f_execute(xtINTERPRET);
	}

	// parse command line
	g_argc = argc; g_argv = argv;
	g_interactive = false;
	bool did_forth = false;
	g_argc--; g_argv++;
	while (g_argc > 0 && g_argv[0][0] == '-') {
		switch (g_argv[0][1]) {
		case 'e':
			if (g_argc == 1)
				die_usage();
			else {
				g_argc--; g_argv++;
                vm.input->set_text(g_argv[0], strlen(g_argv[0]));
				f_execute(xtINTERPRET);
				did_forth = true;
			}
			break;
		case 't':
			vm.user->TRACE = F_TRUE;
			break;
		default:
			die_usage();
		}
		g_argc--; g_argv++;
	}

	// get script, if any
	if (g_argc == 0) {
		if (!did_forth) {
			vm.input->open_terminal();
			g_interactive = true;
			f_execute(xtQUIT);
		}
	}
	else {
		const char* source = g_argv[0];
		g_argc--; g_argv++;

		vm.input->open_file(source);
		f_execute(xtQUIT);
	}

	return EXIT_SUCCESS;
}
