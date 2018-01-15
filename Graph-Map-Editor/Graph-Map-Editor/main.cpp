//Author: Sivert Andresen Cubedo

#include <ostream>
#include <sstream>
#include <iostream>
#include <vector>
#include <array>

#include <SFML\Graphics.hpp>

#include "Window.hpp"

#include "ImageUtil.hpp"

template <typename T>
std::string toString(wykobi::rectangle<T> & rect) {
	std::ostringstream s;
	s << "(" << rect[0].x << "," << rect[0].y
		<< ")->(" << rect[1].x << "," << rect[1].y << ")";
	return s.str();
}

int main() {

	wykobi::rectangle<int> rect = wykobi::make_rectangle<int>(0, 0, 256, 256);

	std::vector<wykobi::rectangle<int>> rect_vec(17, rect);

	rect = wykobi::make_rectangle<int>(0, 0, 12, 43);

	for (std::size_t i = 0; i < 7; ++i) {
		rect_vec.push_back(rect);
	}

	auto pair = ImageUtil::packRectangles<int>(rect_vec);

	std::cout << toString<int>(pair.first) << "\n";

	for (auto & r : pair.second) {
		std::cout << toString<int>(r) << "\n";
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