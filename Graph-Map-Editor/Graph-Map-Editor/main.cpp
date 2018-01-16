//Author: Sivert Andresen Cubedo

#include <ostream>
#include <sstream>
#include <iostream>
#include <vector>
#include <array>

#include <SFML\Graphics.hpp>

#include "Window.hpp"

#include "ImageUtil.hpp"
#include "FileDialog.hpp"

template <typename T>
std::string toString(wykobi::rectangle<T> & rect) {
	std::ostringstream s;
	s << "(" << rect[0].x << "," << rect[0].y
		<< ")->(" << rect[1].x << "," << rect[1].y << ")";
	return s.str();
}

int main() {

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

	auto pair = ImageUtil::packImages(image_vec);

	FileDialog::SaveFile save;
	save.create();
	pair.first.saveToFile(save.getPath());

	for (auto & rect : pair.second) {
		std::cout << toString<int>(rect) << "\n";
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