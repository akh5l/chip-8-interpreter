#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>
#include "stack.h"

extern const uint8_t fontset[80]; // stores fontset to be copied into memory

typedef struct {
  uint8_t memory[4096]; // 4KB RAM
  uint8_t display[64 * 32]; // holds pixels to display before translation to SDL
  uint16_t PC; // program counter
  uint16_t I; // indexes program counter

  Stack stack; // for 16-bit addresses

  uint8_t delay_timer; // decremented at 60Hz until it reaches 0
  uint8_t sound_timer; // beep plays while > 0

  uint8_t V[16]; // general purpose variable registers; register VF is used as a flag
  uint8_t keys[16]; // stores the keypad

  uint16_t opcode; // stores current opcode

} chip8;

void chip8_init(chip8* c8); // clear all memory, load fonts, and set PC to 0x200

bool chip8_load_rom(chip8* c8, const char* filepath); // load a ROM into memory starting at 0x200

void chip8_cycle(chip8* c8); // fetch, decode, execute single opcode

void chip8_update_timers(chip8* c8); // update delay & sound timers

#endif