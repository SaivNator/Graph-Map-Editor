//Author: Sivert Andresen Cubedo

#include "EditorTriangle.hpp"

MapPoint* EditorTriangle::getOddPoint(MapPoint* p1, MapPoint* p2) {
	EditorTriangle::iterator it_1 = std::find(this->begin(), this->end(), p1);
	EditorTriangle::iterator it_2 = std::find(this->begin(), this->end(), p2);
	if (it_1 + 1 != this->end() && it_1 + 1 != it_2) {
		return (*it_1 + 1);
	}
	if (it_2 + 1 != this->end() && it_2 + 1 != it_1) {
		return (*it_2 + 1);
	}
	return this->front();
}
