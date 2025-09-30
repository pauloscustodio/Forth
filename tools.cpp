//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "parser.h"
#include "tools.h"
#include "vm.h"
#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>

void f_dump() {
    uint size = pop();
    uint addr = pop();
    const char* mem = mem_char_ptr(addr, size);
    f_dump(mem, size);
}

void f_dump(const char* mem, uint size) {
    uint addr = mem_addr(mem);
    uint addr_lo = addr & ~0xF;
    uint addr_hi = (addr + size + 15) & ~0xF;
    for (uint p = addr_lo; p < addr_hi; p += 16) {
        std::cout << std::endl << std::hex << std::setfill('0') << std::setw(
                      8) << p << BL << BL;
        for (uint q = p; q < p + 16; ++q) {
            if (q < addr || q >= addr + size) {
                std::cout << BL << BL << BL;
            }
            else {
                std::cout << std::hex << std::setfill('0') << std::setw(2) << cfetch(q) << BL;
            }
        }
        std::cout << BL << BL;
        for (uint q = p; q < p + 16; ++q) {
            if (q < addr || q >= addr + size) {
                std::cout << BL;
            }
            else {
                char c = cfetch(q);
                std::cout << (is_print(c) ? c : '.');
            }
        }
    }
    std::cout << std::endl << std::dec << std::setfill(' ') << std::setw(0);
}

struct Line {
    uint label_id{ 0 };
    uint addr{ 0 };
    uint target_addr{ 0 };
    std::string text;
};

static std::vector<Line> disassemble(uint body, uint size) {
    std::vector<Line> lines;

    uint ptr = body;
    int indent = 0;
    while (ptr < body + size) {
        Line line;
        line.addr = ptr;

        uint xt = fetch(ptr);
        ptr += CELL_SZ;
        Header* header = Header::header(xt);
        if (xt == xtXLITERAL) {
            int value = fetch(ptr);
            ptr += CELL_SZ;
            line.text = std::string(indent, ' ') + number_to_string(value);
        }
        else if (xt == xtX2LITERAL) {
            dint value = dfetch(ptr);
            ptr += DCELL_SZ;
            line.text = std::string(indent, ' ') + number_dot_to_string(value);
        }
        else if (xt == xtXFLITERAL) {
            double value = ffetch(ptr);
            ptr += FCELL_SZ;
            line.text = std::string(indent, ' ') + number_e_to_string(value);
        }
        else if (xt == xtBRANCH || xt == xtZBRANCH) {
            int dist = fetch(ptr);
            line.target_addr = ptr + dist;
            ptr += CELL_SZ;
            line.text = std::string(indent, ' ') + header->name()->to_string();
        }
        else if (xt == xtXDOT_QUOTE) {
            int str_addr = fetch(ptr);
            ptr += CELL_SZ;
            const LongString* message = reinterpret_cast<const LongString*>(
                                            mem_char_ptr(str_addr));
            line.text = std::string(indent, ' ') + ".\" " + message->to_string() + "\"";
        }
        else if (xt == xtXS_QUOTE) {
            int str_addr = fetch(ptr);
            ptr += CELL_SZ;
            const LongString* message = reinterpret_cast<const LongString*>(
                                            mem_char_ptr(str_addr));
            line.text = std::string(indent, ' ') + "S\" " + message->to_string() + "\"";
        }
        else if (xt == xtXABORT_QUOTE) {
            int str_addr = fetch(ptr);
            ptr += CELL_SZ;
            const LongString* message = reinterpret_cast<const LongString*>(
                                            mem_char_ptr(str_addr));
            line.text = std::string(indent, ' ') + "ABORT\" " + message->to_string() + "\"";
        }
        else if (xt == xtXC_QUOTE) {
            int str_addr = fetch(ptr);
            ptr += CELL_SZ;
            const CString* message = reinterpret_cast<const CString*>(
                                         mem_char_ptr(str_addr));
            line.text = std::string(indent, ' ') + "C\" " + message->to_string() + "\"";
        }
        else if (xt == xtXDOES_DEFINE) {
            fetch(ptr);     // int creator_xt =
            ptr += CELL_SZ;
            int run_code = fetch(ptr);
            ptr += CELL_SZ;
            line.target_addr = run_code;
            line.text = std::string(indent, ' ') + "DOES>";
        }
        else if (xt == xtXDO) {
            int dist = fetch(ptr);
            line.target_addr = ptr + dist;
            ptr += CELL_SZ;
            line.text = std::string(indent, ' ') + "DO";
            indent += 2;
        }
        else if (xt == xtXQUERY_DO) {
            int dist = fetch(ptr);
            line.target_addr = ptr + dist;
            ptr += CELL_SZ;
            line.text = std::string(indent, ' ') + "?DO";
            indent += 2;
        }
        else if (xt == xtXLOOP) {
            int dist = fetch(ptr);
            line.target_addr = ptr + dist;
            ptr += CELL_SZ;
            indent -= 2;
            line.text = std::string(indent, ' ') + "LOOP";
        }
        else if (xt == xtXPLUS_LOOP) {
            int dist = fetch(ptr);
            line.target_addr = ptr + dist;
            ptr += CELL_SZ;
            indent -= 2;
            line.text = std::string(indent, ' ') + "+LOOP";
        }
        else if (xt == xtXLEAVE) {
            int dist = fetch(ptr);
            line.target_addr = ptr + dist;
            ptr += CELL_SZ;
            line.text = std::string(indent, ' ') + "LEAVE";
        }
        else if (xt == xtXUNLOOP) {
            line.text = std::string(indent, ' ') + "UNLOOP";
        }
        else if (xt == xtXOF) {
            int dist = fetch(ptr);
            line.target_addr = ptr + dist;
            ptr += CELL_SZ;
            line.text = std::string(indent, ' ') + "OF";
        }
        else {
            line.text = std::string(indent, ' ') + header->name()->to_string();
        }

        lines.push_back(line);
    }
    return lines;
}

