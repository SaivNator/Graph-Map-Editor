//Author: Sivert Andresen Cubedo

#include <ostream>
#include <sstream>
#include <iostream>
#include <vector>
#include <array>

#include <SFML\Graphics.hpp>

#include "Window.hpp"

#include "EditorPath.hpp"

#include "MergeTriangles.hpp"

int main() {

	std::vector<std::unique_ptr<MapPoint>> points;
	//path
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(0.f, 0.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(400.f, 0.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(400.f, 400.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(0.f, 400.f))));

	
	EditorPath path;
	for (std::size_t i = 0; i < 4; ++i) {
		path.push_back(points[i].get());
	}

	std::cout << "Path:\n";
	std::cout << path.toString() << "\n";

	std::cout << "Triangulate:\n";
	auto triangulated_path = path.triangulate();
	for (EditorTriangle t : triangulated_path) {
		std::cout << t.toString() << "\n";
	}

	std::cout << "Merge: \n";
	EditorPath merged_path = MergeTriangles::mergeTriangle(triangulated_path);
	std::cout << merged_path.toString() << "\n";






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