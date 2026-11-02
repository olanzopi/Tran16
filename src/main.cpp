#include "state.h"
#include "renderer.h"
#include "emulator.h"

#include <platform.h>

#include <chrono>
#include <thread>

#include <vector>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <cstring>

void sleep(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int main() {
    emulator_t emulator;
    emulator.reset();
    load_config(emulator);
    parse_binary(emulator);
    //memcpy(&emulator.ram[0x0050], emulator.chip8_fontset, sizeof(emulator.chip8_fontset));

    window_t* window = create_window("tran16", 100, 100, WIDTH, HEIGHT, emulator.display);
    
    using clock = std::chrono::high_resolution_clock;
    auto last_time = clock::now();

    float TIMER_INTERVAL = 1.0f / emulator.config.TIMER_HZ;

    float cpu_accumulator = 0.0f;
    float timer_accumulator = 0.0f;

    bool is_running = true;
    while (is_running) {        
        auto current_time = clock::now();
        std::chrono::duration<float> elapsed = current_time - last_time;
        last_time = current_time;

        float delta_time = elapsed.count();
        cpu_accumulator += delta_time;
        timer_accumulator += delta_time;

        is_running = poll_events(window);

        std::fill_n(emulator.keys, 16, 0); // clear key array
        emulator.keys[0] = is_key_down(window, 48);
        emulator.keys[1] = is_key_down(window, 119); // up 49
        emulator.keys[2] = is_key_down(window, 50);
        emulator.keys[3] = is_key_down(window, 51);
        emulator.keys[4] = is_key_down(window, 115); // down 113
        emulator.keys[5] = is_key_down(window, 119);
        emulator.keys[6] = is_key_down(window, 101);
        emulator.keys[7] = is_key_down(window, 114);
        emulator.keys[8] = is_key_down(window, 97);
        emulator.keys[9] = is_key_down(window, 115);
        emulator.keys[10] = is_key_down(window, 100);
        emulator.keys[11] = is_key_down(window, 102);
        emulator.keys[12] = is_key_down(window, 65362); // up 122
        emulator.keys[13] = is_key_down(window, 65364); // down 120
        emulator.keys[14] = is_key_down(window, 99);
        emulator.keys[15] = is_key_down(window, 118);
        
        float cycles_to_run = cpu_accumulator * emulator.config.CPU_HZ;
        int instructions = static_cast<int>(cycles_to_run);
        
        for (int i = 0; i < instructions; i++) {
            next_instruction(emulator);
        }
        cpu_accumulator -= (instructions / emulator.config.CPU_HZ);

        while (timer_accumulator >= TIMER_INTERVAL) {
            if (emulator.delay_timer > 0) emulator.delay_timer--;
            if (emulator.sound_timer > 0) emulator.sound_timer--;
            timer_accumulator -= TIMER_INTERVAL;
        }

        if (emulator.draw_flag) {
            draw(window);
            emulator.draw_flag = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return 0;
}