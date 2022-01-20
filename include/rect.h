#ifndef RECT_H
#define RECT_H

#include "utils.h"

class Rect {
protected:

    SDL_Renderer * renderer_;
    SDL_Rect rect_;
    int border_pt_;
    Color border_color_;
    Color color_backup_;
    Color color_;

public:

    Rect() {}

    Rect(SDL_Renderer * renderer, int x, int y, int w, int h, Color rgb) {
        this->renderer_ = renderer;
        this->color_ = rgb;
        this->color_backup_ = rgb;
        this->rect_.x = x;
        this->rect_.y = y;
        this->rect_.w = w;
        this->rect_.h = h;
        this->border_color_ = rgb;
        this->border_pt_ = 0;
    }

    /// Renders border and rectangle
    void Render() {
        SDL_Rect newRect = {this->rect_.x + this->border_pt_, this->rect_.y + this->border_pt_, this->rect_.w - (this->border_pt_ * 2), this->rect_.h - (this->border_pt_ * 2)};

        SDL_SetRenderDrawColor(this->renderer_, this->border_color_.r, this->border_color_.g, this->border_color_.b, 255);
        SDL_RenderFillRect(this->renderer_, &this->rect_);

        SDL_SetRenderDrawColor(this->renderer_, this->color_.r, this->color_.g, this->color_.b, 255);
        SDL_RenderFillRect(this->renderer_, &newRect);
    }

    /// Sets r,g,b channels
    /// @param rgb  color channels (not alpha)
    void SetColor(Color rgb) {
        if (rgb.r < 0) rgb.r = 0;
        if (rgb.g < 0) rgb.g = 0;
        if (rgb.b < 0) rgb.b = 0;
        this->color_ = rgb;
        this->color_backup_ = rgb;
    }

    /// Gets color
    /// @return current color
    Color GetColor() const {
        return this->color_;
    }

    /// Set rect size
    /// @param x    x-coord
    /// @param y    y-coord
    /// @param w    width
    /// @param h    height
    void SetDimensions(int x, int y, int w, int h) {
        this->rect_.x = x;
        this->rect_.y = y;
        this->rect_.w = w;
        this->rect_.h = h;
    }

    /// Gets rect size
    /// @return rect size as rect
    SDL_Rect GetRect() const {
        return this->rect_;
    }

    /// Destroys memory
    void Destroy() {
        this->renderer_ = nullptr;
    }

    /// Sets border attributes
    /// @param pt       thickness (px)
    /// @param color    border color
    void SetBorder(int pt, Color color) {
        this->border_pt_ = pt;
        this->border_color_ = color;
    }

    /// Changes brightness
    /// @param percent  percent to decrease, 0 -> 1
    void TempShade(float percent) {
        int r = (float)this->color_backup_.r * percent;
        int g = (float)this->color_backup_.g * percent;
        int b = (float)this->color_backup_.b * percent;
        this->color_ = {r, g, b};
    }

    /// Moves rect
    /// @param x    x-coord
    /// @param y    y-coord
    void SetPosition(int x, int y) {
        this->rect_.x = x;
        this->rect_.y = y;
    }

};

class RectGroup {
private:

    SDL_Renderer * renderer_;
    Screen * screen_;
    map<string, Rect> rects_;

public:

    RectGroup() {}

    RectGroup(const RectGroup& copy) {
        this->operator=(copy);
    }

    void SetAttrib(Screen * screen, SDL_Renderer * renderer);

    void operator=(const RectGroup& copy);

    void Render();

    void Destroy();

    void TempShade(float percent);

    void AddRect(string uid, int x, int y, int w, int h, Color color);

    Rect &operator[](int iterindex);

    Rect at(string uid) const;

    Rect &operator[](string uid);

};

#endif
