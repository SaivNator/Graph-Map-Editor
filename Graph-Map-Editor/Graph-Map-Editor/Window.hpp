//Author: Sivert Andresen Cubedo
#pragma once

#ifndef Window_HEADER
#define Window_HEADER

#include <iostream>
#include <thread>
#include <chrono>
#include <memory>

#include <SFML\Graphics.hpp>
#include <wykobi.hpp>

#include "MapEditor.hpp"
#include "ViewPort.hpp"

class Window {
	sf::RenderWindow m_window;
	bool m_running;
	std::chrono::milliseconds m_sleep_duration;
	std::thread m_window_thread;
	const std::shared_ptr<sf::Font> m_font;

	MapEditor m_map_editor;
	std::unique_ptr<ViewPort> m_view_port;

	/*
	Window thread function
	*/
	void windowHandler(sf::VideoMode mode, const std::string& title, sf::Uint32 style, const sf::ContextSettings& settings);

	/*
	Handles window events (must be in window thread)
	*/
	void eventHandler();

	/*
	Handles window rendering
	*/
	void renderHandler();

	/*
	TEST LOAD FUNCTION
	*/
	void testLoad() {
		m_map_editor.createMap(wykobi::make_vector(256.f, 256.f), wykobi::make_vector(10, 10), 10, 0);
		//m_view_port = std::unique_ptr<ViewPort>(new ViewPort(*m_map_editor.getMap(), wykobi::make_point(0.f, 0.f), wykobi::make_vector(100.f, 100.f)));
	}
public:
	/*
	Constructor
	Start window thread
	*/
	Window(sf::VideoMode mode, const std::string& title, std::shared_ptr<sf::Font> font_ptr, sf::Uint32 style = sf::Style::Default, const sf::ContextSettings& settings = sf::ContextSettings());
	
	/*
	Join window thread
	*/
	void join();

	/*
	Kill window thread
	*/
	void close();
};

#endif // !Window_HEADER


//end
