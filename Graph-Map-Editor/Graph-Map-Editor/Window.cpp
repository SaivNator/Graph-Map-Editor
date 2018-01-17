//Author: Sivert Andresen Cubedo

#include "Window.hpp"

Window::Window(sf::VideoMode mode, const std::string& title, std::shared_ptr<sf::Font> font_ptr, sf::Uint32 style, const sf::ContextSettings& settings) :
	m_running(true),
	m_sleep_duration(16),
	m_font(font_ptr),
	m_window_thread(&Window::windowHandler, this, mode, title, style, settings)
{
}

void Window::windowHandler(sf::VideoMode mode, const std::string& title, sf::Uint32 style, const sf::ContextSettings& settings) {
	m_window.create(mode, title, style, settings);

	testLoad();

	while (m_running) {
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
		eventHandler();
		renderHandler();
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		std::this_thread::sleep_for(m_sleep_duration - std::chrono::duration_cast<std::chrono::milliseconds>(end - start));
	}
	m_window.close();
}

void Window::eventHandler() {
	sf::Event e;
	while (m_window.pollEvent(e)) {
		sf::Vector2i mouse_pixel_pos;
		sf::Vector2f mouse_world_pos;
		switch (e.type) {
		case sf::Event::Closed:
			m_running = false;
			break;
		case sf::Event::MouseButtonPressed:
			mouse_pixel_pos = { e.mouseButton.x, e.mouseButton.y };
			mouse_world_pos = m_window.mapPixelToCoords(mouse_pixel_pos, m_view_port->getView());
			switch (e.mouseButton.button) {
			case sf::Mouse::Left:
				
				break;
			case sf::Mouse::Right:
				if (!m_mouse_left_bounce) {
					m_mouse_move_handler.click(mouse_world_pos);
					m_mouse_left_bounce = true;
				}
				break;
			default:
				break;
			}
			break;
		case sf::Event::MouseButtonReleased:
			switch (e.mouseButton.button) {
			case sf::Mouse::Left:
				break;
			case sf::Mouse::Right:
				if (m_mouse_left_bounce) {
					m_mouse_move_handler.release();
					m_mouse_left_bounce = false;
				}
				break;
			default:
				break;
			}
			break;
		case sf::Event::MouseMoved:
			mouse_pixel_pos = { e.mouseMove.x, e.mouseMove.y };
			mouse_world_pos = m_window.mapPixelToCoords(mouse_pixel_pos, m_view_port->getView());
			m_mouse_move_handler.update(mouse_world_pos, m_view_port->getView());
			break;
		case sf::Event::MouseWheelScrolled:
			mouse_pixel_pos = { e.mouseWheelScroll.x, e.mouseWheelScroll.y };
			mouse_world_pos = m_window.mapPixelToCoords(mouse_pixel_pos, m_view_port->getView());
			m_mouse_zoom_handler.update(mouse_pixel_pos, m_view_port->getView(), m_window, e.mouseWheelScroll.delta);
			break;
		default:
			break;
		}
	}
}

void Window::renderHandler() {
	m_window.clear();
	if (m_view_port) {
		m_view_port->render();
		m_window.draw(*m_view_port);
	}
	m_window.display();
}

void Window::join() {
	if (m_window_thread.joinable()) {
		m_window_thread.join();
	}
}

void Window::close() {
	m_running = false;
}

//end