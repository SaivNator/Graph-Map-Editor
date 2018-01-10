//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MapTriangle_HEADER
#define MapTriangle_HEADER

#include <array>
#include <vector>
#include <memory>

#include "MapPoint.hpp"
#include "MapGroundType.hpp"

class MapTriangle {
	const std::array<MapPoint*, 3> m_points;
	MapGroundType & m_mapGroundType;
	std::vector<MapTriangle*> m_relations;
public:
	/*
	Constructor
	*/
	MapTriangle(std::array<MapPoint*, 3> points, MapGroundType & type);

	/*
	[] operator
	*/
	std::weak_ptr<MapPoint> operator[](std::size_t i);

	/*
	Get size
	*/
	std::size_t size();
	
	/*
	Append verticies to MapGroundType
	*/
	void render();

	/*
	Get type
	*/
	MapGroundType & getType();
};

#endif // !MapTriangle_HEADER


//end