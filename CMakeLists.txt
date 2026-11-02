cmake_minimum_required(VERSION 3.10)
project(tran16)

find_package(X11 REQUIRED COMPONENTS Xrandr)

#tran16
file(GLOB TRAN16_SOURCES "src/*.cpp")
add_executable(tran16 ${TRAN16_SOURCES})
target_link_libraries(tran16 PRIVATE X11::X11 X11::Xrandr)
target_include_directories(tran16 PRIVATE inc)

#trasam
file(GLOB TRASAM_SOURCES "trasam/src/*.cpp")
add_executable(trasam ${TRASAM_SOURCES})
target_include_directories(trasam PRIVATE trasam/inc)