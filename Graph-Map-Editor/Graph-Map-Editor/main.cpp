//Author: Sivert Andresen Cubedo

#include <iostream>
#include <vector>
#include <array>

#include <wykobi.hpp>

#include <SFML\Graphics.hpp>

#include "FileDialog.hpp"


int main() {

	wykobi::point2d<float> p0 = wykobi::make_point(100.f, 100.f);

	wykobi::triangle<float, 2> tri;

	std::cout << p0.x << "\t" << p0.y << "\n";

	sf::RenderWindow window;

	window.create(sf::VideoMode(500, 500), "TEST", sf::Style::Resize | sf::Style::Resize | sf::Style::Titlebar | sf::Style::Close);

	FileDialog::OpenFile dialog;

	dialog.create();

	std::cout << dialog.getPath() << "\n";


	return EXIT_SUCCESS;
}



//end