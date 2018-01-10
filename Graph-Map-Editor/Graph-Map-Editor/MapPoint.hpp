//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MapPoint_HEADER
#define MapPoint_HEADER

#include <vector>

#include "MapTriangle.hpp"

class MapPoint
{
	std::vector<MapTriangle*> m_triangles;
public:
	float x, y;
	
	/*
	Constructor
	*/
	MapPoint(float x, float y);

	/*
	Add triangle to point
	*/
	void addTriangle(MapTriangle* triangle);

	/*
	Remove triangle from point
	*/
	void removeTriangle(MapTriangle* triangle);
};

#endif // !MapPoint_HEADER


//end