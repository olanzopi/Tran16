#include <vector>
#include <stdint.h>
#include <string>
#include <fstream>
#include <iostream>
#include <cstring>
#include <sstream>
#include <unordered_map>

#include "platform.h"
#include "assembler.h"
#include "opcodes.h"

std::vector<std::string> tokenizer(std::string line) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;

    while (ss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

bool assemble_script(std::string path) {
    std::ifstream assembly(path);
    if (!assembly.is_open()) {
        log_fatal("could not open script");
        return false;
    }

    //remove comments
    std::stringstream clean;
    std::string line;
    while (std::getline(assembly, line)) {
        size_t comment_pos = line.find("//");
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        clean << line << ' ';
    }

    std::ofstream out(path + ".t16", std::ios::binary);

    std::string mnemonic;
    while(clean >> mnemonic) {
        auto it = instruction_table.find(mnemonic);
        if (it == instruction_table.end()) {
            log_fatal("unknown instruction: " + mnemonic);
            return false;
        }

        const inst_def& def = it->second;
        int x = 0, y = 0, nnnn = 0;
        std::string tok;

        uint32_t opcode = 0;
        bool is_raw = (def.format == 'r');

        switch (def.format) {
            case 'N':
                break;
            case 'x':
                clean >> tok; x = std::stoi(tok);
                break;
            case 'y':
                clean >> tok; x = std::stoi(tok);
                clean >> tok; y = std::stoi(tok);
                break;
            case 'n':
                clean >> tok; x    = std::stoi(tok);
                clean >> tok; nnnn = std::stoi(tok);
                break;
            case 'j':
                clean >> tok; nnnn = std::stoi(tok);
                break;
            case 'r':
                clean >> tok;
                opcode = static_cast<uint32_t>(std::stoul(tok, nullptr, 16)); // base 16!
                break;
        }

        if(!is_raw) {
            opcode = def.base
                | ((x & 0xF) << 16)
                | ((y & 0xF) << 8)
                | (nnnn & 0xFFFF);
        }

        out.put(static_cast<char>((opcode >> 24) & 0xFF));
        out.put(static_cast<char>((opcode >> 16) & 0xFF));
        out.put(static_cast<char>((opcode >> 8) & 0xFF));
        out.put(static_cast<char>(opcode & 0xFF));
    }

    out.close();
    return true;
}