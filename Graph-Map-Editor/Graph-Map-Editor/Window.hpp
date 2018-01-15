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
#include "MouseMoveHandler.hpp"
#include "MouseZoomHandler.hpp"

class Window {
	sf::RenderWindow m_window;
	bool m_running;
	std::chrono::milliseconds m_sleep_duration;
	std::thread m_window_thread;
	const std::shared_ptr<sf::Font> m_font;
	MapEditor m_map_editor;
	std::unique_ptr<ViewPort> m_view_port;
	std::vector<std::shared_ptr<sf::Texture>> m_textures;
	bool m_mouse_left_bounce = false;
	MouseMoveHandler m_mouse_move_handler;
	MouseZoomHandler m_mouse_zoom_handler;

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
		m_map_editor.createMap(wykobi::make_vector(256.f, 256.f), wykobi::make_vector(10, 10), 1);



		//for (int x = 0; x < m_map_editor.getMap()->getMapSize().x; ++x) {
		//	for (int y = 0; y < m_map_editor.getMap()->getMapSize().y; ++y) {
		//		wykobi::point2d<int> p = wykobi::make_point(x, y);
		//		for (auto & ptr : m_map_editor.getMap()->getChunk(p)->getTriangles()) {
		//			std::cout << ptr->getRelations().size() << "\n";
		//		}
		//	}
		//}

		m_view_port = std::unique_ptr<ViewPort>(new ViewPort(*m_map_editor.getMap(), wykobi::make_point(0.f, 0.f), wykobi::make_vector(static_cast<float>(m_window.getSize().x), static_cast<float>(m_window.getSize().y)), 10));

		std::vector<std::string> tex_path{
			"../../resources/textures/void.png",
			"../../resources/textures/water.png",
			"../../resources/textures/grass_plain.png",
			"../../resources/textures/forrest.png",
			"../../resources/textures/desert.png",
			"../../resources/textures/mountain.png",
			"../../resources/textures/artic.png",
			"../../resources/textures/tundra.png",
			"../../resources/textures/jungle.png"
		};
		m_textures = std::vector<std::shared_ptr<sf::Texture>>(tex_path.size());
		for (std::size_t i = 0; i < tex_path.size(); ++i) {
			m_textures[i] = std::shared_ptr<sf::Texture>(new sf::Texture());
			if (m_textures[i]->loadFromFile(tex_path[i])) {
				m_view_port->getMapGroundTypeRenderer(i).setTexture(m_textures[i]);
			}
			//else {
			//	std::cout << "\"" << tex_path[i] << "\" failed\n";
			//}
		}

		m_mouse_move_handler.setActive(true);
		m_mouse_zoom_handler.setActive(true);

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
