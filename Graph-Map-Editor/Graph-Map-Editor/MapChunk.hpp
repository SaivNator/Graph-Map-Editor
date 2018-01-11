//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MapChunk_HEADER
#define MapChunk_HEADER

#include <vector>
#include <memory>

#include <wykobi.hpp>

#include "MapPoint.hpp"
#include "MapTriangle.hpp"

class MapChunk {
	const wykobi::point2d<int> m_pos;
	const wykobi::vector2d<float> & m_size;
	std::vector<std::unique_ptr<MapPoint>> m_points;		//points inside chunk
	std::vector<std::shared_ptr<MapPoint>> m_edge_points;	//points shared by other chunks
	std::vector<std::unique_ptr<MapTriangle>> m_triangles;	//triangles inside chunk
	std::vector<MapChunk*> m_relations;
public:
	/*
	Constructor
	*/
	MapChunk(const wykobi::point2d<int> pos, const wykobi::vector2d<float> & size);
	
	/*
	Append MapTriangles in chunk to MapGroundType
	*/
	void render();

	/*
	Get chunk pos
	*/
	wykobi::point2d<int> getPos();

	/*
	Make point and return pointer
	if point is outside chunk return nullptr
	*/
	MapPoint* makePoint(wykobi::point2d<float> pos);

	/*
	Make edge point and return pointer
	if point is not on chunk edge, or outside chunk return nullptr
	*/
	MapPoint* makeEdgePoint(wykobi::point2d<float> pos);

	/*
	Make triangle inside chunk
	*/
	void makeTriangle(MapPoint* p0, MapPoint* p1, MapPoint* p2, MapGroundType & type);

	/*
	Clear MapTriangles that chunk is owning, also clear m_points and edge_points that are only owned by this chunk
	*/
	void clear();
};

#endif // !MapChunk_HEADER


//end
