//-----------------------------------------------------------------------------
// C++ implementation of a Forth interpreter
// Copyright (c) Paulo Custodio, 2020-2025
// License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
//-----------------------------------------------------------------------------

#include "tools.h"
#include "vm.h"
#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
using namespace std;

void f_dump() {
    int size = pop();
    int addr = pop();
    const char* mem = mem_char_ptr(addr, size);
    f_dump(mem, size);
}

void f_dump(const char* mem, int size) {
    int addr = mem_addr(mem);
    int addr_lo = addr & ~0xF;
    int addr_hi = (addr + size + 15) & ~0xF;
    for (int p = addr_lo; p < addr_hi; p += 16) {
        cout << endl << hex << setfill('0') << setw(8) << p << BL << BL;
        for (int q = p; q < p + 16; ++q) {
            if (q < addr || q >= addr + size) {
                cout << BL << BL << BL;
            }
            else {
                cout << hex << setfill('0') << setw(2) << cfetch(q) << BL;
            }
        }
        cout << BL << BL;
        for (int q = p; q < p + 16; ++q) {
            if (q < addr || q >= addr + size) {
                cout << BL;
            }
            else {
                char c = cfetch(q);
                cout << (isprint(c) ? c : '.');
            }
        }
    }
    cout << endl << dec << setfill(' ') << setw(0);
}

struct Line {
    int label_id{ 0 };
    int addr{ 0 };
    int target_addr{ 0 };
    string text;
};

static vector<Line> disassemble(int body, int size) {
    vector<Line> lines;

    int ptr = body;
    int indent = 0;
    while (ptr < body + size) {
        Line line;
        line.addr = ptr;

        int xt = fetch(ptr);
        ptr += CELL_SZ;
        Header* header = Header::header(xt);
        if (xt == xtXLITERAL) {
            int value = fetch(ptr);
            ptr += CELL_SZ;
            line.text = string(indent, ' ') + number_to_string(value);
        }
        else if (xt == xtX2LITERAL) {
            dint value = dfetch(ptr);
            ptr += DCELL_SZ;
            line.text = string(indent, ' ') + number_dot_to_string(value);
        }
        else if (xt == xtBRANCH || xt == xtZBRANCH) {
            int dist = fetch(ptr);
            line.target_addr = ptr + dist;
            ptr += CELL_SZ;
            line.text = string(indent, ' ') + header->name()->to_string();
        }
        else if (xt == xtXDOT_QUOTE) {
            int str_addr = fetch(ptr);
            ptr += CELL_SZ;
            const LongString* message = reinterpret_cast<const LongString*>(
                                            mem_char_ptr(str_addr));
            line.text = string(indent, ' ') + ".\" " + message->to_string() + "\"";
        }
        else if (xt == xtXS_QUOTE) {
            int str_addr = fetch(ptr);
            ptr += CELL_SZ;
            const LongString* message = reinterpret_cast<const LongString*>(
                                            mem_char_ptr(str_addr));
            line.text = string(indent, ' ') + "S\" " + message->to_string() + "\"";
        }
        else if (xt == xtXABORT_QUOTE) {
            int str_addr = fetch(ptr);
            ptr += CELL_SZ;
            const LongString* message = reinterpret_cast<const LongString*>(
                                            mem_char_ptr(str_addr));
            line.text = string(indent, ' ') + "ABORT\" " + message->to_string() + "\"";
        }
        else if (xt == xtXC_QUOTE) {
            int str_addr = fetch(ptr);
            ptr += CELL_SZ;
            const CString* message = reinterpret_cast<const CString*>(
                                         mem_char_ptr(str_addr));
            line.text = string(indent, ' ') + "C\" " + message->to_string() + "\"";
        }
        else if (xt == xtXDOES_DEFINE) {
            fetch(ptr);     // int creator_xt =
            ptr += CELL_SZ;
            int run_code = fetch(ptr);
            ptr += CELL_SZ;
            line.target_addr = run_code;
            line.text = string(indent, ' ') + "DOES>";
        }
        else if (xt == xtXDO) {
            int dist = fetch(ptr);
            line.target_addr = ptr + dist;
            ptr += CELL_SZ;
            line.text = string(indent, ' ') + "DO";
            indent += 2;
        }
        else if (xt == xtXQUERY_DO) {
            int dist = fetch(ptr);
            line.target_addr = ptr + dist;
            ptr += CELL_SZ;
            line.text = string(indent, ' ') + "?DO";
            indent += 2;
        }
        else if (xt == xtXLOOP) {
            int dist = fetch(ptr);
            line.target_addr = ptr + dist;
            ptr += CELL_SZ;
            indent -= 2;
            line.text = string(indent, ' ') + "LOOP";
        }
        else if (xt == xtXPLUS_LOOP) {
            int dist = fetch(ptr);
            line.target_addr = ptr + dist;
            ptr += CELL_SZ;
            indent -= 2;
            line.text = string(indent, ' ') + "+LOOP";
        }
        else if (xt == xtXLEAVE) {
            int dist = fetch(ptr);
            line.target_addr = ptr + dist;
            ptr += CELL_SZ;
            line.text = string(indent, ' ') + "LEAVE";
        }
        else if (xt == xtXUNLOOP) {
            line.text = string(indent, ' ') + "UNLOOP";
        }
        else if (xt == xtXOF) {
            int dist = fetch(ptr);
            line.target_addr = ptr + dist;
            ptr += CELL_SZ;
            line.text = string(indent, ' ') + "OF";
        }
        else {
            line.text = string(indent, ' ') + header->name()->to_string();
        }

        lines.push_back(line);
    }
    return lines;
}

