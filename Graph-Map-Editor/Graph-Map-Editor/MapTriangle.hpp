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

class MapTriangle {
	const std::array<MapPoint*, 3> m_points;
	MapChunk & m_chunk;
	MapGroundType & m_type;
	std::vector<MapTriangle*> m_relations;
public:
	/*
	Constructor
	*/
	MapTriangle(std::array<MapPoint*, 3> points, MapChunk & chunk, MapGroundType & type);

	/*
	[] operator
	*/
	MapPoint & operator[](std::size_t i);

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

	/*
	Get chunk
	*/
	MapChunk & getChunk();

	/*
	Get relations
	*/
	std::vector<MapTriangle*> & getRelations();

	/*
	Add relation
	*/
	void addRelation(MapTriangle* triangle);

	/*
	Remove relation
	*/
	void removeRelation(MapTriangle* triangle);
};

#endif // !MapTriangle_HEADER


//end