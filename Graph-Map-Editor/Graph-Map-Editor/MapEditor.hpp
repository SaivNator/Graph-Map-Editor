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

typedef std::array<MapPoint*, 3> Triangle;
typedef std::vector<MapPoint*> Path;

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
	Path outerChunkPath(MapChunk & chunk);

	/*
	Triangulate path of MapPoints
	*/
	std::vector<Triangle> triangulatePath(Path path);

	/*
	Get path orientation
	*/
	int pathOrientation(Path & path);

	/*
	Check if vertex is convex
	*/
	bool convexVertex(const std::size_t index, const Path & path, const int wykovi_orientation);

	/*
	Check if vertex is ear (for triangulation)
	*/
	bool vertexIsEar(const std::size_t index, const Path & path);

	/*
	Make triangle from path and index
	*/
	Triangle vertexTriangle(const std::size_t index, const Path & path);

	/*
	Check if path and triangle has a shared edge
	Return vector of pairs of iterators that make up edges
	Return empty vector if no shared edge is found
	*/
	std::vector<std::pair<Path::iterator, Path::iterator>> sharedEdge(Path & path, Triangle & triangle);

	/*
	Get MapPoint that is not input
	*/
	MapPoint* getOddPoint(Triangle & triangle, MapPoint* p1, MapPoint* p2);

	/*
	Merge Triangles in chunk by type
	*/
	std::map<MapGroundType, std::vector<Path>> mergeTrianglesInChunk(MapChunk & chunk);
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