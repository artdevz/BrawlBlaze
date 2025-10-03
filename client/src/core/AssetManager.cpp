#include "core/AssetManager.hpp"

AssetManager& AssetManager::Get() {
    static AssetManager instance;
    return instance;
}

Texture2D AssetManager::LoadTexture(const std::string& id, const std::string& path) {
    if (textures.find(id) == textures.end()) textures[id] = ::LoadTexture(path.c_str());
    return textures[id];
}

Sound AssetManager::LoadSound(const std::string& id, const std::string& path) {
    if (sounds.find(id) == sounds.end()) sounds[id] = ::LoadSound(path.c_str());
    return sounds[id];
}

const Texture2D AssetManager::GetTexture(const std::string& id) const {
    auto it = textures.find(id);
    return (it != textures.end()) ? it->second : Texture2D{};
}

Sound AssetManager::GetSound(const std::string& id) const {
    auto it = sounds.find(id);
    return (it != sounds.end()) ? it->second : Sound{};
}

void AssetManager::LoadTextures() {
    LoadTexture("human", "assets/sprites/Human.png");
    LoadTexture("bullet", "assets/sprites/Bullet.png");
    LoadTexture("floor", "assets/sprites/FloorTile.png");
}

void AssetManager::UnloadTextures() {
    for (auto& [id, texture] : textures) UnloadTexture(texture);
    textures.clear();
}

void AssetManager::UnloadSounds() {
    for (auto& [id, sound] : sounds) UnloadSound(sound);
    sounds.clear();
}

void AssetManager::UnloadAll() {
    UnloadTextures();
    UnloadSounds();
}