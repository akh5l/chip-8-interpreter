#include <stdio.h>
#include "../include/chip8.h"


int main(int argc, char *argv[])
{

  chip8 c8;
  chip8_init(&c8);

  if (!chip8_load_rom(&c8, "../test-roms/test-opcode.ch8")) {
    fprintf(stderr, "Failed to load ROM!");
    return 1;
  }

  if (chip8_run(&c8) != 0) {
    fprintf(stderr, "Chip8 loop exited unexpectedly!");
    return 1;
  }
  
  return 0;
}