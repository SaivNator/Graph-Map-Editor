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

std::vector<EditorPath> EditorPath::removeHull() {
	//EXPEREMENTAL
	if (m_hulls.empty()) {
		return std::vector<EditorPath>({ *this });
	}

	if (orientation() != wykobi::Clockwise) {
		this->reverse();
	}
	
	RemoveHull::Graph graph(*this);
#if 0
	std::vector<Edge*> hull_bridge_vec;
	//make graph of path and hulls
	//keep track of what objects nodes belong to
	//for each hull, find two edges, edge must either bridge to other object or outer path
	//avoid nodes having more then 3 edges
	//bridges are undirected, so one bridge counts as +1 bridge for both objects
	//for each bridge traverse both directions, mark bridges as visited
	for (std::vector<Node*> & hull : graph.m_hull_vec) {
		std::vector<Edge*> current_bridge_vec;
		std::size_t bridge_count = 0;
		for (Node* n : hull) {
			if (n->m_edges.size() > 2) {
				++bridge_count;
			}
		}
		for (auto hull_it = hull.begin(); hull_it != hull.end() && bridge_count < 2; ++hull_it) {
			for (auto & node_ptr : graph.m_node_vec) {
				if (/*node_ptr->m_edges.size() < 3 && */std::find(hull.begin(), hull.end(), node_ptr.get()) == hull.end() && graph.isEdgeLegal((*hull_it), node_ptr.get())) {
					graph.addEdge((*hull_it), node_ptr.get());
					current_bridge_vec.push_back(graph.m_edge_vec.back().get());
					++bridge_count;
					break;
				}
			}
		}
		assert(bridge_count >= 2);
		if (!current_bridge_vec.empty()) {
			hull_bridge_vec.insert(hull_bridge_vec.end(), current_bridge_vec.begin(), current_bridge_vec.end());
		}
	}
	std::cout << "hull_bridge_vec.size() = " << hull_bridge_vec.size() << "\n";
	std::vector<EditorPath> out_vec;
	for (auto it = hull_bridge_vec.begin(); it != hull_bridge_vec.end(); ++it) {
		if (!(*it)->clockVisit((*it)->m_a)) {
			std::cout << (*it) << "\tclockwise" << "\n";
			out_vec.push_back(graph.makeEditorPath(graph.traverseBridgeClockwise((*it), (*it)->m_a)));
		}
		if (!(*it)->counterClockVisit((*it)->m_a)) {
			std::cout << (*it) << "\tcounterclockwise" << "\n";
			out_vec.push_back(graph.makeEditorPath(graph.traverseBridgeCounterClockwise((*it), (*it)->m_a)));
		}
	}
	return out_vec;
#else
	std::list<RemoveHull::Hull*> not_connected_queue;
	for (RemoveHull::Hull & hull : graph.m_hull_vec) {
		not_connected_queue.push_back(&hull);
	}
	if (not_connected_queue.size() > 1) {
		not_connected_queue.sort(
			[](RemoveHull::Hull* h1, RemoveHull::Hull* h2)
			{ return h1->centroid().x < h2->centroid().x; }
		);
	}
	while (!not_connected_queue.empty()) {
		RemoveHull::Hull* current_hull = not_connected_queue.front();
		not_connected_queue.pop_front();
		if (graph.connectHull(*current_hull)) {
			//if this then current_hull is connected
			graph.m_connected_hulls.push_back(current_hull);
		}
		else {
			//if this then place hull back in queue
			not_connected_queue.push_back(current_hull);
		}
		//std::cout << current_hull->m_connect_count << "\n";
	}
	//std::cout << "hull_bridge_vec.size() = " << graph.m_hull_bridge_vec.size() << "\n";
	std::vector<EditorPath> out_vec;
	for (auto it = graph.m_hull_bridge_vec.begin(); it != graph.m_hull_bridge_vec.end(); ++it) {
		if (!(*it)->clockVisit((*it)->m_a)) {
			//std::cout << (*it) << "\tclockwise" << "\n";
			out_vec.push_back(graph.makeEditorPath(graph.traverseBridgeClockwise((*it), (*it)->m_a)));
		}
		if (!(*it)->counterClockVisit((*it)->m_a)) {
			//std::cout << (*it) << "\tcounterclockwise" << "\n";
			out_vec.push_back(graph.makeEditorPath(graph.traverseBridgeCounterClockwise((*it), (*it)->m_a)));
		}
	}
	return out_vec;
#endif
}

std::string EditorPath::toString() {
	std::ostringstream s;
	for (auto it = begin(); it != end(); ++it) {
		s << "(" << (*it)->getPos().x << "," << (*it)->getPos().y << ")";
	}
	return s.str();
}

