#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

namespace chess {

class TextureManager {
public:
    static sf::Texture& getTexture(const std::string& path);
    static void clearCache();

private:
    static std::unordered_map<std::string, sf::Texture> textures_;
};

} // namespace chess
