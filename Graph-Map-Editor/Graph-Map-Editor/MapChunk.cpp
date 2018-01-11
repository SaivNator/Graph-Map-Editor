//Author: Sivert Andresen Cubedo

#include "MapChunk.hpp"

MapChunk::MapChunk(const wykobi::point2d<int> & pos, const wykobi::vector2d<float> & size) :
	m_pos(pos),
	m_size(size),
	m_offset(wykobi::make_vector(static_cast<float>(pos.x) * size.x, static_cast<float>(pos.y) * size.y))
{
}

void MapChunk::render() {
	for (auto & ptr : m_triangles) {
		ptr->render();
	}
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

std::vector<MapChunk*> & MapChunk::getRelations() {
	return m_relations;
}

MapPoint* MapChunk::addPoint(MapPoint point) {
	m_points.push_back(std::unique_ptr<MapPoint>(new MapPoint(point)));
	return m_points.back().get();
}

MapPoint* MapChunk::addSharedPoint(std::shared_ptr<MapPoint> ptr) {
	m_shared_points.push_back(ptr);
	return m_shared_points.back().get();
}

MapTriangle* MapChunk::addTriangle(MapTriangle triangle) {
	m_triangles.push_back(std::unique_ptr<MapTriangle>(new MapTriangle(triangle)));
	return m_triangles.back().get();
}

void MapChunk::clear() {
	m_triangles.clear();
	m_points.clear();
	auto it = m_shared_points.begin();
	while (it != m_shared_points.end()) {
		if ((*it)->getTriangles().size() < 2) {
			m_shared_points.erase(it);
		}
		else {
			++it;
		}
	}
}

//end