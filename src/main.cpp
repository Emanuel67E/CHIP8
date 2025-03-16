#include <SDL2/SDL.h>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <vector>

#include "chip8.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

#define FPS 60

int main(int argc, char** argv) {
  puts("Starting...");
  printf("\n");

  Chip8 mychip8;
  mychip8.initialize();
  int gameloaded = mychip8.loadGame("Roms/5-quirks.ch8");
  if(gameloaded == 0) {
    puts("Failed to load game, ahhhhhhhh...");
    return 0;
  }

  //std::vector<uint8_t> memory = mychip8.get_memory();
  //printf("Memory: %X\n", memory[0x208]);

  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Error: SDL failed to initialize\nSDL Error: '%s'\n", SDL_GetError());
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  if(!window) {
    printf("Error: Failed to open window\nSDL Error: '%s'\n", SDL_GetError());
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if(!renderer) {
    printf("Error: Failed to create renderer\nSDL Error: '%s'\n", SDL_GetError());
    return 1;
  }

  uint32_t elapsed = 0;
  int frametime = 0;
  int cycle = 0;
  bool running = true;
  while(running) {

    elapsed = SDL_GetTicks();

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT:
          running = false;
          break;

        case SDL_KEYDOWN:
          switch(event.key.keysym.sym) {
            case SDLK_1:
              mychip8.key_down(0x1);
              break;

            case SDLK_2:
              mychip8.key_down(0x2);
              break;

            case SDLK_3:
              mychip8.key_down(0x3);
              break;

            case SDLK_4:
              mychip8.key_down(0xC);
              break;

            case SDLK_q:
              mychip8.key_down(0x4);
              break;

            case SDLK_w:
              mychip8.key_down(0x5);
              break;

            case SDLK_e:
              mychip8.key_down(0x6);
              break;

            case SDLK_r:
              mychip8.key_down(0xD);
              break;

            case SDLK_a:
              mychip8.key_down(0x7);
              break;

            case SDLK_s:
              mychip8.key_down(0x8);
              break;

            case SDLK_d:
              mychip8.key_down(0x9);
              break;

            case SDLK_f:
              mychip8.key_down(0xE);
              break;

            case SDLK_z:
              mychip8.key_down(0xA);
              break;

            case SDLK_x:
              mychip8.key_down(0x0);
              break;

            case SDLK_c:
              mychip8.key_down(0xB);
              break;

            case SDLK_v:
              mychip8.key_down(0xF);
              break;
          }
          break;

        case SDL_KEYUP:
          switch(event.key.keysym.sym) {
            case SDLK_1:
              mychip8.key_up(0x1);
              break;

            case SDLK_2:
              mychip8.key_up(0x2);
              break;

            case SDLK_3:
              mychip8.key_up(0x3);
              break;

            case SDLK_4:
              mychip8.key_up(0xC);
              break;

            case SDLK_q:
              mychip8.key_up(0x4);
              break;

            case SDLK_w:
              mychip8.key_up(0x5);
              break;

            case SDLK_e:
              mychip8.key_up(0x6);
              break;

            case SDLK_r:
              mychip8.key_up(0xD);
              break;

            case SDLK_a:
              mychip8.key_up(0x7);
              break;

            case SDLK_s:
              mychip8.key_up(0x8);
              break;

            case SDLK_d:
              mychip8.key_up(0x9);
              break;

            case SDLK_f:
              mychip8.key_up(0xE);
              break;

            case SDLK_z:
              mychip8.key_up(0xA);
              break;

            case SDLK_x:
              mychip8.key_up(0x0);
              break;

            case SDLK_c:
              mychip8.key_up(0xB);
              break;

            case SDLK_v:
              mychip8.key_up(0xF);
              break;
          }
          break;

        default:
          break;
      }
    }

    if(cycle < 1200) { //400

    printf("Current cycle: %d\n", cycle);
    mychip8.emulateCycle();

    //printf("Draw flag: %d\n", mychip8.get_draw_flag());
    if(mychip8.get_draw_flag() == 1) {
      SDL_SetRenderDrawColor(renderer, 0, 127, 0, 255);
      SDL_Rect rect = {0, 0, 0, 0};
      std::vector<std::vector<uint8_t>> FB = mychip8.get_fb();
      for(int i=0; i<32; ++i) {
        for(int j=0; j<64; ++j) {
          if(FB[i][j] == 1) {
            rect = {j*10, i*10, 10, 10};
            SDL_RenderFillRect(renderer, &rect);
          }
        }
      }
      puts("Rendering");
      mychip8.reset_draw_flag();
    }

    SDL_RenderPresent(renderer); // Update screen
    cycle++;
    //printf("PC at cycle end: %X\n", mychip8.get_pc());
    printf("\n");

    } // if end

    frametime = SDL_GetTicks() - elapsed;
    if( (1000 / FPS) > frametime) {
      SDL_Delay( (1000/FPS) - frametime);
    }

  }

  printf("Cycles: %d\n", cycle);

  puts("Ending...");
  return 0;
}
