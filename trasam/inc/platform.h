#include <stdint.h>
#include <string>
#include <iostream>

inline void log_fatal(std::string error) {
    std::cout << "TRASAM (FATAL!): " << error << std::endl;
}
inline void log_warning(std::string error) {
    std::cout << "TRASAM (WARNING): " << error << std::endl;
}
inline void log_debug(std::string error) {
    std::cout << "TRASAM (DEBUG): " << error << std::endl;
}
template <typename T>
void log_debug_value(std::string error, T value) {
    std::cout << "DEBUG: " << error << value << std::endl;
}