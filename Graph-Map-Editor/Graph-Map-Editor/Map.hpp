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
	std::vector<std::unique_ptr<MapGroundType>> m_types;
	
public:
	/*
	Constructor
	*/
	Map(wykobi::vector2d<float> chunk_size, wykobi::vector2d<int> map_size);

	/*
	Get chunk at pos
		if pos is outside map then return nullptr
	*/
	MapChunk* getChunk(wykobi::point2d<int> pos);

	/*
	Get types
	*/
	std::vector<std::unique_ptr<MapGroundType>> & getTypes();

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
