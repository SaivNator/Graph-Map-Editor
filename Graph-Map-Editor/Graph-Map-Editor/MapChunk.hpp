//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MapChunk_HEADER
#define MapChunk_HEADER

#include <array>
#include <vector>
#include <memory>

#include <wykobi.hpp>

#include "MapPoint.hpp"
#include "MapTriangle.hpp"
#include "MapGroundType.hpp"

class MapPoint;
class MapTriangle;

class MapChunk {
	const wykobi::point2d<int> m_pos;
	const wykobi::vector2d<float> & m_size;
	const wykobi::vector2d<float> m_offset;
	const wykobi::rectangle<float> m_rect;
	const std::array<MapPoint*, 4> m_corner_points;				//corner points
	std::vector<std::shared_ptr<MapPoint>> m_shared_points;		//points shared by other chunks
	std::vector<std::unique_ptr<MapPoint>> m_internal_points;	//points inside chunk
	std::vector<std::unique_ptr<MapTriangle>> m_triangles;		//triangles inside chunk
	std::vector<MapChunk*> m_relations;
public:
	/*
	Constructor
	*/
	MapChunk(const wykobi::point2d<int> & pos, const wykobi::vector2d<float> & size, const std::array<MapPoint*, 4> & corner_points);

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
	Get boundingrectangle of chunk
	*/
	wykobi::rectangle<float> getRect();

	/*
	Get corner points
	*/
	const std::array<MapPoint*, 4> & getCornerPoints();

	/*
	Get shared points
	*/
	std::vector<std::shared_ptr<MapPoint>> & getSharedPoints();

	/*
	Get internal points
	*/
	std::vector<std::unique_ptr<MapPoint>> & getInternalPoints();

	/*
	Get triangles
	*/
	std::vector<std::unique_ptr<MapTriangle>> & getTriangles();

	/*
	Get relations
	*/
	std::vector<MapChunk*> & getRelations();

	/*
	Add internal point to chunk
	return pointer to point
	*/
	MapPoint* addInternalPoint(MapPoint point);

	/*
	Add shared point to chunk
	return pointer to point
	*/
	MapPoint* addSharedPoint(std::shared_ptr<MapPoint> ptr);

	/*
	Construct triangle inside chunk
	return pointer to triangle
	*/
	MapTriangle* addTriangle(std::array<MapPoint*, 3> & points, MapGroundType type);
};

#endif // !MapChunk_HEADER


//end
