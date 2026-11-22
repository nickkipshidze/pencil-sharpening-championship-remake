#include <format>
#include <vector>
#include <iostream>
#include <algorithm>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

#include "conf.hpp"
#include "sharpener.hpp"
#include "pencil.hpp"

SDL_FColor hexToColor(unsigned int hex) {
    return SDL_FColor{
        .r=((hex>>24)&0xFF)/255.0f,
        .g=((hex>>16)&0xFF)/255.0f,
        .b=((hex>>8)&0xFF)/255.0f,
        .a=(hex&0xFF)/255.0f,
    };
}

void renderBackground(SDL_Renderer *renderer) {
    int s = 64;
    for (int y{0}; y<conf::resolution.y; y+=s) {
        for (int x{0}; x<conf::resolution.x; x+=s) {
            SDL_FColor color;
            if (((y%(s*2) == 0)*s+x)%(s*2) == 0) color = hexToColor(0xFFE203FF);
            else color = hexToColor(0xFED600FF);
            SDL_FRect rect = {(float)x, (float)y, (float)s, (float)s};
            SDL_SetRenderDrawColor(renderer, color.r*255, color.g*255, color.b*255, color.a*255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_FPoint position, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, 0, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FRect dstrect = {position.x, position.y, (float)texture->w, (float)texture->h};
    SDL_RenderTexture(renderer, texture, nullptr, &dstrect);
    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}

void renderTimer(SDL_Renderer *renderer, SDL_Texture *spritesheet, TTF_Font *font, float timeElapsed) {
    SDL_FRect dstrectTime = {0, 50, sprsheet::time.w/1.2f, sprsheet::time.h/1.2f};
    SDL_RenderTexture(renderer, spritesheet, &sprsheet::time, &dstrectTime);

    int minutes = timeElapsed/60;
    int seconds = timeElapsed-minutes*60;
    int msecs = (timeElapsed-minutes*60-seconds)*1000;
    renderText(
        renderer,
        font,
        std::format("{:02}:{:02}:{:03}", minutes, seconds, msecs).c_str(),
        {110, 63},
        {26, 26, 26, 255}
    );
}

bool initialize(SDL_Window *&window, SDL_Renderer *&renderer) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << SDL_GetError() << std::endl;
        return false;
    }
    if (!TTF_Init()) {
        std::cerr << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    window = SDL_CreateWindow(
        "Pencil Sharpening Championship",
        conf::resolution.x,
        conf::resolution.y,
        conf::windowflags
    );
    if (!window) {
        std::cerr << SDL_GetError() << std::endl;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error opening SDL3 window", SDL_GetError(), nullptr);
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << SDL_GetError() << std::endl;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error creating SDL3 renderer", SDL_GetError(), window);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    SDL_SetRenderVSync(renderer, conf::vsync);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderLogicalPresentation(renderer, conf::resolution.x, conf::resolution.y, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    return true;
}

void quit(SDL_Window *&window, SDL_Renderer *&renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

// Started dev 11:10 PM
// Paused dev 3:36 PM
// Continued dev 5:00 PM
// Paused dev 6:54 PM
// Continued dev 11:16 PM
// Paused dev 12:03 AM
int main(int, char *[]) {
    SDL_Window *window;
    SDL_Renderer *renderer;
    initialize(window, renderer);

    TTF_Font *fontSoraBold = TTF_OpenFont("./assets/Sora-Bold.ttf", 48);
    if (!fontSoraBold) {
        std::cerr << SDL_GetError() << std::endl;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error opening fonts", SDL_GetError(), window);
    }

    SDL_Texture *spritesheet = IMG_LoadTexture(renderer, "./assets/spritesheet.png");
    if (!spritesheet) {
        std::cerr << SDL_GetError() << std::endl;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error loading textures", SDL_GetError(), window);
    }

    Sharpener sharpener(spritesheet, {95, 364});
    Magazine pencilmag(spritesheet, {(sharpener.position.x+sprsheet::sharpener.w*0.8f-12), 0});

    uint64_t clockNow = SDL_GetPerformanceCounter(), clockLast = clockNow;
    float frequency = SDL_GetPerformanceFrequency();
    float timeElapsed = 0;

    bool running = true;
    while (running) {
        clockNow = SDL_GetPerformanceCounter();
        float deltaTime = std::clamp((float)(clockNow-clockLast)/frequency, 0.0f, 1.0f);
        clockLast = clockNow;
        timeElapsed += deltaTime;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT: {
                    running = false;
                    break;
                }
                case SDL_EVENT_KEY_DOWN: {
                    if (event.key.scancode == SDL_SCANCODE_Q)
                        running = false;
                    
                    // Demo behavior
                    if (event.key.scancode == SDL_SCANCODE_A || event.key.scancode == SDL_SCANCODE_LEFT) {
                        sharpener.activeArrow = 1;
                        if (pencilmag.lasttar() == pencilmag.lastpos() && pencilmag.lastpos().x <= -50) sharpener.sharpen(32.0f);
                        pencilmag.progress(64.0f);
                    } else if (event.key.scancode == SDL_SCANCODE_W || event.key.scancode == SDL_SCANCODE_UP) {
                        sharpener.activeArrow = 2;
                        if (pencilmag.lasttar() == pencilmag.lastpos() && pencilmag.lastpos().x <= -50) sharpener.sharpen(32.0f);
                        pencilmag.progress(64.0f);
                    } else if (event.key.scancode == SDL_SCANCODE_D || event.key.scancode == SDL_SCANCODE_RIGHT) {
                        sharpener.activeArrow = 3;
                        if (pencilmag.lasttar() == pencilmag.lastpos() && pencilmag.lastpos().x <= -50) sharpener.sharpen(32.0f);
                        pencilmag.progress(64.0f);
                    } else {
                        sharpener.activeArrow = 0;
                    }

                    break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        sharpener.update(deltaTime);
        pencilmag.update(deltaTime);

        renderBackground(renderer);
        renderTimer(renderer, spritesheet, fontSoraBold, timeElapsed);

        sharpener.render(renderer);
        pencilmag.render(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(spritesheet);
    TTF_CloseFont(fontSoraBold);
    
    quit(window, renderer);
    return 0;
}
