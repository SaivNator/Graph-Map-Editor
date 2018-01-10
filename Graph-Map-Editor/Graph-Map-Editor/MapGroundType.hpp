//Author: Sivert Andresen Cubedo
#pragma once

#include <memory>

#include <SFML/Graphics.hpp>

#include "MapTriangle.hpp"

class MapGroundType : public sf::Drawable {
	std::shared_ptr<sf::Texture> m_texture;
	sf::VertexArray m_vertex_array = sf::VertexArray(sf::Triangles);

	/*
	SFML draw function
	*/
	void draw(sf::RenderTarget & target, sf::Transform states) const;
public:
	/*
	Constructor
	*/
	MapGroundType(std::shared_ptr<sf::Texture> texture);

	/*
	Clear m_vertex_array
	*/
	void clearVertexArray();

	/*
	Append triangle to m_vertex_array
	*/
	void appendTriangle(MapTriangle* triangle);

	std::shared_ptr<sf::Texture> getTexture();
};

//end