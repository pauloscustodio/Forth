//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "errors.h"
#include "locals.h"
#include "parser.h"
#include "vm.h"
#include <string>
#include <vector>

void Locals::clear() {
    vars_.clear();
    frame_ = 0;
    names_.clear();
}

uint Locals::size() const {
    return static_cast<uint>(vars_.size());
}

void Locals::resize(uint size) {
    vars_.resize(size);
}

uint Locals::frame() const {
    return static_cast<uint>(frame_);
}

void Locals::set_frame(uint frame) {
    frame_ = frame;
}

void Locals::enter_frame() {
    VarValue v;
    v.type = VarType::Frame;
    v.value.frame = frame_;
    frame_ = vars_.size() + 1;
    vars_.push_back(v);
}

void Locals::leave_frame() {
    if (vars_.empty()) {
        // no frame_ to leave
        frame_ = 0;
    }
    else {
        // remove all vars_ in current frame_
        vars_.resize(frame_);
        frame_ = vars_.back().value.frame;
        vars_.pop_back();
    }
}

void Locals::add_local(const std::string& name, VarType type) {
    auto it = names_.find(to_upper(name));
    if (it != names_.end()) {
        error(Error::DuplicateDefinition, name);
    }

    uint index = static_cast<uint>(names_.size());

    VarName vname;
    vname.type = type;
    vname.name = name;
    vname.index = index;
    names_[to_upper(name)] = vname;

    switch (vname.type) {
    case VarType::Int:
        comma(xtXW_TO_LOCAL);
        break;
    case VarType::DInt:
        comma(xtXD_TO_LOCAL);
        break;
    case VarType::Float:
        comma(xtXF_TO_LOCAL);
        break;
    default:
        assert(0);
    }
}

void Locals::init_int_local() {
    int value = pop();
    VarValue vv;
    vv.type = VarType::Int;
    vv.value.n = value;
    vars_.push_back(vv);
}

void Locals::init_dint_local() {
    dint value = dpop();
    VarValue vv;
    vv.type = VarType::DInt;
    vv.value.d = value;
    vars_.push_back(vv);
}

void Locals::init_float_local() {
    double value = fpop();
    VarValue vv;
    vv.type = VarType::Float;
    vv.value.f = value;
    vars_.push_back(vv);
}

void Locals::get_local(uint index) {
    size_t i = index + frame_;
    if (i >= vars_.size()) {
        error(Error::LocalsStackUnderflow);
    }

    switch (vars_[i].type) {
    case VarType::Int:
        push(vars_[i].value.n);
        break;
    case VarType::DInt:
        dpush(vars_[i].value.d);
        break;
    case VarType::Float:
        fpush(vars_[i].value.f);
        break;
    default:
        assert(0);
    }
}

void Locals::set_local(uint index) {
    size_t i = index + frame_;
    if (i >= vars_.size()) {
        error(Error::LocalsStackUnderflow);
    }

    switch (vars_[i].type) {
    case VarType::Int:
        vars_[i].value.n = pop();
        break;
    case VarType::DInt:
        vars_[i].value.d = dpop();
        break;
    case VarType::Float:
        vars_[i].value.f = fpop();
        break;
    default:
        assert(0);
    }
}

bool Locals::find_local(const std::string& name, VarName& vname) const {
    auto it = names_.find(to_upper(name));
    if (it == names_.end()) {
        return false;
    }
    else {
        vname = it->second;
        return true;
    }
}

static void check_in_colon() {
    if (cs_ddepth() != 1) {
        error(Error::CompilerNesting);
    }

    dint pos_addr = cs_dpeek();
    if (dcell_hi(pos_addr) != POS_COLON_START) {
        error(Error::CompilerNesting);
    }
}

void Locals::parse_declaration() {
    check_in_colon();

    int state = 0;  // collecting locals to be initialized from stack
    std::vector<VarName> init_locals;
    std::vector<VarName> uninit_locals;
    VarType type = VarType::Int;
    while (true) {
        uint size;
        const char* word = parse_word(size, BL);

        if (size == 0) {
            break;    // end of locals
        }

        std::string name = to_upper(std::string(word, word + size));
        if (name == ":}") {             // ANS
            break;    // end of locals
        }
        else if (name == "}") {         // Gforth
            break;    // end of locals
        }
        else if (name == "|") {         // ANS
            state = 1;    // collecting uninitialized locals
        }
        else if (name == "--") {
            state = 2;    // collecting and discarding output params
        }
        else if (name == "W:") {
            type = VarType::Int;
        }
        else if (name == "D:") {
            type = VarType::DInt;
        }
        else if (name == "F:") {
            type = VarType::Float;
        }
        else if (state == 0) {
            VarName def;
            def.name = name;
            def.type = type;
            init_locals.push_back(def);     // initialized locals
            type = VarType::Int;
        }
        else if (state == 1) {
            VarName def;
            def.name = name;
            def.type = type;
            uninit_locals.push_back(def);   // uninitialized locals
            type = VarType::Int;
        }
        else if (state == 2) {
            // discard output params
        }
    }

    // build output code
    while (!init_locals.empty()) {
        VarName def = init_locals.back();
        add_local(def.name, def.type);
        init_locals.pop_back();
    }
    while (!uninit_locals.empty()) {
        VarName def = uninit_locals.back();
        switch (def.type) {
        case VarType::Int:
            comma(xtXLITERAL);
            comma(0);
            break;
        case VarType::DInt:
            comma(xtX2LITERAL);
            dcomma(0);
            break;
        case VarType::Float:
            comma(xtFLITERAL);
            fcomma(0.0);
            break;
        default:
            assert(0);
        }
        add_local(def.name, def.type);
        uninit_locals.pop_back();
    }
}

void f_paren_local() {
    uint size = pop();
    uint addr = pop();
    const char* name = mem_char_ptr(addr, size);

    if (size != 0) {
        f_paren_local(name, size);
    }
}

void f_paren_local(const char* name, uint size) {
    f_paren_local(std::string(name, name + size));
}

void f_paren_local(const std::string& name) {
    vm.locals.add_local(name, VarType::Int);
}

void f_locals_bar() {
    while (true) {
        uint size;
        const char* word = parse_word(size, BL);
        if (size == 0 || (size == 1 && word[0] == '|')) {
            break;  // end of locals
        }
        f_paren_local(word, size);
    }
}

void f_locals_bracket() {
    vm.locals.parse_declaration();
}

bool find_local(const std::string& name, VarName& vname) {
    return vm.locals.find_local(name, vname);
}

bool find_local(const char* name, uint size, VarName& vname) {
    return find_local(std::string(name, name + size), vname);
}

