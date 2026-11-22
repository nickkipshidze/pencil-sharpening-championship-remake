#include "conf.hpp"
#include "pencil.hpp"

Magazine::Magazine(SDL_Texture *texture, glm::vec2 position, float scale) : texture(texture), position(position) {
    this->srcPencil = sprsheet::pencilYellow;
    this->dstPencil = {0, 0, this->srcPencil.w*scale, this->srcPencil.h*scale};

    for (int i{0}; i<this->count; i++) {
        this->pencilPositions.push_back({130, (this->vsize*i)});
        this->pencilTargets.push_back({130, (this->vsize*i)});
    }
}

Magazine::~Magazine() {}

void Magazine::update(float deltaTime) {
    float speed = 8.0f;
    
    for (int i{0}; i < (int)this->pencilPositions.size(); ++i) {
        auto &current = this->pencilPositions[i];
        auto &target = this->pencilTargets[i];

        glm::vec2 diff = target - current;
        float dist = glm::length(diff);
        current += glm::vec2{
            glm::clamp(diff.x, -speed*dist*deltaTime, speed*dist*deltaTime),
            glm::clamp(diff.y, -speed*dist*deltaTime, speed*dist*deltaTime),
        };
        if (dist <= 1) current = target;
    }
}

void Magazine::addtop() {
    this->pencilTargets.insert(this->pencilTargets.begin(), {
        this->pencilTargets[0].x, this->pencilTargets[0].y-this->vsize
    });
    this->pencilPositions.insert(this->pencilPositions.begin(), {
        this->pencilTargets[0].x, -this->vsize
    });
}

void Magazine::shiftnext() {
    for (int i{0}; i < (int)this->pencilTargets.size()-1; ++i) {
        auto &current = this->pencilTargets[i];
        auto &next = this->pencilTargets[i+1];
        current = next;
    }
    this->pencilTargets[this->pencilTargets.size()-1] = {130, this->targety};
}

void Magazine::poplast() {
    this->pencilTargets.pop_back();
    this->pencilPositions.pop_back();
}

glm::vec2 &Magazine::lasttar() {
    return this->pencilTargets[this->pencilTargets.size()-1];
}

glm::vec2 &Magazine::lastpos() {
    return this->pencilPositions[this->pencilPositions.size()-1];
}

void Magazine::progress(float stepsize) {
    if (this->pencilTargets.size() <= 2)
        this->addtop();

    if (!(this->lastpos().y == targety)) {
        if (this->lastpos() == this->lasttar())
            shiftnext();
    } else {
        if (this->lastpos().x <= -this->dstPencil.w) {
            this->poplast();
            shiftnext();
        } else if (this->lasttar() == this->lastpos()) {
            this->lasttar().x -= stepsize;
        }
    }
}

void Magazine::render(SDL_Renderer *renderer) {
    SDL_Rect clipRect = {
        (int)position.x,
        (int)position.y,
        (int)conf::resolution.x,
        (int)conf::resolution.y
    };
    SDL_SetRenderClipRect(renderer, &clipRect);
    for (const auto &pencil : this->pencilPositions) {
        SDL_FRect dstrect = {
            this->position.x+pencil.x,
            this->position.y+pencil.y,
            this->dstPencil.w,
            this->dstPencil.h
        };
        SDL_RenderTexture(
            renderer, this->texture,
            &this->srcPencil, &dstrect
        );
    }
    SDL_SetRenderClipRect(renderer, NULL);
}
