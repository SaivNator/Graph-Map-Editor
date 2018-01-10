//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MapChunk_HEADER
#define MapChunk_HEADER

#include <vector>
#include <memory>

#include "MapPoint.hpp"
#include "MapTriangle.hpp"

class MapChunk {
	const int m_x, m_y;
	const float & m_width, m_height;
	std::vector<std::unique_ptr<MapPoint>> m_points;		//points inside chunk
	std::vector<std::shared_ptr<MapPoint>> m_edge_points;	//points shared by other chunks
	std::vector<std::unique_ptr<MapTriangle>> m_triangles;	//triangles inside chunk
	std::vector<MapChunk*> m_relations;
public:
	/*
	Constructor
	*/
	MapChunk(int x, int y, const float & width, const float & height);
	
	/*
	Append MapTriangles in chunk to MapGroundType
	*/
	void render();

	/*
	Make point and return pointer
	if point is outside chunk return nullptr
	*/
	MapPoint* makePoint(float x, float y);

	/*
	Make edge point and return pointer
	if point is not on chunk edge, or outside chunk return nullptr
	*/
	MapPoint* makeEdgePoint(float x, float y);

	/*
	Make triangle inside chunk
	*/
	void makeTriangle(MapPoint* p0, MapPoint* p1, MapPoint* p2, MapGroundType & type);

	/*
	Clear MapTriangles that chunk is owning, also clear unused points
	*/
	void clear();
};

#endif // !MapChunk_HEADER


//end
