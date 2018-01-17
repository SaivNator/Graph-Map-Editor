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
#include "ImageUtil.hpp"

class Window {
	sf::RenderWindow m_window;
	bool m_running;
	std::chrono::milliseconds m_sleep_duration;
	std::thread m_window_thread;
	const std::shared_ptr<sf::Font> m_font;
	MapEditor m_map_editor;
	std::unique_ptr<ViewPort> m_view_port;
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
		m_map_editor.createMap(wykobi::make_vector(256.f, 256.f), wykobi::make_vector(10, 10), 3);

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
		std::vector<sf::Image> image_vec(tex_path.size());
		for (std::size_t i = 0; i < image_vec.size(); ++i) {
			image_vec[i].loadFromFile(tex_path[i]);
		}
		std::pair<sf::Image, std::vector<wykobi::rectangle<int>>> img_pair = ImageUtil::packImages(image_vec);
		std::vector<wykobi::rectangle<float>> rect_vec(img_pair.second.size());
		for (std::size_t i = 0; i < rect_vec.size(); ++i) {
			rect_vec[i] = wykobi::make_rectangle<float>(
				static_cast<float>(img_pair.second[i][0].x),
				static_cast<float>(img_pair.second[i][0].y),
				static_cast<float>(img_pair.second[i][1].x),
				static_cast<float>(img_pair.second[i][1].y)
			);
		}
		std::shared_ptr<sf::Texture> ground_texture(new sf::Texture());
		ground_texture->loadFromImage(img_pair.first);

		m_view_port = std::unique_ptr<ViewPort>(new ViewPort(
			*m_map_editor.getMap(),
			wykobi::make_point<float>(0.f, 0.f),
			wykobi::make_vector<float>(static_cast<float>(m_window.getSize().x), static_cast<float>(m_window.getSize().y)),
			ground_texture,
			rect_vec
		));
			

		m_mouse_move_handler.setActive(true);
		m_mouse_zoom_handler.setActive(true);

		m_view_port->getMapGroundRenderer().setShowTriangleTexture(true);
		m_view_port->getMapGroundRenderer().setShowTriangleLines(true);
		m_view_port->getMapGroundRenderer().setShowTriangleRelations(false);

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
