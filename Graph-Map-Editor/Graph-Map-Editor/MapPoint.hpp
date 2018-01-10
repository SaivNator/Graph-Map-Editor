//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MapPoint_HEADER
#define MapPoint_HEADER

#include <vector>

#include "MapTriangle.hpp"

struct MapPoint
{
	float x, y;
	std::vector<MapTriangle*> triangles
};

#endif // !MapPoint_HEADER


//end