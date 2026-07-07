#include "TileMap.h"

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = &tileset;
    target.draw(vertices, states);
}

bool TileMap::load(const std::string& tilesetFile,
                  sf::Vector2u tileSize,
                  const std::vector<int>& tiles,
                  unsigned int width,
                  unsigned int height)
{
    if (!tileset.loadFromFile(tilesetFile))
        return false;

    vertices.setPrimitiveType(sf::Quads);
    vertices.resize(width * height * 4);

    for (unsigned int i = 0; i < width; i++)
    {
        for (unsigned int j = 0; j < height; j++)
        {
            int tileNumber = tiles[i + j * width];
            sf::Vertex* quad = &vertices[(i + j * width) * 4];

            if (tileNumber < 0) {
                for (int k = 0; k < 4; k++) quad[k].color = sf::Color::Transparent;
                continue;
            }

            int tu = tileNumber % (tileset.getSize().x / tileSize.x);
            int tv = tileNumber / (tileset.getSize().x / tileSize.x);

            quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
            quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
            quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
            quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

            quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
            quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
            quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
            quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
        }
    }
    return true;
}

void TileMap::setColor(sf::Color color)
{
    for (std::size_t i = 0; i < vertices.getVertexCount(); i++)
        vertices[i].color = color;
}

void TileMap::resetColor()
{
    setColor(sf::Color::White);
}
