#include <vector>
#include <stdint.h>
#include <string>
#include <fstream>
#include <iostream>

#include "emulator.h"
#include "platform.h"
#include "renderer.h"

void load_config(emulator_t& emulator) {
    std::ifstream file("config.txt");

    if (!file.is_open()) {
        log_warning("config.txt not found, using defaults");
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue; //skip blanks/comments

        size_t eq_pos = line.find('=');
        if (eq_pos == std::string::npos) continue; //malformed, skip

        std::string key = line.substr(0, eq_pos);
        std::string value = line.substr(eq_pos + 1);

        auto trim = [](std::string& s) {
            s.erase(0, s.find_first_not_of(" \t\r\n"));
            s.erase(s.find_last_not_of(" \t\r\n") + 1);
        };
        trim(key);
        trim(value);

        if (key == "rom_name") {
            emulator.config.path = "games/" + value;
        }
        else if (key == "cpu_hz") {
            emulator.config.CPU_HZ = std::stof(value);
        }
        else if (key == "timer_hz") {
            emulator.config.TIMER_HZ = std::stof(value);
        }
    }
}

bool parse_binary(emulator_t& emulator) {
    std::ifstream file(emulator.config.path, std::ios::binary | std::ios::ate); //binary no text. ate to open at end

    if (!file.is_open()) {
        log_fatal("could not open rom");
        return false;
    }

    std::streamsize size = file.tellg(); //opened with ate so we are at the end. hence our index is the last byte thus file size
    file.seekg(0, std::ios::beg); //go back!

    //cant be too big
    if (size > ram_size) {
        log_fatal("rom too large.");
        return false;
    }

    file.read(reinterpret_cast<char*>(&emulator.ram[0]), size);

    file.close();
    return true;
}

void next_instruction(emulator_t& emulator) {
    uint32_t opcode = ((uint32_t)emulator.ram[emulator.program_counter] << 24) |
                      ((uint32_t)emulator.ram[emulator.program_counter + 1] << 16) |
                      ((uint32_t)emulator.ram[emulator.program_counter + 2] << 8) |
                       (uint32_t)emulator.ram[emulator.program_counter + 3];
    uint8_t  type = (opcode & 0xFF000000) >> 24;
    uint8_t  xx   = (opcode & 0x00FF0000) >> 16;
    uint8_t  yy    = (opcode & 0x0000FF00) >> 8;
    uint16_t nnnn = (opcode & 0x0000FFFF);

    uint16_t& vx = emulator.registers[xx];
    uint16_t& vy = emulator.registers[yy];

    emulator.skip_next();

    switch (type)
    {
        //routine
        case 0x00: //subroutine at NNNN
            emulator.stack[emulator.stack_pointer] = emulator.program_counter;
            emulator.stack_pointer++;
            emulator.program_counter = nnnn;
            break;
        case 0x01: //return from subroutine
            emulator.stack_pointer--;
            emulator.program_counter = emulator.stack[emulator.stack_pointer];
            break;

        //jumps
        case 0x10: //jump to NNNN
            emulator.program_counter = nnnn;
            break;
        case 0x11: //jump to NNNN + VX
            emulator.program_counter = nnnn + vx;
            break;

        //skip next if
        case 0x20: //VX equal NNNN
            if(vx == nnnn) { emulator.skip_next(); }
            break;
        case 0x21: //VX not equal NNNN
            if(vx != nnnn) { emulator.skip_next(); }
            break;
        case 0x22: //VX equal VY
            if(vx == vy) { emulator.skip_next(); }
            break;
        case 0x23: //VX not equal VY
            if(vx != vy) { emulator.skip_next(); }
            break;
        case 0x24: //VX bigger then NNNN
            if(vx > nnnn) { emulator.skip_next(); }
            break;
        case 0x25: //VX smaller then NNNN
            if(vx < nnnn) { emulator.skip_next(); }
            break;
        case 0x26: //VX bigger then VY
            if(vx > vy) { emulator.skip_next(); }
            break;
        case 0x27: //VX smaller then VY
            if(vx < vy) { emulator.skip_next(); }
            break;

        //sets
        case 0x30: //VX to NNNN
            vx = nnnn;
            break;
        case 0x31: //VX to VY
            vx = vy;
            break;
        case 0x32: //VX or VY
            vx |= vy;
            break;
        case 0x33: //VX and VY
            vx &= vy;
            break;
        case 0x34: //VX xor VY
            vx ^= vy;
            break;
        case 0x35: //I to NNNN
            emulator.set_addr_reg(nnnn);
            break;

        //math
        case 0x40: //add NNNN to VX
            vx += nnnn;
            break;
        case 0x41: //add VY to VX
            vx += vy;
            break;
        case 0x42: //subtract VY from VX
            vx -= vy;
            break;
        case 0x43: //set VX to random between 0-NNNN
            vx = emulator.randint(nnnn);
            break;
        case 0x44: //add VX to I
            emulator.set_addr_reg(emulator.address_reg + vx);
            break;

        //shift
        case 0x50: //shift vx NNNN to the right
            vx >>= nnnn;
            break;
        case 0x51: //shift vx NNNN to the left
            vx <<= nnnn;
            break;

        //drawing
        case 0x60: //clear display
            emulator.draw_flag = true;
            std::fill_n(emulator.display, (WIDTH * HEIGHT), 0);
            break;
        case 0x61: //draw pixel at VX, VY. colors are registers r10 g11 b12.
            emulator.draw_flag = true;
            emulator.draw_pixel(emulator.display, vx, vy, rgb_to_hex(
                emulator.registers[10], //r
                emulator.registers[11], //g
                emulator.registers[12]  //b
            ));
            break;

        //keys
        case 0x70: //skip next if KX pressed
            if(emulator.keys[xx] == 1) { emulator.skip_next(); }
            break;
        case 0x71: //skip next if KX not pressed
            if(emulator.keys[xx] != 1) { emulator.skip_next(); }
            break;

        //timers
        case 0x80: //set delay timer to NNNN
            emulator.delay_timer = nnnn;
            break;
        case 0x81: //add NNNN to delay timer
            emulator.delay_timer += nnnn;
            break;

        //memory
        case 0x90: //store vx to vy starting at i (including both)
            for (uint16_t i = xx; i <= yy; i++) {
                uint16_t offset = (i - xx);
                emulator.ram[emulator.address_reg + offset] = (uint8_t)(emulator.registers[i] & 0xFF);
            }
            break;
        case 0x91: //load vx to vy starting at i (including both)
            for (uint16_t i = xx; i <= yy; i++) {
                uint16_t offset = (i - xx);
                emulator.registers[i] = emulator.ram[emulator.address_reg + offset];
            }
            break;
    }
}