#include "conf.hpp"
#include "sharpener.hpp"

Sharpener::Sharpener(SDL_Texture *texture, glm::vec2 position, float scale) : texture(texture), position(position) {
    this->srcSharpener = sprsheet::sharpener;
    this->dstSharpener = {position.x, position.y, this->srcSharpener.w*scale, this->srcSharpener.h*scale};

    this->srcScrap = sprsheet::scrap;
    this->dstScrap = {position.x-240, position.y+40, this->srcScrap.w*scale, this->srcScrap.h*scale};

    this->srcArrowLeft = sprsheet::arrowLeft;
    this->dstArrowLeft = {position.x, position.y-130, this->srcArrowLeft.w*scale, this->srcArrowLeft.h*scale};

    this->srcArrowUp = sprsheet::arrowUp;
    this->dstArrowUp = {position.x+100, position.y-150, this->srcArrowUp.w*scale, this->srcArrowUp.h*scale};

    this->srcArrowRight = sprsheet::arrowRight;
    this->dstArrowRight = {position.x+200, position.y-130, this->srcArrowRight.w*scale, this->srcArrowRight.h*scale};
}

Sharpener::~Sharpener() {}

void Sharpener::update(float deltaTime) {
    float speed = 8.0f;
    
    this->animationTimer += deltaTime;

    this->scrapAngle = SDL_fmodf(this->scrapAngle, 360.0f);
    this->scrapAngleTarget = SDL_fmodf(this->scrapAngleTarget, 360.0f);

    float diff = this->scrapAngleTarget-this->scrapAngle;
    this->scrapAngle += SDL_clamp(
        diff,
        -speed*diff*deltaTime,
        speed*diff*deltaTime
    );

    if (this->scrapAngle > 360.0f)
        this->scrapAngleTarget = 180.0f;
}

void Sharpener::sharpen(float stepsize) {
    this->scrapAngleTarget += stepsize;
}

void Sharpener::render(SDL_Renderer *renderer) {
    SDL_RenderTexture(renderer, this->texture, &this->srcSharpener, &this->dstSharpener);
    
    SDL_Rect clipRectScrap = {0, (int)position.y+107, 512, 512};
    SDL_SetRenderClipRect(renderer, &clipRectScrap);

    SDL_FPoint center = {this->dstScrap.w/2, 7};
    SDL_RenderTextureRotated(
        renderer, this->texture,
        &this->srcScrap, &this->dstScrap,
        this->scrapAngle, &center, SDL_FLIP_NONE
    );

    SDL_SetRenderClipRect(renderer, NULL);

    float yOffset = SDL_sinf(this->animationTimer)*10.0f;

    SDL_FRect dstArrowLeft = this->dstArrowLeft;
    SDL_FRect dstArrowUp = this->dstArrowUp;
    SDL_FRect dstArrowRight = this->dstArrowRight;
    switch (this->activeArrow) {
        case 1: dstArrowLeft.y += yOffset; break;
        case 2: dstArrowUp.y += yOffset; break;
        case 3: dstArrowRight.y += yOffset; break;
    }

    if (this->activeArrow == 1) SDL_SetTextureColorMod(texture, 255, 255, 255);
    else SDL_SetTextureColorMod(texture, 100, 100, 100);
    SDL_RenderTexture(renderer, this->texture, &this->srcArrowLeft, &dstArrowLeft);
    if (this->activeArrow == 2) SDL_SetTextureColorMod(texture, 255, 255, 255);
    else SDL_SetTextureColorMod(texture, 100, 100, 100);
    SDL_RenderTexture(renderer, this->texture, &this->srcArrowUp, &dstArrowUp);
    if (this->activeArrow == 3) SDL_SetTextureColorMod(texture, 255, 255, 255);
    else SDL_SetTextureColorMod(texture, 100, 100, 100);
    SDL_RenderTexture(renderer, this->texture, &this->srcArrowRight, &dstArrowRight);
    SDL_SetTextureColorMod(texture, 255, 255, 255);
}
