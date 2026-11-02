# tran16

(this is a showcase repo. do not create issues)

TRANS 16 c++ emulator, disassembler/assembler and maybe debugger. 

currently only works for linux.

TRANS 16 is a modified version of my CHIP 8 emulater, but with a much larger rgb display. more ram and other changes to extend developing possibilities.

check the build folder and config file.

tran16 assembly table:
| opcode | instruction | description |
| --- | --- | --- |
| **routines** | | |
| 0100NNNN | sub NNNN | subroutine at NNNN |
| 02000000 | return | return subroutine |
| **jumps** | | |
| 1000NNNN | jmp NNNN | jump to NNNN |
| 11VXNNNN | jmpvx NNNN | jump to NNNN + VX |
| **skip next if** | | |
| 20VXNNNN | snvxe VX NNNN | VX equal NNNN |
| 21VXNNNN | snvxde VX NNNN | VX not equal NNNN |
| 22VXVY00 | snvxevy VX VY | VX equal VY |
| 23VXVY00 | snvxdevy VX VY | VX not equal VY |
| **set to** | | |
| 30VXNNNN | svx VX NNNN | VX to NNNN |
| 31VXVY00 | svxvy VX VY | VX to VY |
| 32VXVY00 | svxovy VX VY | VX or VY |
| 33VXVY00 | svxavy VX VY | VX and VY |
| 34VXVY00 | svxxvy VX VY | VX xor VY |
| 3500NNNN | si NNNN | I to NNNN |
| **math** | | |
| 40VXNNNN | avx VX NNNN | add NNNN to VX |
| 41VXVY00 | avxvy VX VY | add VY to VX |
| 42VXVY00 | svxvy VX VY | subtract VY from VX |
| 43VXNNNN | randvx VX NNNN | set VX to random between 0-NNNN |
| 44VX0000 | avxi VX | add VX to I |
| **shifts** | | |
| 50VXNNNN | shvxr VX NNNN | shift vx NNNN to the right |
| 51VXNNNN | shvxl VX NNNN | shift vx NNNN to the left |
| **drawing** | | |
| 60000000 | clear | clear display |
| 61VXVY00 | draw VX VY | draw pixel at VX, VY. colors are registers r10 g11 b12. |
| **keys** | | |
| 70KX0000 | snkxp KX | skip next if KX pressed |
| 71KX0000 | snkxnp KX | skip next if KX not pressed |
| **timers** | | |
| 8000NNNN | sdel NNNN | set delay timer to NNNN |
| 8100NNNN | adel NNNN | add NNNN to delay timer |
| **memory** | | |
| 90VX0000 | store VX | store v0 to vx starting at i (including vx and dont write I) |
| 91VX0000 | load VX | load v0 to vx starting at i (including vx and dont write I) |
| **raw** | | |
| NNNNNNNN | raw NNNNNNNN | raw bytes |