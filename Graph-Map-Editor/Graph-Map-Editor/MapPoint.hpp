//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MapPoint_HEADER
#define MapPoint_HEADER

#include <vector>

#include "MapTriangle.hpp"

class MapPoint {
	const wykobi::point2d<float> m_pos;
	std::vector<MapTriangle*> m_triangles;
public:
	/*
	Constructor
	*/
	MapPoint(wykobi::point2d<float> pos);

	/*
	Get pos
	*/
	wykobi::point2d<float> getPos();

	/*
	Get triangles
	*/
	std::vector<MapTriangle*> & getTriangles();

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