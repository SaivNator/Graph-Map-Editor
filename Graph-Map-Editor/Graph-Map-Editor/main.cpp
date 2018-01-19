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
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(200.f, 0.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(200.f, 200.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(0.f, 200.f))));

	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(20.f, 20.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(80.f, 20.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(80.f, 80.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(20.f, 80.f))));

	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(120.f, 120.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(180.f, 120.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(180.f, 180.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(120.f, 180.f))));


	EditorPath path;
	path.push_back(points[0].get());
	path.push_back(points[1].get());
	path.push_back(points[2].get());
	path.push_back(points[3].get());

	EditorPath hull_1;
	hull_1.push_back(points[4].get());
	hull_1.push_back(points[5].get());
	hull_1.push_back(points[6].get());
	hull_1.push_back(points[7].get());

	EditorPath hull_2;
	hull_2.push_back(points[8].get());
	hull_2.push_back(points[9].get());
	hull_2.push_back(points[10].get());
	hull_2.push_back(points[11].get());

	path.getHulls().push_back(hull_1);
	path.getHulls().push_back(hull_2);

	std::vector<EditorPath> path_vec = path.removeHull();

	std::cout << path.toString() << "\n";
	std::cout << hull_1.toString() << "\n";
	std::cout << hull_2.toString() << "\n";
	for (EditorPath & p : path_vec) {
		std::cout << p.toString() << "\n";
	}
	
	



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