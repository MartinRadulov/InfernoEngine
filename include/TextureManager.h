#pragma once

#include <SDL2/SDL_image.h>
#include <map>
#include <string>

class TextureManager{
public:
    static TextureManager* GetInstance();

    bool Load(const char* fileName, const char* id, SDL_Renderer* renderer);

    void Draw(const char* id, int x, int y, int width, int height, SDL_Renderer* renderer);

    void Clear(const char* id);

private:
    TextureManager();
    ~TextureManager();

    std::map<std::string, SDL_Texture*> m_textureMap;

    static TextureManager* s_pInstance;
};