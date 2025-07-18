#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>

#include "../include/chip8.h"


#define SCALE 12

int main(int argc, char *argv[])
{
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "failed to initialize video\n");
    return 1;
  } 

  SDL_Event event;

  SDL_Window* window = SDL_CreateWindow("CHIP-8", WIDTH * SCALE, HEIGHT * SCALE, SDL_WINDOW_INPUT_FOCUS);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

  chip8 c8;
  chip8_init(&c8);

  if (!chip8_load_rom(&c8, "../test-roms/test-opcode.ch8")) {
    fprintf(stderr, "Failed to load ROM!");
    return 1;
  }

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

    if (!chip8_cycle(&c8)) {
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
        uint8_t pixel = c8.display[y * WIDTH + x];
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