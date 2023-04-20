//-----------------------------------------------------------------------------
// C implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2023
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "forth.h"
#include "math.h"
#include "errors.h"

VM vm;

//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
void vm_init(void) {
    // zero all memory
	memset(&vm, 0, sizeof(vm));
	
    // blank all buffers
	memset(&vm.pad, BL, sizeof(vm.pad));
	
	vm.latest_p = NULL;
	vm.dict_p = &vm.dict[0];
	vm.sp = STACK_SZ;
	vm.rp = STACK_SZ;

#define VAR(word, name, flags, value)	vm.name = value;
#include "words.def"
}

//-----------------------------------------------------------------------------
// memory
//-----------------------------------------------------------------------------
uint ptr_to_addr(void *p) {
	if ((char*)p < (char*)&vm || (char*)p > (char*)&vm + sizeof(vm)) {
		error(ErrorInvalidMemoryAddress);
		return -1;
	}
	else {
		return (uint)((char*)p - (char*)&vm);
	}
}

char* addr_to_cptr(uint addr) {
	if (addr >= sizeof(vm)) {
		error(ErrorInvalidMemoryAddress);
		return NULL;
	}
	else {
		return (char*)&vm + addr;
	}
}

int* addr_to_ptr(uint addr) {
	if (addr % CELL_SZ != 0) {
		error(ErrorAddressAlignmentException);
		return NULL;
	}
	else {
		return (int*)addr_to_cptr(addr);
	}
}

int fetch(uint addr) {
	return *addr_to_ptr(addr);
}

void store(uint addr, int value) {
	*addr_to_ptr(addr) = value;
}

dint dfetch(uint addr) {
    int hi = fetch(addr);
    int lo = fetch(addr + CELL_SZ);
    return DCELL(hi, lo);
}

void dstore(uint addr, dint value) {
    store(addr, DCELL_HI(value));
    store(addr + CELL_SZ, DCELL_LO(value));
}

int cfetch(uint addr) {
    return *addr_to_cptr(addr);
}

void cstore(uint addr, int value) {
    *addr_to_cptr(addr) = value;
}

void f_fill(void) {
	int c = pop();
	uint n = pop();
	uint addr = pop();
	addr_to_cptr(addr + n - 1);		// check bounds
	memset(addr_to_cptr(addr), c, n);
}

void f_erase(void) {
	uint n = pop();
	uint addr = pop();
	addr_to_cptr(addr + n - 1);		// check bounds
	memset(addr_to_cptr(addr), 0, n);
}

void f_move(void) {
	uint n = pop();
	uint dst = pop();
	addr_to_cptr(dst + n - 1);		// check bounds
	uint src = pop();
	addr_to_cptr(src + n - 1);		// check bounds
	memmove(addr_to_cptr(dst), addr_to_cptr(src), n);
}

static uint _counted_str_size(uint len, uint len_mask) {
	return ALIGN(1 + (len & len_mask));
}

static const char* _counted_str_fetch(uint addr, uint len_mask, uint* len) {
	*len = cfetch(addr) & len_mask;
	return addr_to_cptr(addr + 1);
}

static void _counted_str_store(uint addr, const char* text, uint len, uint len_mask, int len_flags) {
	addr_to_cptr(addr + (len & len_mask));				// check for overflow
	cstore(addr, (len & len_mask) | len_flags);
	memcpy(addr_to_cptr(addr + 1), text, len);
}

uint counted_str_size(uint len) {
	return _counted_str_size(len, F_COUNTED_STR_MASK);
}

const char* counted_str_fetch(uint addr, uint* len) {
	return _counted_str_fetch(addr, F_COUNTED_STR_MASK, len);
}

void counted_str_store(uint addr, const char* text, uint len) {
	_counted_str_store(addr, text, len, F_COUNTED_STR_MASK, 0);
}

uint dict_name_size(uint len) {
	return _counted_str_size(len, F_LENMASK);
}

const char* dict_name_fetch(uint addr, uint* len) {
	return _counted_str_fetch(addr, F_LENMASK, len);
}

void dict_name_store(uint addr, const char* text, uint len, int flags) {
	_counted_str_store(addr, text, len, F_LENMASK, flags);
}

int long_str_size(uint len) {
	return ALIGN(CELL_SZ + len);
}

const char* long_str_fetch(uint addr, uint* len) {
	*len = fetch(addr);
	return addr_to_cptr(addr + CELL_SZ);
}

void long_str_store(uint addr, const char* text, uint len) {
	addr_to_cptr(addr + CELL_SZ + len - 1);      // check for overflow
	store(addr, len);
	memcpy(addr_to_cptr(addr + CELL_SZ), text, len);
}

