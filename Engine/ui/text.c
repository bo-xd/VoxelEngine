#include "text.h"
#include <SDL3/SDL_error.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string.h>

bool InitTextSystem() {
    if (TTF_Init() < 0) {
        SDL_Log("TTF_Init failed: %s", SDL_GetError());
        return false;
    }
    return true;
}

TextTexture CreateTextTexture(TTF_Font* font, const char* text, SDL_Color color) {
    TextTexture tex = {0};
    size_t len = strlen(text);
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, len, color);
    if (!surface) {
        SDL_Log("TTF_RenderText_Blended failed: %s", SDL_GetError());
        return tex;
    }

    SDL_Surface* rgbaSurface = SDL_CreateSurface(surface->w, surface->h, SDL_PIXELFORMAT_RGBA32);
    if (!rgbaSurface) {
        SDL_Log("Failed to create RGBA surface: %s", SDL_GetError());
        SDL_DestroySurface(surface);
        return tex;
    }

    if (SDL_BlitSurface(surface, NULL, rgbaSurface, NULL) < 0) {
        SDL_Log("BlitSurface failed: %s", SDL_GetError());
        SDL_DestroySurface(surface);
        SDL_DestroySurface(rgbaSurface);
        return tex;
    }

    SDL_DestroySurface(surface);

    tex.width = rgbaSurface->w;
    tex.height = rgbaSurface->h;

    glGenTextures(1, &tex.texture);
    glBindTexture(GL_TEXTURE_2D, tex.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, rgbaSurface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_DestroySurface(rgbaSurface);

    glGenVertexArrays(1, &tex.VAO);
    glGenBuffers(1, &tex.VBO);
    glBindVertexArray(tex.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, tex.VBO);

    float vertices[6*4] = {0};
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return tex;
}

void RenderTextTexture(shader* s, TextTexture* tex, float x, float y, int screenWidth, int screenHeight) {
    Shader_Use(s);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->texture);
    Shader_SetInt(s, "fontTexture", 0);

    mat4 ortho = Ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight, -1.0f, 1.0f);
    Shader_SetMat4(s, "projection", &ortho);

    float xpos = x;
    float ypos = screenHeight - y - tex->height;

    float w = (float)tex->width;
    float h = (float)tex->height;

    float vertices[6*4] = {
        xpos,     ypos + h, 0.0f, 0.0f,
        xpos,     ypos,     0.0f, 1.0f,
        xpos + w, ypos,     1.0f, 1.0f,

        xpos,     ypos + h, 0.0f, 0.0f,
        xpos + w, ypos,     1.0f, 1.0f,
        xpos + w, ypos + h, 1.0f, 0.0f
    };

    glBindVertexArray(tex->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, tex->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void FreeTextTexture(TextTexture* tex) {
    if (tex->texture) glDeleteTextures(1, &tex->texture);
    if (tex->VBO) glDeleteBuffers(1, &tex->VBO);
    if (tex->VAO) glDeleteVertexArrays(1, &tex->VAO);
}

void ShutdownTextSystem() {
    TTF_Quit();
}
