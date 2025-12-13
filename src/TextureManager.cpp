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
    SDL_Texture* pTexture = m_textureMap[id];

    SDL_Rect destRect = {x, y, width, height};

    SDL_RenderCopy(renderer, pTexture, NULL, &destRect);
}

void TextureManager::Clear(const char* id){
    SDL_Texture* pTexture = m_textureMap[id];

    SDL_DestroyTexture(pTexture);

    m_textureMap.erase(id);
}

void TextureManager::DrawFrame(const char* id, int x, int y, int destW, int destH, 
                               int srcW, int srcH, int row, int frame, 
                               SDL_Renderer* renderer, SDL_RendererFlip flip){
    SDL_Rect srcRect;
    SDL_Rect destRect;

    // 1. SELECT THE CHUNK FROM THE SHEET
    // If your sheet is a grid, this math finds the top-left corner of the frame
    srcRect.x = srcW * frame;  
    srcRect.y = srcH * row;    
    srcRect.w = srcW;
    srcRect.h = srcH;

    // 2. DEFINE WHERE IT GOES ON SCREEN
    destRect.x = x;
    destRect.y = y;
    destRect.w = destW; // SDL will automatically shrink/stretch it to fit here!
    destRect.h = destH;

    // 3. DRAW
    // We use RenderCopyEx to allow flipping (useful if you only draw the Right animation and flip it for Left)
    SDL_RenderCopyEx(renderer, m_textureMap[id], &srcRect, &destRect, 0, 0, flip);
        }