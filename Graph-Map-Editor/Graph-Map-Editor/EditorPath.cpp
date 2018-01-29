//Author: Sivert Andresen Cubedo

#include "EditorPath.hpp"

std::vector<std::pair<EditorPath::iterator, EditorPath::iterator>> EditorPath::sharedEdge(EditorTriangle & triangle) {
	std::vector<std::pair<EditorPath::iterator, EditorPath::iterator>> out_vec;
	for (EditorTriangle::iterator tri_it_1 = triangle.begin(); tri_it_1 != triangle.end(); ++tri_it_1) {
		EditorPath::iterator path_it_1 = std::find(this->begin(), this->end(), (*tri_it_1));
		if (path_it_1 != this->end()) {
			EditorPath::iterator path_it_2 = (path_it_1 != this->end() - 1) ? path_it_1 + 1 : this->begin();
			EditorTriangle::iterator tri_it_2 = std::find(triangle.begin(), triangle.end(), (*path_it_2));
			if (tri_it_2 != triangle.end()) {
				out_vec.push_back(std::pair<EditorPath::iterator, EditorPath::iterator>(path_it_1, path_it_2));
			}
		}
	}
	return out_vec;
}

std::vector<EditorTriangle> EditorPath::triangulate() {
	std::vector<EditorTriangle> out_vec;
	if (this->size() < 3) {
		return out_vec;
	}
	else if (this->size() == 3) {
		out_vec.push_back(EditorTriangle({this->operator[](0), this->operator[](1), this->operator[](2)}));
		return out_vec;
	}
	if (this->orientation() != wykobi::Clockwise) {
		this->reverse();
	}
	while (this->size() > 3) {
		for (std::size_t i = 0; i < this->size(); ++i) {
			if (this->convexVertex(i, wykobi::Clockwise) && this->vertexIsEar(i)) {
				out_vec.push_back(this->vertexTriangle(i));
				this->erase(this->begin() + i);
				break;
			}
		}
	}
	out_vec.push_back(this->vertexTriangle(1));
	return out_vec;
}

EditorTriangle EditorPath::vertexTriangle(const std::size_t index) {
	MapPoint* prev;
	MapPoint* current = this->operator[](index);
	MapPoint* next;
	if (index == 0) {
		prev = this->back();
		next = this->operator[](index + 1);
	}
	else if (index == this->size() - 1) {
		prev = this->operator[](index - 1);
		next = this->front();
	}
	else {
		prev = this->operator[](index - 1);
		next = this->operator[](index + 1);
	}
	return EditorTriangle({ prev, current, next });
}

bool EditorPath::vertexIsEar(const std::size_t index) {
	std::size_t prev;
	std::size_t next;
	if (index == 0) {
		prev = this->size() - 1;
		next = index + 1;
	}
	else if (index == this->size() - 1) {
		prev = index - 1;
		next = 0;
	}
	else {
		prev = index - 1;
		next = index + 1;
	}
	wykobi::triangle<float, 2> triangle = wykobi::make_triangle(this->operator[](prev)->getPos(), this->operator[](index)->getPos(), this->operator[](next)->getPos());
	if (wykobi::robust_collinear(triangle[0], triangle[1], triangle[2])) {
		return false;
	}
	for (std::size_t i = 0; i < this->size(); ++i) {
		if ((i != prev) && (i != next) && (i != index)) {
			if (point_in_triangle(this->operator[](i)->getPos(), triangle)) {
				return false;
			}
		}
	}
	return true;
}

bool EditorPath::convexVertex(const std::size_t index, const int wykobi_orientation) {
	wykobi::point2d<float> prev;
	wykobi::point2d<float> current = this->operator[](index)->getPos();
	wykobi::point2d<float> next;
	if (index == 0) {
		prev = this->back()->getPos();
		next = this->operator[](index + 1)->getPos();
	}
	else if (index == this->size() - 1) {
		prev = this->operator[](index - 1)->getPos();
		next = this->front()->getPos();
	}
	else {
		prev = this->operator[](index - 1)->getPos();
		next = this->operator[](index + 1)->getPos();
	}
	return (wykobi::orientation(prev, current, next) == wykobi_orientation);
}

int EditorPath::orientation() {
	wykobi::polygon<float, 2> poly;
	poly.reserve(this->size());
	for (MapPoint* p : *this) {
		poly.push_back(p->getPos());
	}
	return wykobi::polygon_orientation(poly);
}

void EditorPath::reverse() {
	std::reverse(this->begin(), this->end());
}

std::vector<EditorPath>& EditorPath::getHulls() {
	return m_hulls;
}

std::string EditorPath::toString() {
	std::ostringstream s;
	for (auto it = begin(); it != end(); ++it) {
		s << "(" << (*it)->getPos().x << "," << (*it)->getPos().y << ")";
	}
	return s.str();
}

std::string EditorPath::toPointerString() {
	std::ostringstream s;
	for (auto it = begin(); it != end(); ++it) {
		s << "[" << (*it) << "]" << "(" << (*it)->getPos().x << "," << (*it)->getPos().y << ")";
		if (it != end() - 1) s << " ";
	}
	return s.str();
}

