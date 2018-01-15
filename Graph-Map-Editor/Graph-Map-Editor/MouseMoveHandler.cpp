//Author: Sivert Andresen Cubedo

#include "MouseMoveHandler.hpp"

void MouseMoveHandler::setMode(bool mode) {
	m_active = mode;
}

void MouseMoveHandler::click(sf::Vector2f & pos) {
	if (m_active) {
		m_clicked = true;
		m_start_pos = pos;
	}
}

void MouseMoveHandler::update(sf::Vector2f & pos, sf::View & view) {
	if (m_active && m_clicked) {
		sf::Vector2f diff = m_start_pos - pos;
		view.move(diff);
	}
}

void MouseMoveHandler::release() {
	if (m_active) {
		m_clicked = false;
	}
}

//end