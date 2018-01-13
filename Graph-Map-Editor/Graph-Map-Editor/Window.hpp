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

	std::vector<std::shared_ptr<sf::Texture>> m_textures;

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
	
	*/

	/*
	TEST LOAD FUNCTION
	*/
	void testLoad() {
		m_map_editor.createMap(wykobi::make_vector(256.f, 256.f), wykobi::make_vector(10, 10), 0);
		
		//for (int x = 0; x < m_map_editor.getMap()->getMapSize().x; ++x) {
		//	for (int y = 0; y < m_map_editor.getMap()->getMapSize().y; ++y) {
		//		wykobi::point2d<int> p = wykobi::make_point(x, y);
		//		for (auto & ptr : m_map_editor.getMap()->getChunk(p)->getTriangles()) {
		//			std::cout << ptr->getRelations().size() << "\n";
		//		}
		//	}
		//}
		
		m_view_port = std::unique_ptr<ViewPort>(new ViewPort(*m_map_editor.getMap(), wykobi::make_point(0.f, 0.f), wykobi::make_vector(static_cast<float>(m_window.getSize().x), static_cast<float>(m_window.getSize().y)), 10));
		
		m_textures.push_back(std::shared_ptr<sf::Texture>(new sf::Texture()));
		if (!m_textures[0]->loadFromFile("../../resources/textures/void.png")) {
			std::cout << "Texture load failed\n";
		}

		m_view_port->getMapGroundTypeRenderer(0).setTexture(m_textures[0]);
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
