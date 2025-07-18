#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>

#define SCALE 10

int main(int argc, char *argv[])
{
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    printf("failed to initialize video\n");
    return 1;
  } 

  SDL_Window* window = SDL_CreateWindow("CHIP-8", 640, 320, SDL_WINDOW_INPUT_FOCUS);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
  SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 64, 32);

  SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);


  SDL_Event event;

  uint32_t framebuffer[64 * 32];

  bool running = true;
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        printf("\nexiting...\n");
        running = false;
      }
    }
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 150, 150, 255);
    SDL_FRect r = {27, 16, 10, 6};

    SDL_RenderRect(renderer, &r);
    SDL_RenderFillRect(renderer, &r);
    // SDL_Rect dstRect = {0, 0, 64, 32};

    SDL_SetRenderTarget(renderer, NULL);
    SDL_FRect dstRect = {0, 0, 64 * SCALE, 32 * SCALE};
    SDL_RenderTexture(renderer, texture, NULL, &dstRect);
    SDL_RenderPresent(renderer);


    SDL_Delay(16);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}