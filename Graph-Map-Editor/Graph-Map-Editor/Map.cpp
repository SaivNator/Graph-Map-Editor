//Author: Sivert Andresen Cubedo

#include "Map.hpp"

Map::Map(wykobi::vector2d<float> & chunk_size, wykobi::vector2d<int> & map_size, MapGroundType & number_of_types) :
	m_chunk_size(chunk_size),
	m_map_size(map_size),
	m_number_of_types(number_of_types)
{
	//init m_chunks
	m_chunk_corner_points.reserve(m_map_size.x + 1);
	for (int x = 0; x < m_map_size.x + 1; ++x) {
		m_chunk_corner_points.push_back(std::vector<std::unique_ptr<MapPoint>>());
		m_chunk_corner_points[x].reserve(m_map_size.y + 1);
		for (int y = 0; y < m_map_size.y + 1; ++y) {
			wykobi::point2d<float> p = wykobi::make_point(static_cast<float>(x) * chunk_size.x, static_cast<float>(y) * chunk_size.y);
			m_chunk_corner_points[x].push_back(std::unique_ptr<MapPoint>(new MapPoint(p)));
		}
	}

	m_chunks.reserve(m_map_size.x);
	for (int x = 0; x < m_map_size.x; ++x) {
		m_chunks.push_back(std::vector<std::unique_ptr<MapChunk>>());
		m_chunks[x].reserve(map_size.y);
		for (int y = 0; y < m_map_size.y; ++y) {
			std::array<MapPoint*, 4> corner{
				m_chunk_corner_points[x][y].get(),
				m_chunk_corner_points[x + 1][y].get(),
				m_chunk_corner_points[x + 1][y + 1].get(),
				m_chunk_corner_points[x][y + 1].get()
			};
			m_chunks[x].push_back(std::unique_ptr<MapChunk>(new MapChunk(wykobi::make_point(x, y) , m_chunk_size, corner)));
		}
	}
}

bool Map::chunkExist(wykobi::point2d<int> & pos) {
	if (pos.x < 0 || pos.x >= m_map_size.x || pos.y < 0 || pos.y >= m_map_size.y) {
		return false;
	}
	else {
		return true;
	}
}

MapChunk* Map::getChunk(wykobi::point2d<int> & pos) {
	return m_chunks[pos.x][pos.y].get();
}

std::vector<MapChunk*> Map::getChunkInRect(wykobi::rectangle<float> & rect) {
	int x0 = static_cast<int>(std::floor(wykobi::min(rect[0].x, rect[1].x)));
	int y0 = static_cast<int>(std::floor(wykobi::min(rect[0].y, rect[1].y)));
	int x1 = static_cast<int>(std::ceil(wykobi::max(rect[0].x, rect[1].x)));
	int y1 = static_cast<int>(std::ceil(wykobi::max(rect[0].y, rect[1].y)));

	if (x0 < 0) x0 = 0;
	if (y0 < 0) y0 = 0;
	if (x1 > m_map_size.x) x1 = m_map_size.x;
	if (y1 > m_map_size.y) y1 = m_map_size.y;

	std::vector<MapChunk*> out_vec;
	for (int x = x0; x < x1; ++x) {
		for (int y = y0; y < y1; ++y) {
			out_vec.push_back(m_chunks[x][y].get());
		}
	}
	return out_vec;
}

MapGroundType Map::getNumberOfTypes() {
	return m_number_of_types;
}

wykobi::vector2d<float> Map::getChunkSize() {
	return m_chunk_size;
}

wykobi::vector2d<int> Map::getMapSize() {
	return m_map_size;
}

//end