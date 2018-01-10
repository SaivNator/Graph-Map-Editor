//Author: Sivert Andresen Cubedo
#pragma once

#include <memory>

#include <SFML/Graphics.hpp>

#include "MapTriangle.hpp"

class MapGroundType : public sf::Drawable {
	std::shared_ptr<sf::Texture> m_texture;
	sf::VertexArray m_vertex_array = sf::VertexArray(sf::Triangles);

	void draw(sf::RenderTarget & target, sf::Transform states) const;
public:
	MapGroundType(std::shared_ptr<sf::Texture> texture);

	void clearVertexArray();
	void appendTriangle(MapTriangle* triangle);

	std::shared_ptr<sf::Texture> getTexture();
};

//end