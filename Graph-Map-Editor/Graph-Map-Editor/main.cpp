//Author: Sivert Andresen Cubedo

#include <iostream>
#include <vector>
#include <array>

#include <SFML\Graphics.hpp>

#include "Window.hpp"

int main() {
	std::shared_ptr<sf::Font> arial(new sf::Font());
	if (!arial->loadFromFile("../../resources/fonts/arial.ttf")) {
		std::cout << "Font load failed\n";
		return EXIT_FAILURE;
	}

	Window window(sf::VideoMode(500, 500), "Window", arial);

	window.join();

	return EXIT_SUCCESS;
}



//end