//-----------------------------------------------------------------------------
// dictionary
//-----------------------------------------------------------------------------
static void check_dict_space(uint size) {
	if (vm.dict_p + size >= &vm.dict[DICT_SZ])
		error(ErrorDictionaryOverflow);
}

uint here(void) {
	return ptr_to_addr(vm.dict_p);
}

uint latest(void) {
	return vm.latest_p == NULL ? 0 : ptr_to_addr(vm.latest_p);
}

void align(void) {
	vm.dict_p = &vm.dict[0] + ALIGN(here());
	check_dict_space(0);
}

void comma(int value) {
	align();
	check_dict_space(CELL_SZ);
	store(here(), value);
	vm.dict_p += CELL_SZ;
}

void dcomma(dint value) {
	comma(DCELL_HI(value));
	comma(DCELL_LO(value));
}

void ccomma(int value) {
	check_dict_space(BYTE_SZ);
	cstore(here(), value);
	vm.dict_p++;
}

void counted_str_comma(const char* text, uint len) {
	check_dict_space(counted_str_size(len));
	counted_str_store(here(), text, len);
	vm.dict_p += counted_str_size(len);
}

void dict_name_comma(const char* text, uint len, int flags) {
	check_dict_space(dict_name_size(len));
	dict_name_store(here(), text, len, flags);
	vm.dict_p += dict_name_size(len);
}

void long_str_comma(const char* text, uint len) {
	align();
	check_dict_space(long_str_size(len));
	long_str_store(here(), text, len);
	vm.dict_p += long_str_size(len);
}

//-----------------------------------------------------------------------------
// stacks
//-----------------------------------------------------------------------------
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
	else {
		int idx = vm.sp + depth;
		if (idx >= STACK_SZ) {
			error(ErrorStackUnderflow);
			return 0;
		}
		else {
			return vm.stack[idx];
		}
	}
}

int depth(void) {
	return STACK_SZ - vm.sp;
}

void roll(int depth) {
	if (depth < 0) {
		error(ErrorInvalidMemoryAddress);
	}
	else {
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

uint s0(void) {
	return ptr_to_addr(&vm.stack[STACK_SZ]);
}

uint get_sp(void) {
	return ptr_to_addr(&vm.stack[vm.sp]);
}

void set_sp(uint addr) {
	if (addr < ptr_to_addr(&vm.stack[0]) || addr > ptr_to_addr(&vm.stack[STACK_SZ]))
		error(ErrorInvalidMemoryAddress);
	else
		vm.sp = (addr - ptr_to_addr(&vm.stack[0])) / CELL_SZ;
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

void rpush(int value) {
	if (vm.rp <= 0)
		error(ErrorReturnStackOverflow);
	else
		vm.rstack[--vm.rp] = value;
}

int rpop(void) {
	if (vm.rp >= STACK_SZ) {
		error(ErrorReturnStackUnderflow);
		return 0;
	}
	else {
		return vm.rstack[vm.rp++];
	}
}

int rpeek(int depth) {
	if (depth < 0) {
		error(ErrorInvalidMemoryAddress);
		return 0;
	}
	else {
		int idx = vm.rp + depth;
		if (idx >= STACK_SZ) {
			error(ErrorReturnStackUnderflow);
			return 0;
		}
		else {
			return vm.rstack[idx];
		}
	}
}

void rdpush(dint value) {
	rpush(DCELL_LO(value));
	rpush(DCELL_HI(value));
}

dint rdpop(void) {
	int hi = rpop();
	int lo = rpop();
	return DCELL(hi, lo);
}

dint rdpeek(int depth) {
	int hi = rpeek(2 * depth);
	int lo = rpeek(2 * depth + 1);
	return DCELL(hi, lo);
}

uint r0(void) {
	return ptr_to_addr(&vm.rstack[STACK_SZ]);
}

uint get_rp(void) {
	return ptr_to_addr(&vm.rstack[vm.rp]);
}

void set_rp(uint addr) {
	if (addr < ptr_to_addr(&vm.rstack[0]) || addr > ptr_to_addr(&vm.rstack[STACK_SZ]))
		error(ErrorInvalidMemoryAddress);
	else
		vm.rp = (addr - ptr_to_addr(&vm.rstack[0])) / CELL_SZ;
}

void print_rstack() {
	printf("(R: ");
	for (int i = STACK_SZ - 1; i >= vm.rp; i--)
		printf("%d ", vm.rstack[i]);
	printf(")");
}

//-----------------------------------------------------------------------------
