#pragma once

#include <SDL2/SDL_image.h>
#include <map>
#include <string>
#include <vector>

struct RenderObject{
    std::string textureID;
    SDL_Rect destRect;
    SDL_Rect srcRect;
    float sortY;
    SDL_RendererFlip flip;

    bool operator<(const RenderObject& other) const{
        return sortY < other.sortY;
    }

    RenderObject();
};

class TextureManager{
public:
    static TextureManager* GetInstance();

    bool Load(const char* fileName, const char* id, SDL_Renderer* renderer);

    void Draw(const char* id, int x, int y, int width, int height, SDL_Renderer* renderer);

    void DrawFrame(const char* id, int x, int y, int destW, int destH, 
        int srcW, int srcH, int row, int frame, 
        SDL_Renderer* renderer, SDL_RendererFlip flip = SDL_FLIP_NONE);

    void DrawFrameRect(const char* id, SDL_Rect srcRect, SDL_Rect destRect, 
        SDL_Renderer* renderer, SDL_RendererFlip flip = SDL_FLIP_NONE);

    void Clear(const char* id);

private:
    TextureManager();
    ~TextureManager();

    std::map<std::string, SDL_Texture*> m_textureMap;

    static TextureManager* s_pInstance;
};