static void mark_labels(std::vector<Line>& lines) {
    int label_id = 1;
    for (auto& line : lines) {
        if (line.target_addr != 0) {
            auto it = find_if(lines.begin(), lines.end(),
            [&line](const Line & l) {
                return l.addr == line.target_addr;
            });
            if (it != lines.end()) {
                if (it->label_id == 0) {
                    it->label_id = label_id++;
                }
                line.text += " L" + std::to_string(it->label_id);
            }
        }
    }
}

static void dump_colon_definition(uint body, uint size) {
    std::vector<Line> lines = disassemble(body, size);
    mark_labels(lines);

    for (const auto& line : lines) {
        if (line.label_id != 0) {
            std::cout << "L" << line.label_id << ":" << std::endl;
        }
        std::cout << "    " << line.text << std::endl;
    }
    std::cout << ";" << std::endl;
}

void dump_body_definition(uint body, uint size) {
    f_dump(mem_char_ptr(body, size), size);
}

void f_see() {
    Header* header = vm.dict.parse_find_existing_word();
    assert(header != nullptr);
    uint xt = header->xt();
    uint size = header->get_size();
    uint body = xt + CELL_SZ;
    uint code = header->code;
    std::string name = header->name()->to_string();

    switch (code) {
    case idXDOCOL:
        std::cout << std::endl << ": " << name << std::endl;
        dump_colon_definition(body, size);
        break;
    case idXDOVAR:
        if (size == CELL_SZ) {
            int value = fetch(body);
            std::cout << std::endl << "VARIABLE " << name << BL;
            print_number(value);
            std::cout << name << BL << "!" << std::endl;
        }
        else if (size == DCELL_SZ) {
            dint value = dfetch(body);
            std::cout << std::endl << "2VARIABLE " << name << BL;
            print_number_dot(value);
            std::cout << name << BL << "2!" << std::endl;
        }
        else {
            std::cout << std::endl << "CREATE " << name << BL;
            dump_body_definition(body, size);
        }
        break;
    case idXDOFVAR:
        if (size == FCELL_SZ) {
            double value = ffetch(body);
            std::cout << std::endl << "FVARIABLE " << name << BL;
            print_number_e(value);
            std::cout << name << BL << "F!" << std::endl;
        }
        else {
            std::cout << std::endl << "CREATE " << name << BL;
            dump_body_definition(body, size);
        }
        break;
    case idXDOCONST: {
        int value = fetch(body);
        std::cout << std::endl;
        print_number(value);
        std::cout << "CONSTANT " << name << std::endl;

        if (size > CELL_SZ) {
            dump_body_definition(body + CELL_SZ, size - CELL_SZ);
        }
        break;
    }
    case idXDO2CONST: {
        dint value = dfetch(body);
        std::cout << std::endl;
        print_number_dot(value);
        std::cout << "2CONSTANT " << name << std::endl;

        if (size > DCELL_SZ) {
            dump_body_definition(body + DCELL_SZ, size - DCELL_SZ);
        }
        break;
    }
    case idXDOFCONST: {
        double value = ffetch(body);
        std::cout << std::endl;
        print_number_e(value);
        std::cout << "FCONSTANT " << name << std::endl;

        if (size > FCELL_SZ) {
            dump_body_definition(body + FCELL_SZ, size - FCELL_SZ);
        }
        break;
    }
    case idXMARKER:
        std::cout << std::endl << "MARKER " << name << std::endl
                  << "Latest: ";
        print_number(fetch(body));
        std::cout << std::endl
                  << "Here:   ";
        print_number(fetch(body + CELL_SZ));
        std::cout << std::endl
                  << "Names:  ";
        print_number(fetch(body + 2 * CELL_SZ));
        std::cout << std::endl;
        if (size > 3 * CELL_SZ) {
            dump_body_definition(body + 3 * CELL_SZ, size - 3 * CELL_SZ);
        }
        break;
    case idXDEFER:
        if (size == CELL_SZ) {
            int action_xt = fetch(body);
            Header* action_header = Header::header(action_xt);
            std::cout << std::endl << "DEFER " << name << BL
                      << "ACTION OF " << action_header->name()->to_string() << std::endl;
        }
        if (size > CELL_SZ) {
            dump_body_definition(body + CELL_SZ, size - CELL_SZ);
        }
        break;
    case idXDOES_RUN: {
        Header* creator_header = Header::header(header->creator_xt);
        std::cout << std::endl << creator_header->name()->to_string() << BL << name;
        dump_body_definition(body, size);
        break;
    }
    case idXPLUS_FIELD:
        if (size == CELL_SZ) {
            uint offset = fetch(body);
            std::cout << std::endl << "FIELD " << name << BL
                      << "OFFSET " << offset << std::endl;
        }
        if (size > CELL_SZ) {
            dump_body_definition(body + CELL_SZ, size - CELL_SZ);
        }
        break;
    case idXSYNONYM: {
        if (size == CELL_SZ) {
            uint old_xt = fetch(body);
            Header* old_header = Header::header(old_xt);
            std::cout << std::endl << "SYNONYM " << name << BL
                      << old_header->name()->to_string() << std::endl;
        }
        if (size > CELL_SZ) {
            dump_body_definition(body + CELL_SZ, size - CELL_SZ);
        }
        break;
    }
    default: {
        std::cout << std::endl << name << std::endl;
        dump_body_definition(body, size);
        break;
    }
    }
}

