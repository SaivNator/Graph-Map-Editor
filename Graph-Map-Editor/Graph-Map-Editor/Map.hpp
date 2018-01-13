//Author: Sivert Andresen Cubedo
#pragma once

#ifndef Map_HEADER
#define Map_HEADER

#include <array>
#include <vector>
#include <memory>

#include <wykobi.hpp>

#include "MapChunk.hpp"
#include "MapGroundType.hpp"

class Map {
	const wykobi::vector2d<float> m_chunk_size;				//size of each chunk
	const wykobi::vector2d<int> m_map_size;					//number of chunks in each dimention

	std::vector<std::vector<std::unique_ptr<MapChunk>>> m_chunks;
	std::vector<std::vector<std::unique_ptr<MapPoint>>> m_chunk_corner_points;
public:
	/*
	Constructor
	*/
	Map(wykobi::vector2d<float> & chunk_size, wykobi::vector2d<int> & map_size);
	
	/*
	Check if chunk exist
	*/
	bool chunkExist(wykobi::point2d<int> & pos);

	/*
	Get chunk at pos
	Input must be in bounds
	*/
	MapChunk* getChunk(wykobi::point2d<int> & pos);

	/*
	Get chunks that rect is overlapping
	*/
	std::vector<MapChunk*> getChunkInRect(wykobi::rectangle<float> & rect);

	/*
	Get chunk size
	*/
	wykobi::vector2d<float> getChunkSize();

	/*
	Get map size
	*/
	wykobi::vector2d<int> getMapSize();
};

#endif // !Map_HEADER

//end
