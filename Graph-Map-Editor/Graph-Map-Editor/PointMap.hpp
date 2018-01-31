//Author: Sivert Andresen Cubedo
#pragma once

#ifndef PointMap_HEADER
#define PointMap_HEADER

#include <iostream>
#include <map>
#include <vector>
#include <wykobi.hpp>

template <typename T>
class PointMap2d {
	std::map<T, std::map<T, wykobi::point2d<T>>> m_map;
public:
	
	struct iterator : public std::bidirectional_iterator_tag {
		iterator(const iterator&);
		~iterator();
		iterator& operator=(const iterator&);
		iterator& operator++();
		reference operator*() const;
		friend void swap(iterator& lhs, iterator& rhs);
	};

	void emplace(wykobi::point2d<T> point);

	iterator find(iterator it);

	void erase(iterator it);
};

#endif // !PointMap_HEADER


//end