void f_n_to_r() {
    uint n = pop();
    for (uint i = 0; i < n; ++i) {
        r_push(pop());
    }
    r_push(n);
}

void f_n_r_from() {
    uint n = r_pop();
    for (uint i = 0; i < n; ++i) {
        push(r_pop());
    }
}

void f_forget() {
    Header* header = vm.dict.parse_find_existing_word();
    assert(header != nullptr);
    vm.here = mem_addr(reinterpret_cast<char*>
                       (header)); // reset here to reclaim memory
    vm.latest = header->link; // point to previous word
    Header* latest = reinterpret_cast<Header*>(mem_char_ptr(vm.latest));
    vm.names = latest->name_addr;
}

void f_to_name() {
    uint xt = pop();
    Header* header = Header::header(xt);
    uint nt = mem_addr(reinterpret_cast<char*>(header));
    push(nt);
}

void f_name_to_compile() {
    uint name_addr = pop();
    Header* header = reinterpret_cast<Header*>(
                         mem_char_ptr(name_addr));
    if (header->flags.immediate) {
        push(header->xt());
        push(xtEXECUTE);
    }
    else {
        push(header->xt());
        push(xtCOMPILE_COMMA);
    }
}

void f_name_to_string() {
    uint name_addr = pop();
    Header* header = reinterpret_cast<Header*>(
                         mem_char_ptr(name_addr));
    CString* name = reinterpret_cast<CString*>(
                        mem_char_ptr(header->name_addr));
    push(mem_addr(name->str()));
    push(name->size());
}

void f_name_to_interpret() {
    uint name_addr = pop();
    Header* header = reinterpret_cast<Header*>(
                         mem_char_ptr(name_addr));
    uint xt = header->xt();
    push(xt);
}

void f_synonym() {
    uint new_xt = vm.dict.parse_create(idXSYNONYM, 0);
    Header* new_header = Header::header(new_xt);

    Header* old_header = vm.dict.parse_find_existing_word();
    assert(old_header != nullptr);

    comma(old_header->xt());
    if (old_header->flags.immediate) {
        new_header->flags.immediate = true;
    }
}

void f_xsynonym(uint body) {
    uint old_xt = fetch(body);
    Header* old_header = Header::header(old_xt);
    if (old_header->flags.immediate ||
            vm.user->STATE == STATE_INTERPRET) {
        f_execute(old_xt);
    }
    else {
        fcomma(old_xt);
    }
}

void f_traverse_wordlist() {
    uint wid = pop();
    uint xt = pop();

    std::vector<uint> nts = vm.dict.get_word_nts(wid);
    for (auto nt : nts) {
        push(nt);
        f_execute(xt);
        bool continue_traversal = pop();
        if (!continue_traversal) {
            break;
        }
    }
}

void f_bracket_defined() {
    Header* header = vm.dict.parse_find_word();
    if (header != nullptr) {
        push(F_TRUE);
    }
    else {
        push(F_FALSE);
    }
}

void f_bracket_undefined() {
    Header* header = vm.dict.parse_find_word();
    if (header == nullptr) {
        push(F_TRUE);
    }
    else {
        push(F_FALSE);
    }
}

static void update_cond_status() {
    for (int i = 0; i < vm.cond_stack.depth(); ++i) {
        if (vm.cond_stack.peek(i) == 0) {
            vm.cond_status = false;
            return;
        }
    }
    vm.cond_status = true;
}

void f_bracket_if() {
    int flag = pop();
    vm.cond_stack.push(flag);
    update_cond_status();
}

void f_bracket_else() {
    int flag = !vm.cond_stack.pop();
    vm.cond_stack.push(flag);
    update_cond_status();
}

void f_bracket_then() {
    vm.cond_stack.pop();
    update_cond_status();
}

void start_cond_compilation() {
    vm.cond_stack.clear();
    vm.cond_status = true;
}

void check_end_cond_compilation() {
    if (vm.cond_stack.depth() != 0) {
        error(Error::UnmatchedConditionalCompilation);
    }
}
