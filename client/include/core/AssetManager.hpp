#pragma once

#include <string>
#include <unordered_map>

#include <raylib.h>

class AssetManager {

public:
    static AssetManager& Get();

    Texture2D LoadTexture(const std::string& id, const std::string& path);
    Sound LoadSound(const std::string& id, const std::string& path);

    const Texture2D GetTexture(const std::string& id) const;
    Sound GetSound(const std::string& id) const;

    void LoadTextures();
    void UnloadTextures();

    void UnloadSounds(); 

    void UnloadAll();

private:
    std::unordered_map<std::string, Texture2D> textures;
    std::unordered_map<std::string, Sound> sounds;

};