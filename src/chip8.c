#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "../include/chip8.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SCALE 8
#define ROM_START 0x200
#define SUPER_CHIP false

void chip8_init(chip8* c8) { 
  
  memset(c8->memory, 0, sizeof(c8->memory)); // zero all memory
  memset(c8->display, 0, sizeof(c8->display)); // zero display
  memset(c8->V, 0, sizeof(c8->V));
  memset(c8->keys, 0, sizeof(c8->keys));

  c8->I = 0;
  c8->delay_timer = 0;
  c8->sound_timer = 0;

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

uint16_t chip8_fetch(chip8* c8) {
  // combine two nibbles into one opcode
  uint16_t opcode = c8->memory[c8->PC] << 8 | c8->memory[c8->PC + 1];

  // increment by 2 as next instruction is 2 bytes ahead
  c8->PC += 2;

  return opcode;
}

void chip8_execute(chip8* c8, uint16_t opcode) {

  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t y = (opcode & 0x00F0) >> 4;
  
  uint8_t n = opcode & 0x000F;
  uint8_t nn = opcode & 0x00FF;
  uint16_t nnn = opcode & 0x0FFF;

  // isolates first hex character to categorise opcode
  switch (opcode & 0xF000) {
    case 0x0000:
      if (opcode == 0x00E0) { // clear display
        memset(c8->display, 0, sizeof(c8->display));
      }
      else if (opcode == 0x00EE) { // return to last PC value stored in stack
        c8->PC = (uint16_t)(uintptr_t)stack_pop(&c8->stack);
      }
      break;
    
    case 0x1000:
      c8->PC = nnn; // PC jumps to 0x0NNN
      break;
    
    case 0x2000:
      stack_push(&c8->stack, (void*)(uintptr_t)c8->PC); // store current PC value and jump to 0x0NNN
      c8->PC = nnn;
      break;

    case 0x3000:
      if (c8->V[x] == nn) { // the following 4 are conditional skips
        c8->PC += 2;
      }
      break;
    
    case 0x4000:
      if (c8->V[x] != nn) {
        c8->PC += 2;
      }
      break;

    case 0x5000:
      if (c8->V[x] == c8->V[y]) {
        c8->PC += 2;
      }
      break;
    
    case 0x9000:
      if (c8->V[x] != c8->V[y]) {
        c8->PC += 2;
      }
      break;

    case 0x6000:
      c8->V[x] = nn; // set VX to NN
      break;
    
    case 0x7000:
      c8->V[x] += nn; // add NN to VX
      break;
    
    case 0x8000: // logical & arithmetic instructions
      switch (opcode & 0x000F) {
        case 0:
          c8->V[x] = c8->V[y]; // VX set equal to VY
          break;
        case 1:
          c8->V[x] |= c8->V[y]; // VX set to VX bitwise OR VY
          break;
        case 2:
          c8->V[x] &= c8->V[y]; // VX set to VX bitwise AND VY
          break;
        case 3:
          c8->V[x] ^= c8->V[y]; // VX set to VX bitwise XOR VY
          break;
        case 4: // add
          c8->V[0xF] = (c8->V[x] + c8->V[y]) > 255; // trigger flag if overflow
          c8->V[x] += c8->V[y];
          break;
        case 5: // subtract
          c8->V[0xF] = c8->V[x] >= c8->V[y] ? 1 : 0; // no borrow = 1
          c8->V[x] -= c8->V[y]; // wraps naturally unsigned
          break;
        case 7:
          c8->V[0xF] = c8->V[y] >= c8->V[x] ? 1 : 0; // no borrow = 1
          c8->V[x] = c8->V[y] - c8->V[x]; // wraps naturally unsigned
          break;
        case 6: // shift depending on super-chip config or not
          if (SUPER_CHIP) {
            c8->V[x] = c8->V[y];
          }
          c8->V[0xF] = c8->V[x] & 0x1;
          c8->V[x] >>= 1;
          break;
        case 0xE:
          if (SUPER_CHIP) {
            c8->V[x] = c8->V[y];
          }
          c8->V[0xF] = c8->V[(opcode & 0x0F00) & 0x80] >> 7;
          c8->V[x] <<= 1;
          break;
      }
      break;

    case 0xA000:
      c8->I = nnn; // I (index) set to NNN
      break;

    case 0xB000:
      c8->PC = nnn;
      c8->PC += SUPER_CHIP ? c8->V[x] : c8->V[0];
      break;

    case 0xC000:
      c8->V[x] = (rand() % 256) & nn;
      break;
    
    case 0xD000: // writing to display
      uint8_t dx = c8->V[x]; // get x from VX and wrap around screen with modulo
      uint8_t dy = c8->V[y]; // same as above but VY and y

      c8->V[0xF] = 0;

      for (int row = 0; row < n; row++) {
        uint8_t sprite_byte = c8->memory[c8->I + row];
        uint8_t pixel_y = (dy + row) % HEIGHT;
        for (int col = 0; col < 8; col++) { // col < 8 as it's just going thru the byte
          uint8_t pixel_x = (dx + col) % WIDTH;
          uint8_t sprite_pixel = (sprite_byte >> (7 - col)) & 1;

          uint16_t idx = (pixel_y * WIDTH + pixel_x);

          if (sprite_pixel == 1) {
            if (c8->display[idx] == 1) {
              c8->V[0xF] = 1;
            }
            c8->display[idx] ^= 1;
          }
        }
      }
      break;

    case 0xE000: // skip one instruction if key corresponding to VX is pressed
      if (nn == 0x009E) {

      } else { // above but skip if not pressed

      }
      break;
    case 0xF000:
      
      break;

    default:
      fprintf(stderr, "Opcode not recognised: %#04x\n", opcode);
      break;
  }
}

// fetch, decode, execute single opcode
bool chip8_cycle(chip8* c8) { 
  if (c8->PC >= 4096) {
    fprintf(stderr, "Program counter out of bounds!\n");
    return false;
  }

  chip8_execute(c8, chip8_fetch(c8));

  return true;
}

int chip8_run(chip8* c8) {
  
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "failed to initialize video\n");
    return 1;
  }

  SDL_Event event;

  SDL_Window* window = SDL_CreateWindow("CHIP-8", WIDTH * SCALE, HEIGHT * SCALE, SDL_WINDOW_INPUT_FOCUS);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

  bool running = true;
  while (running) { // extract function?
    
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        printf("\nexiting...\n");
        running = false;
      }
    }

    if (!chip8_cycle(c8)) {
      running = false;
    }

    void* pixels;
    int pitch = WIDTH * 4; // WIDTH to be replaced by texture width?

    if (!SDL_LockTexture(texture, NULL, &pixels, &pitch)) {
      fprintf(stderr, "failed to lock texture: %s\n", SDL_GetError());
      return 1;
    }

    uint32_t* framebuffer = (uint32_t*)pixels;

    for (int y = 0; y < HEIGHT; y++) {
      for (int x = 0; x < WIDTH; x++) {
        uint8_t pixel = c8->display[y * WIDTH + x];
        framebuffer[y * (pitch / 4) + x] = pixel ? 0xFFFFFFFF : 0xFF000000;
      }
    }


    SDL_UnlockTexture(texture);

    SDL_RenderClear(renderer);

    SDL_FRect dstRect = {0, 0, WIDTH * SCALE, HEIGHT * SCALE};
    SDL_RenderTexture(renderer, texture, NULL, &dstRect);
    SDL_RenderPresent(renderer);


    SDL_Delay(16); // 16ms = 62.5FPS
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}

 // update delay & sound timers
void chip8_update_timers(chip8* c8) {

}