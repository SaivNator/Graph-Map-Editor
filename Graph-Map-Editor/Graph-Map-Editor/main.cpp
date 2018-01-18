//Author: Sivert Andresen Cubedo

#include <ostream>
#include <sstream>
#include <iostream>
#include <vector>
#include <array>

#include <SFML\Graphics.hpp>

#include "Window.hpp"

#include "EditorPath.hpp"

int main() {

	std::vector<std::unique_ptr<MapPoint>> points;
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(0.f, 0.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(1.f, 0.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(1.f, 1.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(0.f, 1.f))));

	EditorPath path;
	path.push_back(points[0].get());
	path.push_back(points[1].get());
	path.push_back(points[2].get());
	path.push_back(points[3].get());


	
	
	std::cout << path.toString() << "\n";



	return EXIT_SUCCESS;
}

/*
int main() {
	std::shared_ptr<sf::Font> arial(new sf::Font());
	if (!arial->loadFromFile("../../resources/fonts/arial.ttf")) {
		std::cout << "Font load failed\n";
		return EXIT_FAILURE;
	}

	Window window(sf::VideoMode(1000, 600), "Window", arial);

	window.join();

	return EXIT_SUCCESS;
}
*/


//end