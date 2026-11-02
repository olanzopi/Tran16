#pragma once
#include <unordered_map>
#include <string>
#include <cstdint>

struct inst_def {
    uint32_t base;
    int parameters;
    char format;
};

inline const std::unordered_map<std::string, inst_def> instruction_table = {
    {"sub",      {0x00000000, 1, 'j'}},
    {"return",   {0x01000000, 0, 'N'}},
    {"jmp",      {0x10000000, 1, 'j'}},
    {"jmpvx",    {0x11000000, 2, 'n'}},
    {"snvxe",    {0x20000000, 2, 'n'}},
    {"snvxde",   {0x21000000, 2, 'n'}},
    {"snvxevy",  {0x22000000, 2, 'y'}},
    {"snvxdevy", {0x23000000, 2, 'y'}},
    {"snvxb",    {0x24000000, 2, 'n'}},
    {"snvxs",    {0x25000000, 2, 'n'}},
    {"snvxbvy",  {0x26000000, 2, 'y'}},
    {"snvxsvy",  {0x27000000, 2, 'y'}},
    {"svx",      {0x30000000, 2, 'n'}},
    {"svxvy",    {0x31000000, 2, 'y'}},
    {"svxovy",   {0x32000000, 2, 'y'}},
    {"svxavy",   {0x33000000, 2, 'y'}},
    {"svxxvy",   {0x34000000, 2, 'y'}},
    {"si",       {0x35000000, 1, 'j'}},
    {"avx",      {0x40000000, 2, 'n'}},
    {"avxvy",    {0x41000000, 2, 'y'}},
    {"sbxvy",    {0x42000000, 2, 'y'}},
    {"randvx",   {0x43000000, 2, 'n'}},
    {"avxi",     {0x44000000, 1, 'x'}},
    {"shvxr",    {0x50000000, 2, 'n'}},
    {"shvxl",    {0x51000000, 2, 'n'}},
    {"clear",    {0x60000000, 0, 'N'}},
    {"draw",     {0x61000000, 2, 'y'}},
    {"snkxp",    {0x70000000, 1, 'x'}},
    {"snkxnp",   {0x71000000, 1, 'x'}},
    {"sdel",     {0x80000000, 1, 'j'}},
    {"adel",     {0x81000000, 1, 'j'}},
    {"store",    {0x90000000, 1, 'x'}},
    {"load",     {0x91000000, 1, 'x'}},
    {"raw",      {0x00000000, 1, 'r'}}
};