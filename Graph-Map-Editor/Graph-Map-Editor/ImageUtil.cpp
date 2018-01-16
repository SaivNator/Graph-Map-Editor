//Author: Sivert Andresen Cubedo

#include "ImageUtil.hpp"

std::pair<sf::Image, std::vector<wykobi::rectangle<int>>> ImageUtil::packImages(std::vector<sf::Image> & image_vec) {

	std::vector<wykobi::rectangle<int>> rect_vec;
	rect_vec.reserve(image_vec.size());
	//make rectangles
	for (sf::Image & image : image_vec) {
		wykobi::rectangle<int> rect = wykobi::make_rectangle<int>(0, 0, image.getSize().x/* + 1*/, image.getSize().y/* + 1*/);
		rect_vec.push_back(rect);
	}

	auto pair = packRectangles<int>(rect_vec);

	sf::Image out_image;

	out_image.create(pair.first[1].x, pair.first[1].y, sf::Color::Transparent);

	//copy over images
	for (std::size_t i = 0; i < image_vec.size(); ++i) {
		out_image.copy(image_vec[i], pair.second[i][0].x, pair.second[i][0].y);
	}
	
	
	return std::pair<sf::Image, std::vector<wykobi::rectangle<int>>>(out_image, pair.second);
}

//end