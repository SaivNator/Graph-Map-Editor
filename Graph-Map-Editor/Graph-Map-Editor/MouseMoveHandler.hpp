//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MouseMoveHandler_HEADER
#define MouseMoveHandler_HEADER

#include <SFML\Graphics.hpp>

#include <wykobi.hpp>

class MouseMoveHandler {
	bool m_active = false;
	wykobi::point2d<float> m_start_pos;
public:
	/*
	Activate handler, when mouse button is clicked
	*/
	void activate(wykobi::point2d<float> pos);
	
	/*
	Update view, when mouse button is down and is dragging
	*/
	void update(wykobi::point2d<float> pos, sf::View & view);

	/*
	Deactivate handler, when mouse button is released
	*/
	void deactivate();
};

#endif // !MouseMoveHandler_HEADER

//end