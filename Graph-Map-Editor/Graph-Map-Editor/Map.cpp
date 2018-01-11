//Author: Sivert Andresen Cubedo

#include "Map.hpp"

Map::Map(wykobi::vector2d<float> & chunk_size, wykobi::vector2d<int> & map_size) :
	m_chunk_size(chunk_size),
	m_map_size(map_size)
{
	//init m_chunks
	m_chunks.reserve(m_map_size.x);
	for (int x = 0; x < m_map_size.x; ++x) {
		m_chunks.push_back(std::vector<std::unique_ptr<MapChunk>>());
		m_chunks[x].reserve(map_size.y);
		for (int y = 0; y < m_map_size.y; ++y) {
			m_chunks[x].push_back(std::unique_ptr<MapChunk>(new MapChunk(wykobi::make_point(x, y) , m_chunk_size)));
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

MapChunk & Map::getChunk(wykobi::point2d<int> & pos) {
	return *m_chunks[pos.x][pos.y];
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
	for (std::size_t x = x0; x < x1; ++x) {
		for (std::size_t y = y0; y < y1; ++y) {
			out_vec.push_back(m_chunks[x][y].get());
		}
	}
	return out_vec;
}

std::vector<std::unique_ptr<MapGroundType>> & Map::getTypes() {
	return m_types;
}

wykobi::vector2d<float> Map::getChunkSize() {
	return m_chunk_size;
}

wykobi::vector2d<int> Map::getMapSize() {
	return m_map_size;
}

//end