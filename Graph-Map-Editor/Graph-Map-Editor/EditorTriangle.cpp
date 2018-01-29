//Author: Sivert Andresen Cubedo

#include "EditorTriangle.hpp"

EditorTriangle::EditorTriangle(const std::array<MapPoint*, 3> points) :
	std::array<MapPoint*, 3>(points)
{
}

MapPoint* EditorTriangle::getOddPoint(MapPoint* p1, MapPoint* p2) {
	EditorTriangle::iterator it_1 = std::find(this->begin(), this->end(), p1);
	EditorTriangle::iterator it_2 = std::find(this->begin(), this->end(), p2);
	//points must be in triangle
	assert(it_1 != this->end() && it_2 != this->end());
	if (it_1 + 1 != this->end() && it_1 + 1 != it_2) {
		return (*it_1 + 1);
	}
	if (it_2 + 1 != this->end() && it_2 + 1 != it_1) {
		return (*it_2 + 1);
	}
	return this->front();
}

int EditorTriangle::orientation() {
	wykobi::polygon<float, 2> poly;
	poly.reserve(this->size());
	for (MapPoint* p : *this) {
		poly.push_back(p->getPos());
	}
	return wykobi::polygon_orientation(poly);
}

void EditorTriangle::reverse() {
	std::reverse(this->begin(), this->end());
}

std::string EditorTriangle::toString() {
	std::ostringstream s;
	for (auto it = begin(); it != end(); ++it) {
		s << "(" << (*it)->getPos().x << "," << (*it)->getPos().y << ")";
	}
	return s.str();
}

std::string EditorTriangle::toPointerString() {
	std::ostringstream s;
	for (auto it = begin(); it != end(); ++it) {
		s << "[" << (*it) << "]" << "(" << (*it)->getPos().x << "," << (*it)->getPos().y << ")";
		if (it != end() - 1) s << " ";
	}
	return s.str();
}
