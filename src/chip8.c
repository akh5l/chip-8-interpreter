#include "../include/chip8.h"
#include <stdio.h>
#include <string.h>


#define ROM_START 0x200

void chip8_init(chip8* c8) { 
  
  memset(c8->memory, 0, sizeof(c8->memory)); // zero all memory
  memset(c8->display, 0, sizeof(c8->display)); // zero display
  memset(c8->V, 0, sizeof(c8->V));
  memset(c8->keys, 0, sizeof(c8->keys));

  c8->I = 0;
  c8->delay_timer = 0;
  c8->sound_timer = 0;
  c8->opcode = 0;

  stack_initialize(&c8->stack);

  const uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  memcpy(&c8->memory[0x50], fontset, sizeof(fontset)); // load fontset into memory

  c8->PC = ROM_START; // set program counter to 512 (0x200)

}
// load a ROM into memory starting at 0x200
bool chip8_load_rom(chip8* c8, const char* filepath) {
  FILE* file = fopen(filepath, "rb");

  if (!file) {
    perror("Failed to open the ROM file");
    return false;
  }

  fseek(file, 0, SEEK_END);
  
  long rom_size = ftell(file);

  rewind(file);

  if (rom_size + ROM_START > sizeof(c8->memory)) {
    perror("ROM too large for memory");
    return false;
  }

  size_t read = fread(&c8->memory[ROM_START], 1, rom_size, file);
  if (read != rom_size) {
    perror("ROM was not fully read");
    return false;
  }

  printf("Loaded rom %s\n", filepath);
  fclose(file);
  return true;
}

// fetch, decode, execute single opcode
void chip8_cycle(chip8* c8) { 
  
}

 // update delay & sound timers
void chip8_update_timers(chip8* c8) {

}