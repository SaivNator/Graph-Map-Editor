//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MouseZoomHandler_HEADER
#define MouseZoomHandler_HEADER

#include <SFML\Graphics.hpp>

class MouseZoomHandler {
	bool m_active = false;
	const float m_zoom_amount = 1.1;
public:
	/*
	Set active, true = on, false = off
	*/
	void setActive(bool v);

	/*
	When mouse wheel is scrolled
	*/
	void update(sf::Vector2i & pixel_pos, sf::View & view, sf::RenderWindow & window, float delta);
};

#endif // !MouseZoomHandler_HEADER


//end