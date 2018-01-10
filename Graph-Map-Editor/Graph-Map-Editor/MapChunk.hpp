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
	std::vector<std::shared_ptr<MapPoint>> m_points;
	std::vector<std::unique_ptr<MapTriangle>> m_triangles;
public:
	MapChunk(int x, int y, const float & width, const float & height);

	void addTriangle();
};

#endif // !MapChunk_HEADER


//end
