//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MapGroundTypeRenderer_HEADER
#define MapGroundTypeRenderer_HEADER



#include <memory>

#include <SFML/Graphics.hpp>

#include "MapTriangle.hpp"

class MapTriangle;

class MapGroundTypeRenderer : public sf::Drawable {
	sf::Color m_color;	//NOT CONNECTED!?!?!?!
	std::shared_ptr<sf::Texture> m_texture;
	sf::VertexArray m_vertex_array = sf::VertexArray(sf::Triangles);

	/*
	SFML draw function
	*/
	void draw(sf::RenderTarget & target, sf::RenderStates states) const;
public:
	/*
	Constructor
	*/
	MapGroundTypeRenderer();

	/*
	Clear m_vertex_array
	*/
	void clear();

	/*
	Append triangle to m_vertex_array
	*/
	void appendTriangle(MapTriangle & triangle);

	/*
	Set Color
	*/
	void setColor(sf::Color color);

	/*
	Set texture
	*/
	void setTexture(std::shared_ptr<sf::Texture> texture);

	/*
	Get color
	*/
	sf::Color getColor();

	/*
	Get texture
	*/
	std::shared_ptr<sf::Texture> getTexture();
};
#endif // !MapGroundTypeRenderer_HEADER

//end