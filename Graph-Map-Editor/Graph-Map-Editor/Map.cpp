//Author: Sivert Andresen Cubedo

#include "Map.hpp"

Map::Map(wykobi::vector2d<float> chunk_size, wykobi::vector2d<int> map_size) :
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


//end