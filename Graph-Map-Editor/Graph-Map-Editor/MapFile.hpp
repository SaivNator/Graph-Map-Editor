//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MapFile_HEADER
#define MapFile_HEADER

#include <memory>

#include "Map.hpp"

namespace MapFile {
	/*
	Load map from file
	return nullptr if load failed
	*/
	std::unique_ptr<Map> loadMap(std::string & path);

	/*
	Save map to file
	return false if save failed
	*/
	bool saveMap(std::string & path, Map & map);
}

#endif // !MapFile_HEADER

//end