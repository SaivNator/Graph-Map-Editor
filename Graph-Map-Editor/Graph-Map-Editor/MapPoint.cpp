//Author:: Sivert Andresen Cubedo

#include "MapPoint.hpp"

MapPoint::MapPoint(wykobi::point2d<float> & pos) :
	m_pos(pos)
{
	m_triangles.reserve(3);
}

wykobi::point2d<float> MapPoint::getPos() {
	return m_pos;
}

std::vector<MapTriangle*> & MapPoint::getTriangles() {
	return m_triangles;
}

void MapPoint::addTriangle(MapTriangle* triangle) {
	if (std::find(m_triangles.begin(), m_triangles.end(), triangle) == m_triangles.end()) {
		m_triangles.push_back(triangle);
	}
}

void MapPoint::removeTriangle(MapTriangle* triangle) {
	auto it = std::find(m_triangles.begin(), m_triangles.end(), triangle);
	if (it != m_triangles.end()) {
		m_triangles.erase(it);
	}
}


//end