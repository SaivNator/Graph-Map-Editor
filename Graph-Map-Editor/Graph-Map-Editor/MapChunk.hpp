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
	const wykobi::vector2d<float> m_offset;
	std::vector<std::unique_ptr<MapPoint>> m_points;		//points inside chunk
	std::vector<std::shared_ptr<MapPoint>> m_shared_points;	//points shared by other chunks
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
	Get chunk size
	*/
	wykobi::vector2d<float> getSize();

	/*
	Get offset
	*/
	wykobi::vector2d<float> getOffset();

	/*
	Add point to chunk
	return pointer to point
	*/
	MapPoint* addPoint(MapPoint point);

	/*
	Add shared point to chunk
	return pointer to point
	*/
	MapPoint* addSharedPoint(std::shared_ptr<MapPoint> ptr);

	/*
	Add triangle inside chunk
	return pointer to triangle
	*/
	MapTriangle* addTriangle(MapTriangle triangle);

	/*
	Clear MapTriangles that chunk is owning, also clear m_points and edge_points that are only owned by this chunk
	*/
	void clear();
};

#endif // !MapChunk_HEADER


//end
