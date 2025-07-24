//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include <cstdint>
using namespace std;

// types
typedef uint8_t			uchar;
typedef unsigned int	uint;
typedef int64_t			dint;
typedef uint64_t		udint;

struct VM {
	// VM size
	inline static const int BUFFER_SZ = 1024;
	inline static const int DICT_SZ = 256 * 1024;
	inline static const int STACK_SZ = 16 * 1024;

	// constants
	inline static const char BL = ' ';
	inline static const char CR = '\n';
	inline static const int SCREEN_WIDTH = 80;

	inline static const int F_TRUE = -1;
	inline static const int F_FALSE = 0;

	// VM
	char	wordbuf[BUFFER_SZ];
	int		wordbuf_ptr;
	char	tib[BUFFER_SZ];
	int		tib_ptr;
	int		tib_size;
	char	mem[DICT_SZ];
	int		stack[STACK_SZ];
	int		sp;
	int		rstack[STACK_SZ];
	int		rp;

	// pointer to address
	int ptr_to_addr(const char* ptr);

	// init
	void init();
	void init_stacks();

	// stack
	void push(int value);
	int pop();
	int peek(int depth = 0);
	void dot_stack();

	// words
	void words();

	// input
	void refill(const string& text);
	bool parse_word(char delimiter = ' ');
	void f_s_quote();
	void f_type();
	const char* append_wordbuf(const char* str, int len);
	void f_environment_q();
};

extern VM vm;
