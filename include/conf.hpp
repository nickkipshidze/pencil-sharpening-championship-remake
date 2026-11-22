#include <SDL3/SDL.h>
#include <glm/glm.hpp>

namespace conf {
    glm::vec2 resolution = {1080, 675};
    bool vsync = true;
};

namespace sprsheet {
    SDL_FRect time = {0, 1, 533, 113};
    SDL_FRect pencilGreen = {541, 0, 687, 96};
    SDL_FRect pencilYellow = {541, 98, 687, 96};
    SDL_FRect arrowLeft = {1, 116, 110, 141};
    SDL_FRect arrowUp = {113, 116, 110, 186};
    SDL_FRect arrowRight = {224, 116, 110, 141};
    SDL_FRect sharpener = {0, 305, 362, 220};
    SDL_FRect scrap = {412, 217, 585, 299};
    SDL_FRect goodgisLogo = {1156, 453, 72, 72};
};
