//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "forth.h"
#include "memory.h"
#include "vm.h"
#include <cstring>
using namespace std;

Mem::Mem() {
	memset(data_, 0, sizeof(data_));
	bottom_ = 0;
	top_ = static_cast<int>(sizeof(data_));
}

int Mem::addr(const char* ptr) const {
	int addr = check_addr(ptr - data_);
	return addr;
}

int Mem::addr(const int* ptr) const {
	int addr = check_addr(reinterpret_cast<const char*>(ptr) - data_);
	return addr;
}

char* Mem::char_ptr(int addr, int size) {
	addr = check_addr(addr, size);
	return data_ + addr;
}

int* Mem::int_ptr(int addr, int size) {
	addr = check_addr(addr, size);
	return reinterpret_cast<int*>(data_ + addr);
}

int Mem::fetch(int addr) {
	return *int_ptr(addr, CELL_SZ);
}

void Mem::store(int addr, int value) {
	*int_ptr(addr, CELL_SZ) = value;
}

dint Mem::dfetch(int addr) {
	int hi = fetch(addr);
	int lo = fetch(addr + CELL_SZ);
	return dcell(hi, lo);
}

void Mem::dstore(int addr, dint value) {
	store(addr, dcell_hi(value));
	store(addr + CELL_SZ, dcell_lo(value));
}

int Mem::cfetch(int addr) {
	return static_cast<uchar>(data_[check_addr(addr, CHAR_SZ)]);
}

void Mem::cstore(int addr, int value) {
	data_[check_addr(addr, CHAR_SZ)] = value;
}

void Mem::fill(int addr, int size, char c) {
	memset(char_ptr(addr), c, size);
}

void Mem::erase(int addr, int size) {
	fill(addr, size, 0);
}

void Mem::move(int src, int dst, int size) {
	memmove(char_ptr(dst), char_ptr(src), size);
}

char* Mem::alloc_bottom(int size) {
	if (bottom_ + size >= top_)
		error(Error::MemoryOverflow);
	char* ret = char_ptr(bottom_);
	bottom_ += aligned(size);
	return ret;
}

char* Mem::alloc_top(int size) {
	if (bottom_ + size >= top_)
		error(Error::MemoryOverflow);
	top_ -= aligned(size);
	return char_ptr(top_);
}


int Mem::check_addr(ptrdiff_t addr, int size) const {
	return check_addr(static_cast<int>(addr), size);
}

int Mem::check_addr(int addr, int size) const {
	if (addr < 0 || addr + size > static_cast<int>(sizeof(data_))) {
		error(Error::InvalidMemoryAddress);
		return 0;
	}
	else if (size > CHAR_SZ && (addr % CELL_SZ) != 0) {
		error(Error::AddressAlignmentException);
		return 0;
	}
	else {
		return addr;
	}
}

void f_fill() {
	int c = pop();
	int n = pop();
	int addr = pop();
	memset(mem_char_ptr(addr), c, n);
}

void f_erase() {
	int n = pop();
	int addr = pop();
	memset(mem_char_ptr(addr), 0, n);
}

void f_move() {
	int n = pop();
	int dst = pop();
	int src = pop();
	memmove(mem_char_ptr(dst), mem_char_ptr(src), n);
}
