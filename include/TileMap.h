#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class TileMap : public sf::Drawable, public sf::Transformable
{
private:
    sf::VertexArray vertices;
    sf::Texture tileset;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
    bool load(const std::string& tilesetFile,
              sf::Vector2u tileSize,
              const std::vector<int>& tiles,
              unsigned int width,
              unsigned int height);

    void setColor(sf::Color color);
    void resetColor();
};