static void mark_labels(vector<Line>& lines) {
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
                line.text += " L" + to_string(it->label_id);
            }
        }
    }
}

static void dump_colon_definition(int body, int size) {
    vector<Line> lines = disassemble(body, size);
    mark_labels(lines);

    for (const auto& line : lines) {
        if (line.label_id != 0) {
            cout << "L" << line.label_id << ":" << endl;
        }
        cout << "    " << line.text << endl;
    }
    cout << ";" << endl;
}

void dump_body_definition(int body, int size) {
    f_dump(mem_char_ptr(body, size), size);
}

void f_see() {
    Header* header = vm.dict->parse_find_existing_word();
    assert(header != nullptr);
    int xt = header->xt();
    int size = header->get_size();
    int body = xt + CELL_SZ;
    int code = header->code;
    string name = header->name()->to_string();

    switch (code) {
    case idXDOCOL:
        cout << endl << ": " << name << endl;
        dump_colon_definition(body, size);
        break;
    case idXDOVAR:
        if (size == CELL_SZ) {
            int value = fetch(body);
            cout << endl << "VARIABLE " << name << BL;
            print_number(value);
            cout << name << BL << "!" << endl;
        }
        else if (size == DCELL_SZ) {
            dint value = dfetch(body);
            cout << endl << "2VARIABLE " << name << BL;
            print_number_dot(value);
            cout << name << BL << "2!" << endl;
        }
        else {
            cout << endl << "CREATE " << name << BL;
            dump_body_definition(body, size);
        }
        break;
    case idXDOCONST: {
        int value = fetch(body);
        cout << endl;
        print_number(value);
        cout << "CONSTANT " << name << endl;

        if (size > CELL_SZ) {
            dump_body_definition(body + CELL_SZ, size - CELL_SZ);
        }
        break;
    }
    case idXDO2CONST: {
        dint value = dfetch(body);
        cout << endl;
        print_number_dot(value);
        cout << "2CONSTANT " << name << endl;

        if (size > DCELL_SZ) {
            dump_body_definition(body + DCELL_SZ, size - DCELL_SZ);
        }
        break;
    }
    case idXMARKER:
        cout << endl << "MARKER " << name << endl
             << "Latest: ";
        print_number(fetch(body));
        cout << endl
             << "Here:   ";
        print_number(fetch(body + CELL_SZ));
        cout << endl
             << "Names:  ";
        print_number(fetch(body + 2 * CELL_SZ));
        cout << endl;
        if (size > 3 * CELL_SZ) {
            dump_body_definition(body + 3 * CELL_SZ, size - 3 * CELL_SZ);
        }
        break;
    case idXDEFER:
        if (size == CELL_SZ) {
            int action_xt = fetch(body);
            Header* action_header = Header::header(action_xt);
            cout << endl << "DEFER " << name << BL
                 << "ACTION OF " << action_header->name()->to_string() << endl;
        }
        if (size > CELL_SZ) {
            dump_body_definition(body + CELL_SZ, size - CELL_SZ);
        }
        break;
    case idXDOES_RUN: {
        Header* creator_header = Header::header(header->creator_xt);
        cout << endl << creator_header->name()->to_string() << BL << name;
        dump_body_definition(body, size);
        break;
    }
    case idXPLUS_FIELD:
        if (size == CELL_SZ) {
            int offset = fetch(body);
            cout << endl << "FIELD " << name << BL
                 << "OFFSET " << offset << endl;
        }
        if (size > CELL_SZ) {
            dump_body_definition(body + CELL_SZ, size - CELL_SZ);
        }
        break;
    default: {
        cout << endl << name << endl;
        dump_body_definition(body, size);
        break;
    }
    }
}
