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
		eventHandler();
		renderHandler();
		std::this_thread::sleep_for(m_sleep_duration);
	}
	m_window.close();
}

void Window::eventHandler() {
	sf::Event e;
	while (m_window.pollEvent(e)) {
		switch (e.type) {
		case sf::Event::Closed:
			m_running = false;
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