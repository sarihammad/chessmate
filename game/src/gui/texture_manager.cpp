#include "gui/texture_manager.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_map>

namespace chess {

std::unordered_map<std::string, sf::Texture> TextureManager::textures_;

sf::Texture& TextureManager::getTexture(const std::string& path) {
    auto it = textures_.find(path);
    if (it != textures_.end()) {
        return it->second;
    }
    
    // Load new texture
    sf::Texture texture;
    if (!texture.loadFromFile(path)) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        throw std::runtime_error("Failed to load texture: " + path);
    }
    
    textures_[path] = texture;
    return textures_[path];
}

void TextureManager::clearCache() {
    textures_.clear();
}

} // namespace chess 