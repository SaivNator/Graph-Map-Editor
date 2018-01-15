//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MouseMoveHandler_HEADER
#define MouseMoveHandler_HEADER

#include <SFML\Graphics.hpp>

class MouseMoveHandler {
	bool m_active = false;
	bool m_clicked = false;
	sf::Vector2f m_start_pos;
public:
	/*
	Set active, true = on, false = off
	*/
	void setActive(bool v);

	/*
	Click handler, when mouse button is clicked
	*/
	void click(sf::Vector2f & pos);
	
	/*
	Update view, when mouse button is down and is dragging
	*/
	void update(sf::Vector2f & pos, sf::View & view);

	/*
	Release handler, when mouse button is released
	*/
	void release();
};

#endif // !MouseMoveHandler_HEADER

//end