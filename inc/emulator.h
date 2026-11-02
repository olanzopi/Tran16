#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdint.h>
#include <random>
#include <string>

#include "state.h"
#include "renderer.h"

struct emulator_t
{
    uint32_t* display = (uint32_t*)malloc(WIDTH * HEIGHT * sizeof(uint32_t));
    void draw_pixel(uint32_t* display, uint16_t x, uint16_t y, uint32_t color) {
        if (x >= WIDTH || y >= HEIGHT) return;
        display[(y * WIDTH) + x] = color;
    }
    uint8_t keys[256];
    bool draw_flag = false;

    uint8_t* ram = (uint8_t*)malloc(ram_size);

    uint16_t registers[256]; //12r 13g 14b reserved for display

    uint16_t delay_timer = 0;
    uint16_t sound_timer = 0;

    uint16_t program_counter = 0;
    uint16_t address_reg = 0;

    uint32_t stack[16];
    uint16_t stack_pointer = 0;


    struct config_t {
        std::string path; //rom path
        float CPU_HZ = 5000.0f; //instructions ps
        float TIMER_HZ = 60.0f; //timer and display hz
    } config;

    void set_addr_reg(uint16_t address) {
        address_reg = address & 0xFFFF;
    };

    void skip_next() {
        program_counter += 0x4;
    }

    void reset() {
        std::fill_n(ram, ram_size, 0);
        std::fill_n(registers, 16, 0);
        std::fill_n(stack, 16, 0);
        
        delay_timer = 0;
        sound_timer = 0;
        address_reg = 0;
        stack_pointer = 0;
        program_counter = 0; 
    }

    //random function. icl i dont got a clue how it works.
    int randint(int nnnn) {
        static std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<int> dist(0, nnnn);
        return dist(gen);
    }
};

void load_config(emulator_t& emulator);
bool parse_binary(emulator_t& emulator);
void next_instruction(emulator_t& emulator);

#endif