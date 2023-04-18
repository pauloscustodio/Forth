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
	vm.base = 10;
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

int depth(void) {
	return STACK_SZ - vm.sp;
}

void roll(int depth) {
	if (depth > 0) {
        int bot = vm.sp + depth;
        if (bot >= STACK_SZ) {
            error(ErrorStackUnderflow);
		}
		else {
			int bot_value = vm.stack[bot];
			memmove(&vm.stack[vm.sp + 1], &vm.stack[vm.sp], depth * CELL_SZ);
			vm.stack[vm.sp] = bot_value;
		}
    }
}

void set_sp(int sp) {
	if (sp < 0)
		error(ErrorStackOverflow);
	else if (sp > STACK_SZ)
		error(ErrorStackUnderflow);
	else 
		vm.sp = sp;
}

void print_stack() {
	printf("( ");
	for (int i = STACK_SZ - 1; i >= vm.sp; i--)
		printf("%d ", vm.stack[i]);
	printf(")");
}

void dpush(dint value) {
    push(DCELL_LO(value));
    push(DCELL_HI(value));
}

dint dpop(void) {
    int hi = pop();
    int lo = pop();
    return DCELL(hi, lo);
}

dint dpeek(int depth) {
    int hi = peek(2 * depth);
    int lo = peek(2 * depth + 1);
    return DCELL(hi, lo);
}
