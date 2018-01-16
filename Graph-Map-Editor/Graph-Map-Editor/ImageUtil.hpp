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
	std::pair<sf::Image, std::vector<wykobi::rectangle<int>>> packImages(std::vector<sf::Image> & image_vec);

	/*
	Find nearest power of 2
	*/
	template <class T>
	T nearestPowerOf2(T v) {
		return static_cast<T>(std::pow(static_cast<T>(2), std::round(std::log(v) / std::log(static_cast<T>(2)))));
	}

	/*
	Next higher power of 2
	*/
	template <class T>
	T nextPowerOf2(T v) {
		return static_cast<T>(std::pow(static_cast<T>(2), std::ceil(std::log(v) / std::log(static_cast<T>(2)))));
	}

	/*
	Check if number is even
	*/
	template <typename T>
	bool isEven(T v) {
		return !static_cast<bool>(std::remainder<T>(v, static_cast<T>(2)));
	}

	/*
	Get next even power of 2
	*/
	template <typename T>
	T nextEvenPowerOf2(T v) {
		T exp = static_cast<T>(std::ceil(std::log(v) / std::log(static_cast<T>(2))));
		if (isEven<T>(exp)) {
			return static_cast<T>(std::pow(static_cast<T>(2), exp));
		}
		else {
			return static_cast<T>(std::pow(static_cast<T>(2), exp + static_cast<T>(1)));
		}
	}

	/*
	Order rectangle
	*/
	template <typename T>
	wykobi::rectangle<T> orderRectangle(wykobi::rectangle<T> & rect) {
		return wykobi::make_rectangle<T>(
			std::min(rect[0].x, rect[1].x),
			std::min(rect[0].y, rect[1].y),
			std::max(rect[0].x, rect[1].x),
			std::max(rect[0].y, rect[1].y)
		);
	}

	/*
	Rectangle max length of x and y
	*/
	template <typename T>
	T rectMaxLength(wykobi::rectangle<T> & rect) {
		wykobi::rectangle<T> r = orderRectangle<T>(rect);
		return std::max(r[1].x - r[0].x, r[1].y - r[0].y);
	}

	/*
	Pack rectangles in to a rectangle
	Will pack all rects in same size power of 2 square
	*/
	template <typename T>
	std::pair<wykobi::rectangle<T>, std::vector<wykobi::rectangle<T>>> packRectangles(std::vector<wykobi::rectangle<T>> & rect_vec) {
		std::vector<std::pair<std::size_t, wykobi::rectangle<T>>> pending_rect;
		pending_rect.reserve(rect_vec.size());
		for (std::size_t i = 0; i < rect_vec.size(); ++i) {
			pending_rect.push_back(std::make_pair(i, rect_vec[i]));
		}
		std::sort(pending_rect.begin(), pending_rect.end(),
			[&](auto & p0, auto & p1) { return (rectMaxLength<T>(p0.second) < rectMaxLength<T>(p1.second)); }
		);
		wykobi::rectangle<T> & largest_rect = pending_rect.back().second;
		T side = rectMaxLength<T>(largest_rect);
		side = nextPowerOf2<T>(side);
		wykobi::rectangle<T> square_container = wykobi::make_rectangle<T>(0, 0, side, side);
		std::size_t number_of_squares = nextEvenPowerOf2<std::size_t>(pending_rect.size());
		std::size_t index_length = static_cast<std::size_t>(std::sqrt<std::size_t>(number_of_squares));
		T real_length = static_cast<T>(index_length) * side;
		wykobi::rectangle<T> bounding_square = wykobi::make_rectangle(static_cast<T>(0), static_cast<T>(0), real_length, real_length);
		std::vector<std::pair<std::size_t, wykobi::rectangle<T>>> placed_rect;
		placed_rect.reserve(pending_rect.size());
		for (std::size_t x = 0; (x < index_length && !pending_rect.empty()); ++x) {
			for (std::size_t y = 0; (y < index_length && !pending_rect.empty()); ++y) {
				std::pair<std::size_t, wykobi::rectangle<T>> p = pending_rect.back();
				pending_rect.pop_back();
				wykobi::vector2d<T> offset = wykobi::make_vector<T>(static_cast<T>(x) * side, static_cast<T>(y) * side);
				p.second = wykobi::translate(offset, p.second);
				placed_rect.push_back(p);
			}
		}
		std::vector<wykobi::rectangle<T>> out_rect(placed_rect.size());
		for (auto & pair : placed_rect) {
			out_rect[pair.first] = pair.second;
		}
		return std::pair<wykobi::rectangle<T>, std::vector<wykobi::rectangle<T>>>(bounding_square, out_rect);
	}
}

#endif // !ImageUtil_HEADER


//end
