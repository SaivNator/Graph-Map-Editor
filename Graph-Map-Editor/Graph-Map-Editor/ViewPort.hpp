//Author: Sivert Andresen Cubedo
#pragma once

#ifndef ViewPort_HEADER
#define ViewPort_HEADER

#include <iostream>

#include <SFML\Graphics.hpp>

#include <wykobi.hpp>

#include "MapChunk.hpp"
#include "Map.hpp"
#include "MapGroundType.hpp"
#include "MapGroundTypeRenderer.hpp"

class ViewPort : public sf::Drawable {
	Map & m_map;
	sf::View m_view;
	std::vector<MapGroundTypeRenderer> m_type_renderer_vec;

	/*
	SFML draw
	*/
	void draw(sf::RenderTarget & target, sf::RenderStates states) const;
public:
	/*
	Constructor
	*/
	ViewPort(Map & map, wykobi::point2d<float> centre, wykobi::vector2d<float> size);

	/*
	Get view
	*/
	sf::View & getView();

	/*
	Get view rect in world cors
	*/
	wykobi::rectangle<float> getRect();

	/*
	Get MapGroundTypeRenderer
	*/
	MapGroundTypeRenderer & getMapGroundTypeRenderer(MapGroundType type);

	/*
	Convert view pos to map pos
	https://github.com/SFML/SFML/blob/master/src/SFML/Graphics/RenderTarget.cpp
	*/
	wykobi::point2d<float> viewToMapPos(wykobi::point2d<float> & pos);

	/*
	Convert map pos to view pos
	https://github.com/SFML/SFML/blob/master/src/SFML/Graphics/RenderTarget.cpp
	*/
	wykobi::point2d<float> mapToViewPos(wykobi::point2d<float> & pos);

	/*
	Update vertexarrays
	*/
	void render();
};

#endif // !ViewPort_HEADER


//end
