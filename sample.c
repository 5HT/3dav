
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

void drawText ( SDL_Surface* screen, char* string, int size, int x, int y, SDL_Color fgC, SDL_Color bgC) {
  // Remember to call TTF_Init(), TTF_Quit(), before/after using this function.
  TTF_Font* font = TTF_OpenFont("CascadiaCode.ttf", size);
  if(!font) {
    printf("[ERROR] TTF_OpenFont() Failed with: %s\n", TTF_GetError());
    exit(2);
  }
  TTF_SetFontStyle(font, TTF_STYLE_BOLD);
  SDL_Surface* textSurface = TTF_RenderText_Shaded(font, string, fgC, bgC);   // anti-aliased glyphs
  SDL_Rect textLocation = { x, y, 0, 0 };
  SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
  SDL_FreeSurface(textSurface);
  TTF_CloseFont(font);
}


int main(int argc, char* args[]) {
  SDL_Window* window = NULL;
  SDL_Surface* screenSurface = NULL;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
    return 1;
  }
  window = SDL_CreateWindow(
                "hello_sdl2",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                SCREEN_WIDTH, SCREEN_HEIGHT,
                //SDL_WINDOW_SHOWN | 
		SDL_WINDOW_OPENGL
                );
  if (window == NULL) {
    fprintf(stderr, "could not create window: %s\n", SDL_GetError());
    return 1;
  }
  screenSurface = SDL_GetWindowSurface(window);
  SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
  SDL_UpdateWindowSurface(window);
  
  //-----------------------------------------------------
  // Draw some Text
  //-----------------------------------------------------
  if(TTF_Init() == -1) {
    printf("[ERROR] TTF_Init() Failed with: %s\n", TTF_GetError());
    exit(2);
  }
  SDL_Color fg = { 0x00,0x00,0xff }, bg = {0xff,0xff,0xff};      // Blue text on white background
  drawText( screenSurface, (char*) "Hello World! @ (x=50, y=100)", 18, 50, 100, fg, bg);
  SDL_UpdateWindowSurface(window);
  TTF_Quit();

  SDL_Delay(2000);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}

#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR argv, int argc) {
    return SDL_main(argc, NULL);
}
#endif
