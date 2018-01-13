//Author: Sivert Andresen Cubedo

#include "Window.hpp"

Window::Window(sf::VideoMode mode, const std::string& title, std::shared_ptr<sf::Font> font_ptr, sf::Uint32 style, const sf::ContextSettings& settings) :
	m_running(true),
	m_font(font_ptr),
	m_window_thread(&Window::windowHandler, this, mode, title, style, settings)
{
}

void Window::windowHandler(sf::VideoMode mode, const std::string& title, sf::Uint32 style, const sf::ContextSettings& settings) {
	m_window.create(mode, title, style, settings);
	while (m_running) {
		eventHandler();
		renderHandler();
	}
	m_window.close();
}

//end