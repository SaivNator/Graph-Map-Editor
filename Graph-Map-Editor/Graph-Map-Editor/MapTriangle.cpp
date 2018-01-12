//Author: Sivert Andresen Cubedo

#include "MapTriangle.hpp"

MapTriangle::MapTriangle(std::array<MapPoint*, 3> points, MapChunk & chunk, MapGroundType & type) :
	m_points(points),
	m_chunk(chunk),
	m_type(type)
{
}

MapPoint & MapTriangle::operator[](std::size_t i) {
	return *m_points[i];
}

std::size_t MapTriangle::size() {
	return m_points.size();
}

MapGroundType MapTriangle::getType() {
	return m_type;
}

MapChunk & MapTriangle::getChunk() {
	return m_chunk;
}

std::vector<MapTriangle*> & MapTriangle::getRelations() {
	return m_relations;
}

void MapTriangle::addRelation(MapTriangle* triangle) {
	if (std::find(m_relations.begin(), m_relations.end(), triangle) == m_relations.end()) {
		m_relations.push_back(triangle);
	}
}

void MapTriangle::removeRelation(MapTriangle* triangle) {
	auto it = std::find(m_relations.begin(), m_relations.end(), triangle);
	if (it != m_relations.end()) {
		m_relations.erase(it);
	}
}

//end