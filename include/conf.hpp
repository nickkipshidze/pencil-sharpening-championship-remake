#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

namespace conf {
    inline glm::vec2 resolution = {1080, 675};
    inline bool vsync = true;
    inline SDL_WindowFlags windowflags = 0;
};

namespace sprsheet {
    inline SDL_FRect time = {0, 1, 533, 113};
    inline SDL_FRect pencilGreen = {541, 0, 687, 96};
    inline SDL_FRect pencilYellow = {541, 98, 687, 96};
    inline SDL_FRect arrowLeft = {1, 116, 110, 141};
    inline SDL_FRect arrowUp = {113, 116, 110, 186};
    inline SDL_FRect arrowRight = {224, 116, 110, 141};
    inline SDL_FRect sharpener = {0, 305, 362, 220};
    inline SDL_FRect scrap = {412, 217, 585, 299};
    inline SDL_FRect goodgisLogo = {1156, 453, 72, 72};
};
