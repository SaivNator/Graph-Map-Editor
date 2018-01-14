//Author: Sivert Andresen Cubedo

#include "MouseMoveHandler.hpp"

void MouseMoveHandler::activate(wykobi::point2d<float> pos) {
	m_active = true;
	m_start_pos = pos;
}

void MouseMoveHandler::update(wykobi::point2d<float> pos, sf::View & view) {
	if (m_active) {
		wykobi::vector2d<float> diff = m_start_pos - pos;
		view.move({ diff.x, diff.y });
	}
}

void MouseMoveHandler::deactivate() {
	m_active = false;
}

//end