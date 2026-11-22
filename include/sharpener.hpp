#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

class Sharpener {
    private:
        SDL_Texture *texture;

        SDL_FRect srcSharpener;
        SDL_FRect dstSharpener;

        SDL_FRect srcScrap;
        SDL_FRect dstScrap;

        SDL_FRect srcArrowLeft;
        SDL_FRect dstArrowLeft;

        SDL_FRect srcArrowUp;
        SDL_FRect dstArrowUp;

        SDL_FRect srcArrowRight;
        SDL_FRect dstArrowRight;

        float scrapAngle = 180.0f;
        float scrapAngleTarget = 180.0f;
        float animationTimer = 0.0f;
        
    public:
        glm::vec2 position;
        int activeArrow = 0;

        Sharpener(SDL_Texture *texture, glm::vec2 position, float scale=0.8f);
        ~Sharpener();

        void update(float deltaTime);
        void sharpen(float stepsize);
        void render(SDL_Renderer *renderer);
};
