#include <vector>
#include <stdint.h>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include "platform.h"
#include "disassembler.h"
#include "opcodes.h"

std::string get_v(int i) {
    return std::to_string(i);
}

static const std::unordered_map<uint8_t, std::pair<std::string, inst_def>>& get_reverse_table() {
    static std::unordered_map<uint8_t, std::pair<std::string, inst_def>> reverse_table;
    if (reverse_table.empty()) {
        for (const auto& entry : instruction_table) {
            if (entry.first == "raw") continue; // raw is an assembler-only pseudo-instruction, not decodable
            uint8_t type = static_cast<uint8_t>((entry.second.base >> 24) & 0xFF);
            reverse_table[type] = { entry.first, entry.second };
        }
    }
    return reverse_table;
}

bool disassemble_binary(std::string path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate); //binary no text. ate to open at end

    if (!file.is_open()) {
        log_fatal("could not open rom");
        return false;
    }

    std::streamsize size = file.tellg(); //opened with ate so we are at the end. hence our index is the last byte thus file size
    file.seekg(0, std::ios::beg); //go back!

    //cant be too big
    if (size > 0x10000) {
        log_fatal("rom too large.");
        return false;
    }

    uint8_t ram[0x10000];
    file.read(reinterpret_cast<char*>(&ram[0]), size);
    file.close();

    //rom is now parsed. we will now disassemble it and write it to rompath.t16s

    std::ofstream disassembled_file;
    disassembled_file.open(path + ".t16s");
    //disassembled_file << "decompiled " + path << std::endl;

    const auto& reverse_table = get_reverse_table();

    for(int i = 0; i + 3 < size; i += 4) {
        uint32_t opcode = ((uint32_t)ram[i] << 24) |
                      ((uint32_t)ram[i + 1] << 16) |
                      ((uint32_t)ram[i + 2] << 8) |
                       (uint32_t)ram[i + 3];
        uint8_t  type = (opcode & 0xFF000000) >> 24;
        uint8_t  xx   = (opcode & 0x00FF0000) >> 16;
        uint8_t  yy   = (opcode & 0x0000FF00) >> 8;
        uint16_t nnnn = (opcode & 0x0000FFFF);
        //std::cout << std::showbase << std::hex << opcode << std::endl;

        std::string instruction = "";

        auto it = reverse_table.find(type);
        if (it != reverse_table.end()) {
            const std::string& name = it->second.first;
            const inst_def& def = it->second.second;

            switch (def.format)
            {
            case 'N':
                instruction = name;
                break;
            case 'x':
                instruction = name + " " + get_v(xx);
                break;
            case 'y':
                instruction = name + " " + get_v(xx) + " " + get_v(yy);
                break;
            case 'n':
                instruction = name + " " + get_v(xx) + " " + std::to_string(nnnn);
                break;
            case 'j':
                instruction = name + " " + std::to_string(nnnn);
                break;
            default:
                break;
            }
        }

        disassembled_file << instruction << std::endl;
    }
    disassembled_file.close();
    return true;
}