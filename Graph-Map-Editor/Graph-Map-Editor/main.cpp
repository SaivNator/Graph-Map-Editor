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

	std::size_t width = 10;
	std::size_t height = 10;

	//path
	for (std::size_t x = 0; x < width; ++x) {
		for (std::size_t y = 0; y < height; ++y) {
			points.push_back(std::unique_ptr<MapPoint>(new MapPoint(wykobi::make_point(static_cast<float>(x) * 100.f, static_cast<float>(y) * 100.f))));
		}
	}
	std::vector<EditorTriangle> triangle_vec;
	
	std::function<std::size_t(wykobi::point2d<std::size_t>)> to_arr = [&](wykobi::point2d<std::size_t> p){ return p.x + width * p.y; };

	for (std::size_t x = 0; x < 9; ++x) {
		for (std::size_t y = 0; y < 9; ++y) {

			std::size_t a = to_arr(wykobi::make_point(x, y));
			std::size_t b = to_arr(wykobi::make_point(x + 1, y + 1));
			std::size_t c = to_arr(wykobi::make_point(x, y + 1));
			std::size_t d = to_arr(wykobi::make_point(x + 1, y));

			
			EditorTriangle tri_1({ points[a].get(), points[b].get(), points[c].get() });
			EditorTriangle tri_2({ points[a].get(), points[d].get(), points[b].get() });

			triangle_vec.push_back(tri_1);
			triangle_vec.push_back(tri_2);

		}
	}
	
	//triangle_vec.push_back(EditorTriangle({ points[0].get(), points[0].get(), points[0].get() }));
	//triangle_vec.push_back(EditorTriangle({ points[0].get(), points[1].get(), points[3].get() }));
	//triangle_vec.push_back(EditorTriangle({ points[1].get(), points[3].get(), points[4].get() }));
	//
	//triangle_vec.push_back(EditorTriangle({ points[1].get(), points[2].get(), points[4].get() }));
	//triangle_vec.push_back(EditorTriangle({ points[2].get(), points[4].get(), points[5].get() }));
	//
	//triangle_vec.push_back(EditorTriangle({ points[3].get(), points[4].get(), points[6].get() }));
	//triangle_vec.push_back(EditorTriangle({ points[4].get(), points[6].get(), points[7].get() }));
	//
	//triangle_vec.push_back(EditorTriangle({ points[4].get(), points[5].get(), points[7].get() }));
	//triangle_vec.push_back(EditorTriangle({ points[5].get(), points[7].get(), points[8].get() }));

	std::cout << "Triangles:\n";
	for (EditorTriangle & t : triangle_vec) {
		std::cout << t.toString() << "\n";
	}

	std::cout << "Merge:\n";
	EditorPath merged_path = MergeTriangles::mergeTriangle(triangle_vec);
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