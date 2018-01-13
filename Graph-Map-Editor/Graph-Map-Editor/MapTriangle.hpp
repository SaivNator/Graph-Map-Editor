//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MapTriangle_HEADER
#define MapTriangle_HEADER

#include <array>
#include <vector>
#include <memory>

#include "MapPoint.hpp"
#include "MapChunk.hpp"
#include "MapGroundType.hpp"

class MapPoint;
class MapChunk;

class MapTriangle {
	const std::array<MapPoint*, 3> m_points;
	MapChunk & m_chunk;
	const MapGroundType m_type;
	std::vector<MapTriangle*> m_relations;

	/*
	Find triangle relations
	*/
	void findRelations();
public:
	/*
	Constructor
	*/
	MapTriangle(const std::array<MapPoint*, 3> & points, MapChunk & chunk, MapGroundType & type);

	/*
	Remove this from points,
	Remove two way relations
	*/
	void release();

	/*
	[] operator
	*/
	wykobi::point2d<float> operator[](std::size_t i);

	/*
	Get points
	*/
	const std::array<MapPoint*, 3> & getPoints();

	/*
	Get size
	*/
	std::size_t size();

	/*
	Get type
	*/
	MapGroundType getType();

	/*
	Get chunk
	*/
	MapChunk & getChunk();

	/*
	Get relations
	*/
	std::vector<MapTriangle*> & getRelations();

	/*
	Add two way relation between this and triangle
	*/
	void addRelation(MapTriangle* triangle);

	/*
	Remove two way relation between this and triangle
	*/
	void removeRelation(MapTriangle* triangle);
};

#endif // !MapTriangle_HEADER


//end