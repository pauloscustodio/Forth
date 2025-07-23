//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
using namespace std;

class Pad {
public:
	void init() {}

	char* pad() { return m_pad; }

private:
	char m_pad[PAD_SZ];
};

struct User {
	//@@BEGIN: Vars
	int BASE;
	int STATE;
	int DPL;
	//@@END

	void init();
};
