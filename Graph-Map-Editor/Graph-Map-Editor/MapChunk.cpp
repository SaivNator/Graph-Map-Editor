//Author: Sivert Andresen Cubedo

#include "MapChunk.hpp"

MapChunk::MapChunk(const wykobi::point2d<int> & pos, const wykobi::vector2d<float> & size, const std::array<MapPoint*, 4> & corner_points) :
	m_pos(pos),
	m_size(size),
	m_offset(wykobi::make_vector(static_cast<float>(pos.x) * size.x, static_cast<float>(pos.y) * size.y)),
	m_rect(wykobi::make_rectangle(m_offset, m_offset + m_size)),
	m_corner_points(corner_points)
{
}

wykobi::point2d<int> MapChunk::getPos() {
	return m_pos;
}

wykobi::vector2d<float> MapChunk::getSize() {
	return m_size;
}

wykobi::vector2d<float> MapChunk::getOffset() {
	return m_offset;
}

wykobi::rectangle<float> MapChunk::getRect() {
	return m_rect;
}

const std::array<MapPoint*, 4> & MapChunk::getCornerPoints() {
	return m_corner_points;
}

std::vector<std::shared_ptr<MapPoint>> & MapChunk::getSharedPoints() {
	return m_shared_points;
}

std::vector<std::unique_ptr<MapPoint>> & MapChunk::getInternalPoints() {
	return m_internal_points;
}

std::vector<std::unique_ptr<MapTriangle>> & MapChunk::getTriangles() {
	return m_triangles;
}

std::vector<MapChunk*> & MapChunk::getRelations() {
	return m_relations;
}

MapPoint* MapChunk::addInternalPoint(MapPoint point) {
	m_internal_points.push_back(std::unique_ptr<MapPoint>(new MapPoint(point)));
	return m_internal_points.back().get();
}

MapPoint* MapChunk::addSharedPoint(std::shared_ptr<MapPoint> ptr) {
	m_shared_points.push_back(ptr);
	return m_shared_points.back().get();
}

MapTriangle* MapChunk::addTriangle(std::array<MapPoint*, 3> & points, MapGroundType type) {
	m_triangles.push_back(std::unique_ptr<MapTriangle>(new MapTriangle(points, *this, type)));
	return m_triangles.back().get();
}

//end