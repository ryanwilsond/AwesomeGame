#ifndef OBJECT_H
#define OBJECT_H

#include "utils.h"
#include "game.h"

class WASDController {
private:

    bool up_;
    bool down_;
    bool left_;
    bool right_;

public:

    WASDController() {
        this->up_ = false;
        this->down_ = false;
        this->left_ = false;
        this->right_ = false;
    }

    void SetUpMovement(bool moving) {
        this->up_ = moving;
    }

    void SetDownMovement(bool moving) {
        this->down_ = moving;
    }

    void SetLeftMovement(bool moving) {
        this->left_ = moving;
    }

    void SetRightMovement(bool moving) {
        this->right_ = moving;
    }

    bool MovingUp() {
        return this->up_;
    }

    bool MovingDown() {
        return this->down_;
    }

    bool MovingLeft() {
        return this->left_;
    }

    bool MovingRight() {
        return this->right_;
    }

};

class GameObject {
private:

    SDL_Surface surface_;
    SDL_Surface surface_original_;
    SDL_Texture * texture_;
    SDL_Rect hitbox_;
    SDL_Renderer * renderer_;
    int speed_;
    int rotation_;
    SDL_Point rotate_axis_;
    int moment_bounds_[4];

public:

    int MomentX;
    int MomentY;

    ~GameObject() {
        SDL_DestroyTexture(this->texture_);
    }

    GameObject() {
    }

    GameObject(SDL_Renderer * renderer) {
        this->renderer_ = renderer;
        this->rotate_axis_ = {-1, -1};
        this->rotation_ = 0;
    }

    void Destroy() {
        this->~GameObject();
    }

    void SetRenderer(SDL_Renderer * renderer) {
        this->renderer_ = renderer;
    }

    void SetSpeed(int speed) {
        this->speed_ = speed;
    }

    void SetMomentBounds(int xLower, int xUpper, int yLower, int yUpper) {
        this->moment_bounds_[0] = xLower;
        this->moment_bounds_[1] = xUpper;
        this->moment_bounds_[2] = yLower;
        this->moment_bounds_[3] = yUpper;
    }

    int GetX() {
        return this->hitbox_.x;
    }

    int GetY() {
        return this->hitbox_.y;
    }

    int GetWidth() {
        return this->hitbox_.w;
    }

    int GetHeight() {
        return this->hitbox_.h;
    }

    void CheckMomentBounds() {
        if (this->MomentX < this->moment_bounds_[0]) this->MomentX = this->moment_bounds_[0];
        if (this->MomentX > this->moment_bounds_[1]) this->MomentX = this->moment_bounds_[1];
        if (this->MomentY < this->moment_bounds_[2]) this->MomentY = this->moment_bounds_[2];
        if (this->MomentY > this->moment_bounds_[3]) this->MomentY = this->moment_bounds_[3];
    }

    void CheckPositionBounds(int width, int height) {
        if (this->GetX() + this->GetWidth() > width) this->hitbox_.x = width - this->GetWidth();
        if (this->GetX() < 0) this->hitbox_.x = 0;
        if (this->GetY() + this->GetHeight() > height) this->hitbox_.y = height - this->GetHeight();
        if (this->GetY() < 0) this->hitbox_.y = 0;
    }

    void Render() {
        if (this->rotate_axis_.x != -1 && this->rotate_axis_.y != -1)
            SDL_RenderCopyEx(this->renderer_, this->texture_, NULL, &this->hitbox_, this->rotation_, &this->rotate_axis_, SDL_FLIP_NONE);
        else
            SDL_RenderCopyEx(this->renderer_, this->texture_, NULL, &this->hitbox_, this->rotation_, NULL, SDL_FLIP_NONE);
    }

    int CreateImageSurface(const char * path) {
        std::string spath = "resources/img/";
        spath += path;
        SDL_Surface * surface = IMG_Load(spath.c_str());

        if (surface == NULL) {
            ConsoleOutput("Failed loading image: %s\n", IMG_GetError());
            errno = GAME_ERROR_GENERAL_FAIL;
            return errno;
        }

        this->surface_ = *surface;
        this->surface_original_ = *surface;
        return 0;
    }

    void LoadTextureFromSurface() {
        this->texture_ = SDL_CreateTextureFromSurface(this->renderer_, &this->surface_);
        SDL_QueryTexture(this->texture_, NULL, NULL, &this->hitbox_.w, &this->hitbox_.h);
    }

    void FreeSurface() {
        SDL_FreeSurface(&this->surface_);
    }

    void Resize(int width, int height) {
        // this->hitbox_.w = width;
        // this->hitbox_.h = height;
    }

    void ResizeFlat(int width) {
        double zoom = this->hitbox_.w / width;
        SDL_Surface * surface = rotozoomSurface(&this->surface_original_, this->rotation_, zoom, 1);
        printf("rotozoomed\n");
        SDL_FreeSurface(&this->surface_);
        printf("freed\n");
        this->surface_ = *surface;
        printf("copying\n");
        // SDL_FreeSurface(surface);
    }

    void Position(int x, int y) {
        this->hitbox_.x = x;
        this->hitbox_.y = y;
    }

    void UpdatePosition() {
        this->hitbox_.x += this->MomentX;
        this->hitbox_.y += this->MomentY;
    }

    void Rotate(int angle, SDL_Point center = {-1, -1}) {
        this->rotate_axis_ = center;
        this->rotation_ = angle;
    }

    void Crop(int x, int y, int w, int h) {
        SDL_Surface * surface = SDL_CreateRGBSurface(this->surface_original_.flags, w, h, this->surface_original_.format->BitsPerPixel,
            this->surface_original_.format->Rmask, this->surface_original_.format->Gmask, this->surface_original_.format->Bmask, this->surface_original_.format->Amask);
        SDL_Rect rect = {x, y, w, h};

        SDL_BlitSurface(&this->surface_original_, &rect, surface, 0);
        SDL_FreeSurface(&this->surface_); // crashes without this, memory leak without it anyways
        this->surface_ = *surface;
    }

};

inline GameObject CreateObjectFromImage(SDL_Renderer * renderer, const char * path, int width = 0, int height = 0, int cropx = 0, int cropy = 0, int cropw = 0, int croph = 0) {
    GameObject object (renderer);
    object.CreateImageSurface(path);
    if (cropw != 0 && croph != 0) object.Crop(cropx, cropy, cropw, croph);
    if (width != 0 && height != 0) object.ResizeFlat(width); // currently just supports proportional scaling
    object.LoadTextureFromSurface();
    object.FreeSurface();

    return object;
}

#endif
