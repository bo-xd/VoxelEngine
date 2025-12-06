#ifndef TEXT_H
#define TEXT_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <GL/glew.h>
#include "../utils/MathUtil.h"
#include "../Shaderer.h"

typedef struct {
    GLuint VAO, VBO;
    GLuint texture;
    int width, height;
} TextTexture;

bool InitTextSystem();
TextTexture CreateTextTexture(TTF_Font* font, const char* text, SDL_Color color);
void RenderTextTexture(shader* s, TextTexture* tex, float x, float y, int screenWidth, int screenHeight);
void FreeTextTexture(TextTexture* tex);
void ShutdownTextSystem();

#endif
