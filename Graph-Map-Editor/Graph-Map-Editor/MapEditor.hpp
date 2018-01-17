//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MapEditor_HEADER
#define MapEditor_HEADER

#include <iostream>
#include <array>
#include <vector>
#include <list>
#include <set>
#include <map>

#include "MapChunk.hpp"
#include "Map.hpp"
#include "MapGroundType.hpp"

#include "EditorTriangle.hpp"
#include "EditorPath.hpp"

class MapEditor {
	bool m_map_is_loaded = false;
	std::unique_ptr<Map> m_map;

	/*
	Apply chunk relations
	*/
	void applyChunkRelation(Map & map);

	/*
	Fill chunk with single type
	*/
	void fillChunk(MapChunk & chunk, MapGroundType type);

	/*
	Fill map with single type
	*/
	void fillMap(Map & map, MapGroundType type);

	/*
	Clear MapTriangles and internal points
	*/
	void clearChunk(MapChunk & chunk);

	/*
	Clear all shared points from map
	*/
	void clearSharedPoints(Map & map);

	/*
	Make outer chunk path
	*/
	EditorPath outerChunkPath(MapChunk & chunk);

	/*
	Merge Triangles in chunk by type
	*/
	std::map<MapGroundType, std::vector<EditorPath>> mergeTrianglesInChunk(MapChunk & chunk);
public:
	/*
	Constructor
	*/
	MapEditor();

	/*
	Create new map
	*/
	void createMap(wykobi::vector2d<float> chunk_size, wykobi::vector2d<int> map_size, MapGroundType default_type);

	/*
	Check if mapeditor has map loaded
	*/
	bool empty();

	/*
	Free map
	*/
	void clear();

	/*
	Get pointer to map
	*/
	Map* getMap();

	/*
	Insert polygon shape of a type onto map
	*/
	void insertPolygon(wykobi::polygon<float, 2> & polygon, MapGroundType type);
};

#endif // !MapEditor_HEADER


//end