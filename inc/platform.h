#pragma once

#include <stdint.h>
#include <string>
#include <iostream>

inline void log_fatal(std::string error) {
    std::cout << "SCHWM (FATAL!): " << error << std::endl;
}
inline void log_warning(std::string error) {
    std::cout << "SCHWM (WARNING): " << error << std::endl;
}
inline void log_debug(std::string error) {
    std::cout << "SCHWM (DEBUG): " << error << std::endl;
}
template <typename T>
void log_debug_value(std::string error, T value) {
    std::cout << "DEBUG: " << error << value << std::endl;
}

struct window_t;

window_t* create_window(std::string name, uint16_t pos_x, uint16_t pos_y, uint16_t size_x, uint16_t size_y, uint32_t* pixel_buffer);
void draw(window_t* window);
bool poll_events(window_t* window);

bool is_key_down(window_t* window, uint16_t key_code);
bool is_mouse_button_down(window_t* window, unsigned int button);
void get_mouse_pos(window_t* window, int* x, int* y);