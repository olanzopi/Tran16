#include <iostream>
#include <cstring>

#include "platform.h"
#include "disassembler.h"
#include "assembler.h"

int main(int argc, char *argv[]){
    if(argc == 1) {
        log_fatal("use argument ya bum. -c for assemble. -d for disassemble");
        return 1;
    }

    if(strcmp(argv[1], "-c") == 0) {
        if(argc == 2) { log_fatal("no path provided"); return 1; }
        std::string path = argv[2];

        log_debug(path);
        // assemble path
        assemble_script(path);
    }
    else if(strcmp(argv[1], "-d") == 0) {
        if(argc == 2) { log_fatal("no path provided"); return 1; }
        std::string path = argv[2];

        // disassemble path
        if(disassemble_binary(path)) {
            log_debug("succesfully disassembled: " + path);
        } else { log_fatal("something went wrong while disassembling"); return 1;}
    }
    return 0;
}