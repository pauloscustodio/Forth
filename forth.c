//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2023
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "forth.h"
#include "errors.h"

VM vm;

//-----------------------------------------------------------------------------
void vm_init(void) {
	memset(&vm, 0, sizeof(vm));
	stacks_init();
}

//-----------------------------------------------------------------------------
void stacks_init(void) {
	vm.sp = STACK_SZ;
	vm.rp = STACK_SZ;
}

void push(int value) {
	if (vm.sp <= 0)
		error(ErrorStackOverflow);
	else
		vm.stack[--vm.sp] = value;
}
	
int pop(void) {
	if (vm.sp >= STACK_SZ) {
		error(ErrorStackUnderflow);
		return 0;
	}
	else {
		return vm.stack[vm.sp++];
	}
}

int peek(int depth) {
	if (depth < 0) {
		error(ErrorInvalidMemoryAddress);
		return 0;
	}
	int idx = vm.sp + depth;
	if (idx >= STACK_SZ) {
		error(ErrorStackUnderflow);
		return 0;
	}
	else {
		return vm.stack[idx];
	}
}

void print_stack() {
	printf("( ");
	for (int i = STACK_SZ - 1; i >= vm.sp; i--)
		printf("%d ", vm.stack[i]);
	printf(")");
}
