//Author: Sivert Andresen Cubedo
#pragma once

#ifndef EditorTriangle_HEADER
#define EditorTriangle_HEADER

#include <array>

#include "MapPoint.hpp"

class EditorTriangle : public std::array<MapPoint*, 3> {
public:
	/*
	Constructor
	*/
	EditorTriangle(const std::array<MapPoint*, 3> points);

	/*
	Get MapPoint that is not input
	*/
	MapPoint* getOddPoint(MapPoint* p1, MapPoint* p2);
};

#endif // !EditorTriangle_HEADER

//end