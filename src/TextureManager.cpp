#include "../include/TextureManager.h"
#include <iostream>

TextureManager* TextureManager::s_pInstance = nullptr;

TextureManager* TextureManager::GetInstance(){
    if(s_pInstance == nullptr){
        s_pInstance = new TextureManager();
    }
    return s_pInstance;
}

TextureManager::TextureManager(){};
TextureManager::~TextureManager(){};

bool TextureManager::Load(const char* fileName, const char* id, SDL_Renderer* renderer){
    SDL_Surface* pTempSurface = IMG_Load(fileName);

    if(pTempSurface == nullptr){
        std::cout<<"Error handling image " << fileName << IMG_GetError() << std::endl;
        return false;
    }

    SDL_Texture* pTexture = SDL_CreateTextureFromSurface(renderer, pTempSurface);

    SDL_FreeSurface(pTempSurface);

    if(pTexture == nullptr){
        std::cout<<"Error creating image from surface "<< SDL_GetError()<<std::endl;
        return false;
    }

    m_textureMap[std::string(id)] = pTexture;
    return true;
}

void TextureManager::Draw(const char* id, int x, int y, int width, int height, SDL_Renderer* renderer){
    SDL_Rect destRect = {x, y, width, height};
    SDL_Rect dest = {x, y, width, height};

    SDL_RenderCopy(renderer, m_textureMap[id], NULL, &dest);
}

void TextureManager::Clear(const char* id){
    SDL_Texture* pTexture = m_textureMap[id];

    SDL_DestroyTexture(pTexture);

    m_textureMap.erase(id);
}

void TextureManager::DrawFrame(const char* id, int x, int y, int destW, int destH, 
                               int srcW, int srcH, int row, int frame, 
                               SDL_Renderer* renderer, SDL_RendererFlip flip){
    SDL_Rect srcRect = { srcW * frame, srcH * row, srcW, srcH };
    SDL_Rect destRect = { x, y, destW, destH };

    // Pass the calculated work to the Master function
    DrawFrameRect(id, srcRect, destRect, renderer, flip);
}

void TextureManager::DrawFrameRect(const char* id, SDL_Rect srcRect, SDL_Rect destRect, 
                                   SDL_Renderer* renderer, SDL_RendererFlip flip) 
{
    // Safety Check
    if (m_textureMap[id] == nullptr) return;

    SDL_RenderCopyEx(renderer, m_textureMap[id], &srcRect, &destRect, 0, 0, flip);
}

RenderObject::RenderObject(){
    flip = SDL_FLIP_NONE; 
        srcRect = {0, 0, 0, 0};
        destRect = {0, 0, 0, 0};
        sortY = 0.0f;
}