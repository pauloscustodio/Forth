//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#pragma once

#include "forth.h"
#include <string>
#include <unordered_map>
#include <vector>

enum class VarType { Frame, Int, DInt, Float };

struct VarName {
    VarType type;
    std::string name;
    uint index;
};

struct VarValue {
    VarType type;
    union {
        size_t frame;
        int n;
        dint d;
        double f;
    } value;
};

class Locals {
public:
    void clear();
    void enter_frame();
    void leave_frame();

    void add_local(const std::string& name, VarType type);

    void init_int_local();
    void init_dint_local();
    void init_float_local();

    void get_local(uint index);
    void set_local(uint index);

    bool find_local(const std::string& name, VarName& vname);

    void parse_declaration();

private:
    std::vector<VarValue> vars;
    size_t frame{ 0 };
    std::unordered_map<std::string, VarName> names;

};

void f_paren_local();
void f_paren_local(const char* name, uint size);
void f_paren_local(const std::string& name);
void f_locals_bar();
void f_locals_bracket();
bool find_local(const std::string& name, VarName& vname);
bool find_local(const char* name, uint size, VarName& vname);
