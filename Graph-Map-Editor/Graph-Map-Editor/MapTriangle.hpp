//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MapTriangle_HEADER
#define MapTriangle_HEADER

#include <array>
#include <memory>

#include "MapPoint.hpp"
#include "MapGroundType.hpp"

class MapTriangle {
	const std::array<std::weak_ptr<MapPoint>, 3> m_points;
	const MapGroundType* m_mapGroundType;
public:
	MapTriangle(std::array<std::weak_ptr<MapPoint>, 3> points);
	std::weak_ptr<MapPoint> operator[](std::size_t i);
	MapGroundType* getType();
};

#endif // !MapTriangle_HEADER


//end