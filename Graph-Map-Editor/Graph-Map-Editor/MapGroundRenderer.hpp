//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MapGroundRenderer_HEADER
#define MapGroundRenderer_HEADER



#include <memory>

#include <SFML/Graphics.hpp>

#include "MapTriangle.hpp"

class MapTriangle;

class MapGroundRenderer : public sf::Drawable {
	const std::shared_ptr<sf::Texture> m_texture;
	const std::vector<wykobi::rectangle<float>> m_tex_offset;
	sf::VertexArray m_vertex_array_tri = sf::VertexArray(sf::Triangles);
	sf::VertexArray m_vertex_array_line = sf::VertexArray(sf::Lines);
	bool m_show_triangle_texture = true;
	bool m_show_triangle_lines = false;
	bool m_show_triangle_relations = false;

	/*
	SFML draw function
	*/
	void draw(sf::RenderTarget & target, sf::RenderStates states) const;
public:
	/*
	Constructor
	*/
	MapGroundRenderer(std::shared_ptr<sf::Texture> tex_ptr, std::vector<wykobi::rectangle<float>> & tex_offset);

	/*
	Set show triangle texture
	*/
	void setShowTriangleTexture(bool v);

	/*
	Set show triangle lines
	*/
	void setShowTriangleLines(bool v);

	/*
	Set show triangle relations
	*/
	void setShowTriangleRelations(bool v);

	/*
	Clear m_vertex_array
	*/
	void clear();

	/*
	Append triangle to m_vertex_array
	*/
	void appendTriangle(MapTriangle & triangle);

	/*
	Get texture
	*/
	std::shared_ptr<sf::Texture> getTexture();
};
#endif // !MapGroundRenderer_HEADER

//end