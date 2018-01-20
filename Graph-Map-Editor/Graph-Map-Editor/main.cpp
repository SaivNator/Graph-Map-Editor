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
	//path
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(0.f, 0.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(400.f, 0.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(400.f, 400.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(0.f, 400.f))));

	//hulls
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(20.f, 20.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(80.f, 20.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(80.f, 80.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(20.f, 80.f))));

	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(320.f, 320.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(380.f, 320.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(380.f, 380.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(320.f, 380.f))));

	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(20.f, 320.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(80.f, 320.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(80.f, 380.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(20.f, 380.f))));

	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(320.f, 20.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(380.f, 20.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(380.f, 80.f))));
	points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(320.f, 80.f))));


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

	EditorPath hull_3;
	hull_3.push_back(points[12].get());
	hull_3.push_back(points[13].get());
	hull_3.push_back(points[14].get());
	hull_3.push_back(points[15].get());

	EditorPath hull_4;
	hull_4.push_back(points[16].get());
	hull_4.push_back(points[17].get());
	hull_4.push_back(points[18].get());
	hull_4.push_back(points[19].get());

	path.getHulls().push_back(hull_1);
	path.getHulls().push_back(hull_2);
	path.getHulls().push_back(hull_3);
	path.getHulls().push_back(hull_4);

	std::vector<EditorPath> path_vec = path.removeHull();

	std::cout << path.toString() << "\n";
	std::cout << hull_1.toString() << "\n";
	std::cout << hull_2.toString() << "\n";
	std::cout << hull_3.toString() << "\n";
	std::cout << hull_4.toString() << "\n";

	std::cout << "output:\n";
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