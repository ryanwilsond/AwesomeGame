#ifndef TEXT_H
#define TEXT_H

#include "utils.h"

class Text {
private:

    SDL_Renderer * renderer_;
    TTF_Font * font_;
    Color color_backup_;
    Color font_color_;
    SDL_Rect rect_;
    SDL_Texture * texture_;


public:

    Text() {}

    Text(SDL_Renderer * renderer, string font, int pt, Color color, string text = "") {
        this->font_ = TTF_OpenFont((string("resources/font/") + font + ".ttf").c_str(), pt);
        if (this->font_ == NULL) ConsoleOutput("%s\n", TTF_GetError());
        this->font_color_ = color;
        this->color_backup_ = color;
        this->rect_.x = 0;
        this->rect_.y = 0;
        this->renderer_ = renderer;

        this->SetText(text);
    }

    /// Changes text to render
    /// @param text new text
    void SetText(string text) {
        if (this->font_ == NULL) return;
        SDL_Color fontColor = {(unsigned char)this->font_color_.r, (unsigned char)this->font_color_.g, (unsigned char)this->font_color_.b, 255};
        SDL_Surface * surface = TTF_RenderText_Solid(this->font_, text.c_str(), fontColor);
        this->texture_ = SDL_CreateTextureFromSurface(this->renderer_, surface);
        TTF_SizeText(this->font_, text.c_str(), &this->rect_.w, &this->rect_.h);
        SDL_FreeSurface(surface);
    }

    void TempShade(float percent) {
        int r = (float)this->color_backup_.r * percent;
        int g = (float)this->color_backup_.g * percent;
        int b = (float)this->color_backup_.b * percent;
        this->font_color_ = {r, g, b};
    }

    /// Renders texture to screen
    void Render() const {
        SDL_RenderCopy(this->renderer_, this->texture_, NULL, &this->rect_);
    }

    /// Moves location of text
    /// @param x    x-coord
    /// @param y    y-coord
    void SetPosition(int x, int y) {
        this->rect_.x = x;
        this->rect_.y = y;
    }

    /// Gets text size
    /// @return text size as rect
    SDL_Rect GetRect() const {
        return this->rect_;
    }

    /// Destroys memory
    void Destroy() {
        SDL_DestroyTexture(this->texture_);
        this->renderer_ = nullptr;
    }
};

class Paragraph {
private:

    SDL_Renderer * renderer_;
    Screen * screen_;
    map<string, Text> lines_;

public:

    Paragraph() {}

    void SetAttrib(Screen * screen, SDL_Renderer * renderer) {
        this->renderer_ = renderer;
        this->screen_ = screen;
    }

    void operator=(const Paragraph& copy) {
        this->renderer_ = copy.renderer_;
        for (map<string, Text>::iterator it = ((map<string, Text>)copy.lines_).begin(); it != copy.lines_.end(); ++it) {
            this->lines_[it->first] = copy.lines_.at(it->first);
        }
    }

    void Destroy() {
        for (map<string, Text>::iterator it = this->lines_.begin(); it != this->lines_.end(); ++it) {
            this->lines_[it->first].Destroy();
        }
        this->renderer_ = nullptr;
    }

    void TempShade(float percent) {
        for (map<string, Text>::iterator it = this->lines_.begin(); it != this->lines_.end(); ++it) {
            this->lines_[it->first].TempShade(percent);
        }
    }

    void AddLine(string uid, int x, int y, string font, int pt, Color color, string text) {
        this->lines_[uid] = Text(this->renderer_, font, pt, color, text);
        if (x == Screen::CENTERED) x = (this->screen_->GetRect().w / 2) - (this->lines_[uid].GetRect().w / 2);
        if (y == Screen::CENTERED) y = (this->screen_->GetRect().h / 2) - (this->lines_[uid].GetRect().h / 2);
        this->lines_[uid].SetPosition(x, y);
    }

    Text &operator[](unsigned int iterindex) {
        vector<string> keys;

        for (map<string, Text>::iterator it = this->lines_.begin(); it != this->lines_.end(); ++it) {
            keys.push_back(it->first);
        }

        return this->lines_[keys[iterindex]];
    }

    Text &operator[](string uid) {
        return this->lines_[uid];
    }

    Text at(string uid) const {
        return this->lines_.at(uid);
    }

    void Render() {
        for (map<string, Text>::iterator it = this->lines_.begin(); it != this->lines_.end(); ++it) {
            this->lines_[it->first].Render();
        }
    }
};

#endif
