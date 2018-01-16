//Author: Sivert Andresen Cubedo

#include "MapTriangle.hpp"

MapTriangle::MapTriangle(const std::array<MapPoint*, 3> & points, MapChunk & chunk, MapGroundType & type) :
	m_points(points),
	m_chunk(chunk),
	m_type(type),
	m_centroid(wykobi::centroid(m_points[0]->getPos(), m_points[1]->getPos(), m_points[2]->getPos()))
{
	//tell points that this triangle exist
	for (MapPoint* p : m_points) {
		p->addTriangle(this);
	}
	//find relations
	findRelations();
}

void MapTriangle::release() {
	for (MapPoint* p : m_points) {
		p->removeTriangle(this);
	}
	for (MapTriangle* tri : m_relations) {
		removeRelation(tri);
	}
}

wykobi::point2d<float> MapTriangle::operator[](std::size_t i) {
	return m_points[i]->getPos();
}

const std::array<MapPoint*, 3> & MapTriangle::getPoints() {
	return m_points;
}

wykobi::point2d<float> MapTriangle::getCentroid() {
	return m_centroid;
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
	if (std::find(triangle->getRelations().begin(), triangle->getRelations().end(), this) == triangle->getRelations().end()) {
		triangle->getRelations().push_back(this);
	}
}

void MapTriangle::removeRelation(MapTriangle* triangle) {
	auto it = std::find(m_relations.begin(), m_relations.end(), triangle);
	if (it != m_relations.end()) {
		m_relations.erase(it);
	}
	it = std::find(triangle->getRelations().begin(), triangle->getRelations().end(), this);
	if (it != triangle->getRelations().end()) {
		triangle->getRelations().erase(it);
	}
}

void MapTriangle::findRelations() {
	for (MapPoint* origin_point : m_points) {
		for (MapTriangle* tri : origin_point->getTriangles()) {
			std::size_t count = 0;
			for (auto it = tri->getPoints().begin(); it != tri->getPoints().end(); ++it) {
				for (MapPoint* test_point : m_points) {
					if ((*it) == test_point) {
						++count;
					}
				}
			}
			if (count == 2) {
				addRelation(tri);
			}
		}
	}
}

//end