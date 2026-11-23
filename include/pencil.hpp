#pragma once

#include <vector>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

class Magazine {
    private:
        SDL_Texture *texture;

        SDL_FRect srcPencil;
        SDL_FRect dstPencil;

        std::vector<glm::vec2> pencilPositions;
        std::vector<glm::vec2> pencilTargets;

        glm::vec2 zero{0, 0};

        float vsize = 100.0f;
        float targety = 415.0f;
        int count = 5;
        
    public:
        glm::vec2 position;
        bool empty = false;

        Magazine(SDL_Texture *texture, glm::vec2 position, float scale=0.8f);
        ~Magazine();

        void update(float deltaTime);
        void addtop();
        void shiftnext();
        void poplast();
        glm::vec2 &lasttar();
        glm::vec2 &lastpos();
        void progress(float stepsize);
        void render(SDL_Renderer *renderer);
};
