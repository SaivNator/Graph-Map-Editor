//Author: Sivert Andresen Cubedo
#pragma once

#ifndef ImageUtil_HEADER
#define ImageUtil_HEADER

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <memory>
#include <tuple>
#include <functional>
#include <algorithm>

#include <SFML\Graphics.hpp>

#include <wykobi.hpp>
#include <wykobi_algorithm.hpp>

namespace ImageUtil {
	/*
	Copy image_vec to image
	Fill rect_vec with coords to images in order of image_vec
	*/
	std::pair<sf::Image, std::vector<wykobi::rectangle<float>>> mergeImages(std::vector<sf::Image> & image_vec);

	/*
	Find nearest power of 2
	*/
	template <class T>
	T nearestPowerOf2(T v) {
		return std::pow(static_cast<T>(2), std::round(std::log(v) / std::log(static_cast<T>(2))));
	}

	/*
	Next higher power of 2
	*/
	template <class T>
	T nextPowerOf2(T v) {
		return std::pow(static_cast<T>(2), std::ceil(std::log(v) / std::log(static_cast<T>(2))));
	}

	/*
	Pack rectangles in to a rectangle
	*/
	template <typename T>
	std::pair<wykobi::rectangle<T>, std::vector<wykobi::rectangle<T>>> packRectangles(std::vector<wykobi::rectangle<T>> & rect_vec) {
		std::vector<std::pair<std::size_t, std::reference_wrapper<wykobi::rectangle<T>>>> pending_rect;
		pending_rect.reserve(rect_vec.size());
		for (std::size_t i = 0; i < rect_vec.size(); ++i) {
			pending_rect.push_back(std::make_pair(i, rect_vec[i]));
		}
		std::sort(pending_rect.begin(), pending_rect.end(),
			[&](auto & p0, auto & p1){ return (wykobi::area(p0.second) < wykobi::area(p1.second)); }
		);

		std::vector<wykobi::rectangle<T>> free_areas;
		std::vector<std::pair<std::size_t, wykobi::rectangle<float>>> placed_rect;

		placed_rect.push_back(pending_rect.back());

		while (!pending_rect.empty()) {
			//make new free areas
			


		}



		placed_rect.reserve(pending_rect.size());

		//first rect
		placed_rect.push_back(pending_rect.back());
		pending_rect.pop_back();
		
	}
}

#endif // !ImageUtil_HEADER


//end
