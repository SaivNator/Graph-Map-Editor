//Author: Sivert Andresen Cubedo

#include "MouseZoomHandler.hpp"

void MouseZoomHandler::setActive(bool v) {
	m_active = v;
}

void MouseZoomHandler::update(sf::Vector2i & pixel_pos, sf::View & view, sf::RenderWindow & window, float delta) {
	if (m_active) {
		float zoom;
		if (delta > 0.f) {
			zoom = 1.f / m_zoom_amount;
		}
		else if (delta < 0.f) {
			zoom = m_zoom_amount;
		}
		else {
			return;
		}
		const sf::Vector2f before_coord = window.mapPixelToCoords(pixel_pos, view);
		view.zoom(zoom);
		const sf::Vector2f after_coord = window.mapPixelToCoords(pixel_pos, view);
		const sf::Vector2f offset_coords = before_coord - after_coord;
		view.move(offset_coords);
	}
}

//end