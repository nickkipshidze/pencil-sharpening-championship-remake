#include <format>
#include <stdlib.h>
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

void renderBackground(SDL_Renderer *renderer, float alpha=1.0f) {
    int s = 64;
    for (int y{0}; y<conf::resolution.y; y+=s) {
        for (int x{0}; x<conf::resolution.x; x+=s) {
            SDL_FColor color;
            if (((y%(s*2) == 0)*s+x)%(s*2) == 0) color = hexToColor(0xFFE203FF);
            else color = hexToColor(0xFED600FF);
            color.a = alpha;
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

class MainMenu {
    private:
        bool closing = false;

        SDL_FRect positionSharpener = {
            43.0f, 45.0f,
            sprsheet::menuSharpener.w, sprsheet::menuSharpener.h
        };
        SDL_FRect positionShaving = {
            195.0f, 545.0f,
            sprsheet::menuShaving.w, sprsheet::menuShaving.h
        };
        SDL_FRect positionPencils = {
            796.0f, 29.0f,
            sprsheet::menuPencils.w, sprsheet::menuPencils.h
        };
        SDL_FRect positionTitle = {
            125.5f, 200.5f,
            sprsheet::menuTitle.w, sprsheet::menuTitle.h
        };
        SDL_FRect positionPressStart = {
            412.0f, 511.0f,
            0, 0
        };

        SDL_Texture *texture;
        TTF_Font *font;
        float animationTimer = 0.0f;
        float closeTimer = 1.0f;
    
    public:
        bool visible = true;

        MainMenu(SDL_Texture *texture, TTF_Font *font) : texture(texture), font(font) {}

        ~MainMenu() {}

        void close() {
            this->closing = true;
        }

        void update(float deltaTime) {
            this->animationTimer += deltaTime;
            if (this->closing)
                this->closeTimer -= deltaTime;
            if (this->closeTimer <= 0)
                this->visible = false;
        }

        void render(SDL_Renderer *renderer) {
            Uint8 alpha = (Uint8)(SDL_clamp(255.0f*this->closeTimer, 0, 255));
            float yOffset = SDL_sinf(this->animationTimer)*10.0f;

            if (this->visible) {
                renderBackground(renderer, this->closeTimer);
                SDL_SetTextureAlphaMod(this->texture, alpha);
                SDL_RenderTexture(renderer, this->texture,&sprsheet::menuTitle, &this->positionTitle);
                SDL_RenderTexture(renderer, this->texture,&sprsheet::menuSharpener, &this->positionSharpener);
                SDL_RenderTexture(renderer, this->texture,&sprsheet::menuPencils, &this->positionPencils);
                SDL_RenderTexture(renderer, this->texture,&sprsheet::menuShaving, &this->positionShaving);
                SDL_SetTextureAlphaMod(this->texture, 255);
                if (alpha > 5)
                    renderText(
                        renderer, this->font, "Press Start",
                        {this->positionPressStart.x, this->positionPressStart.y+yOffset},
                        {0, 0, 0, alpha}
                    );
            }
        }
};

bool initialize(SDL_Window *&window, SDL_Renderer *&renderer) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
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
// Paused dev 1:01 AM
// Continued dev 7:53 PM
// Paused dev 9:15 PM
int main(int, char *[]) {
    SDL_Window *window;
    SDL_Renderer *renderer;
    initialize(window, renderer);

    TTF_Font *fontSoraBold = TTF_OpenFont("./assets/Sora-Bold.ttf", 48);
    if (!fontSoraBold) {
        std::cerr << SDL_GetError() << std::endl;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error opening fonts", SDL_GetError(), window);
    }

    SDL_Texture *imgMenu = IMG_LoadTexture(renderer, "./assets/menu.png");
    SDL_Texture *imgSprites = IMG_LoadTexture(renderer, "./assets/sprites.png");

    if (!imgMenu || !imgSprites) {
        std::cerr << SDL_GetError() << std::endl;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error loading textures", SDL_GetError(), window);
    }

    SDL_AudioSpec audioWavSpec;
    Uint8 *audioWavBuffer = NULL;
    Uint32 audioWavLength = 0;
    if (!SDL_LoadWAV("./assets/OneStepAtATIme - Alex McCulloch.wav", &audioWavSpec, &audioWavBuffer, &audioWavLength)) {
        std::cerr << SDL_GetError() << std::endl;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error loading audio", SDL_GetError(), window);
    }

    SDL_AudioStream *audioStream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
        &audioWavSpec,
        NULL, NULL
    );
    if (!audioStream) {
        std::cerr << SDL_GetError() << std::endl;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error opening audio device stream", SDL_GetError(), window);
    }

    MainMenu menuScreen(imgMenu, fontSoraBold);
    Sharpener sharpener(imgSprites, {95, 364});
    Magazine pencilmag(imgSprites, {(sharpener.position.x+sprsheet::sharpener.w*0.8f-12), 0});

    int keyCurrent = rand() % 3 + 1;
    int keyNext = rand() % 3 + 1;

    uint64_t clockNow = SDL_GetPerformanceCounter(), clockLast = clockNow;
    float frequency = SDL_GetPerformanceFrequency();
    float timeElapsed = 0;

    bool running = true;
    while (running) {
        clockNow = SDL_GetPerformanceCounter();
        float deltaTime = std::clamp((float)(clockNow-clockLast)/frequency, 0.0f, 1.0f);
        clockLast = clockNow;
        
        bool settled;
        if (!pencilmag.empty) settled = (pencilmag.lasttar() == pencilmag.lastpos());
        else settled = false;
        if (!settled) sharpener.activeArrow = 0;
        else sharpener.activeArrow = keyCurrent;

        if (SDL_GetAudioStreamQueued(audioStream) == 0) {
            SDL_PutAudioStreamData(audioStream, audioWavBuffer, audioWavLength);
            SDL_ResumeAudioStreamDevice(audioStream);
        }

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
                    
                    if (event.key.scancode == SDL_SCANCODE_SPACE)
                        menuScreen.close();
                    
                    if (menuScreen.visible == true) break;

                    int keyPressed = 0;
                    if (event.key.scancode == SDL_SCANCODE_A) keyPressed = 1;
                    if (event.key.scancode == SDL_SCANCODE_LEFT) keyPressed = 1;
                    if (event.key.scancode == SDL_SCANCODE_W) keyPressed = 2;
                    if (event.key.scancode == SDL_SCANCODE_UP) keyPressed = 2;
                    if (event.key.scancode == SDL_SCANCODE_D) keyPressed = 3;
                    if (event.key.scancode == SDL_SCANCODE_RIGHT) keyPressed = 3;

                    if (keyPressed == keyCurrent && settled) {
                        if (pencilmag.lastpos().x <= -50) sharpener.sharpen(32.0f);
                        pencilmag.progress(64.0f);
                        keyCurrent = keyNext;
                        keyNext = rand() % 3 + 1;
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
        if (menuScreen.visible == false && !pencilmag.empty) timeElapsed += deltaTime;
        renderTimer(renderer, imgSprites, fontSoraBold, timeElapsed);

        sharpener.render(renderer);
        pencilmag.render(renderer);

        menuScreen.update(deltaTime);
        menuScreen.render(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyAudioStream(audioStream);
    SDL_free(audioWavBuffer);
    SDL_DestroyTexture(imgSprites);
    SDL_DestroyTexture(imgMenu);
    TTF_CloseFont(fontSoraBold);
    quit(window, renderer);

    return 0;
}
