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


struct Node;
struct Edge;
struct Graph;

struct Edge {
	Node* m_a;
	Node* m_b;

	bool m_visited_a = false;
	bool m_visited_b = false;

	Edge(Node* a, Node* b) :
		m_a(a),
		m_b(b)
	{
	}
	void visit(Node* n) {
		if (n == m_a) m_visited_a = true;
		else m_visited_b = true;
	}
	Node* getNode(Node* n) {
		return (n == m_a) ? m_b : m_a;
	}
	bool getVisited(Node* n) {
		return (n == m_a) ? m_visited_a : m_visited_b;
	}
};

struct Node {
	MapPoint* m_point;
	std::vector<std::shared_ptr<Edge>> m_edges;

	wykobi::point2d<float> getPos() {
		return m_point->getPos();
	}

	Edge* getClockwiseMost(Edge* prev_edge) {
		Node* prev_node = prev_edge->getNode(this);
		std::vector<Edge*> edges;
		edges.reserve(m_edges.size());
		for (auto & ptr : m_edges) {
			edges.push_back(ptr.get());
		}
		std::vector<Edge*>::iterator it = std::find(edges.begin(), edges.end(), prev_edge);
		if (it != edges.end()) edges.erase(it);
		if (edges.empty()) return nullptr;
		it = edges.begin();
		wykobi::vector2d<float> current_dir = this->getPos() - prev_node->getPos();
		Node* next_node = (*it)->getNode(this);
		Edge* next_edge = (*it);
		++it;
		wykobi::vector2d<float> next_dir = next_node->getPos() - this->getPos();
		bool is_current_convex = (wykobi::dot_product(next_dir, wykobi::perpendicular(current_dir)) <= 0);
		while (it != edges.end()) {
			Node* test_node = (*it)->getNode(this);
			Edge* test_edge = (*it);
			wykobi::vector2d<float> test_dir = test_node->getPos() - this->getPos();
			if (is_current_convex) {
				if (wykobi::dot_product(current_dir, wykobi::perpendicular(test_dir)) < 0 || wykobi::dot_product(next_dir, wykobi::perpendicular(test_dir)) < 0) {
					next_dir = test_dir;
					next_node = test_node;
					next_edge = test_edge;
					is_current_convex = (wykobi::dot_product(next_dir, wykobi::perpendicular(current_dir)) <= 0);
				}
			}
			else {
				if (wykobi::dot_product(current_dir, wykobi::perpendicular(test_dir)) < 0 && wykobi::dot_product(next_dir, wykobi::perpendicular(test_dir)) < 0) {
					next_dir = test_dir;
					next_node = test_node;
					next_edge = test_edge;
					is_current_convex = (wykobi::dot_product(next_dir, wykobi::perpendicular(current_dir)) <= 0);
				}
			}
			++it;
		}
		return next_edge;
	}
	Edge* getCounterClockwiseMost(Edge* prev_edge) {
		Node* prev_node = prev_edge->getNode(this);
		std::vector<Edge*> edges;
		edges.reserve(m_edges.size());
		for (auto & ptr : m_edges) {
			edges.push_back(ptr.get());
		}
		std::vector<Edge*>::iterator it = std::find(edges.begin(), edges.end(), prev_edge);
		if (it != edges.end()) edges.erase(it);
		if (edges.empty()) return nullptr;
		it = edges.begin();
		wykobi::vector2d<float> current_dir = this->getPos() - prev_node->getPos();
		Node* next_node = (*it)->getNode(this);
		Edge* next_edge = (*it);
		++it;
		wykobi::vector2d<float> next_dir = next_node->getPos() - this->getPos();
		bool is_current_convex = (wykobi::dot_product(next_dir, wykobi::perpendicular(current_dir)) <= 0);
		while (it != edges.end()) {
			Node* test_node = (*it)->getNode(this);
			Edge* test_edge = (*it);
			wykobi::vector2d<float> test_dir = test_node->getPos() - this->getPos();
			if (is_current_convex) {
				if (wykobi::dot_product(current_dir, wykobi::perpendicular(test_dir)) > 0 && wykobi::dot_product(next_dir, wykobi::perpendicular(test_dir)) > 0) {
					next_dir = test_dir;
					next_node = test_node;
					next_edge = test_edge;
					is_current_convex = (wykobi::dot_product(next_dir, wykobi::perpendicular(current_dir)) <= 0);
				}
			}
			else {
				if (wykobi::dot_product(current_dir, wykobi::perpendicular(test_dir)) > 0 || wykobi::dot_product(next_dir, wykobi::perpendicular(test_dir)) > 0) {
					next_dir = test_dir;
					next_node = test_node;
					next_edge = test_edge;
					is_current_convex = (wykobi::dot_product(next_dir, wykobi::perpendicular(current_dir)) <= 0);
				}
			}
			++it;
		}
		return next_edge;
	}
};

struct Graph {

};

std::vector<EditorPath> EditorPath::removeHull() {
	//EXPEREMENTAL
	if (m_hulls.empty()) {
		return std::vector<EditorPath>({ *this });
	}

	if (orientation() != wykobi::Clockwise) {
		reverse();
	}
	
	//make graph of path and hulls
	//keep track of what objects nodes belong to
	//for each hull, find two edges, edge must either bridge to other object or outer path
	//bridges are undirected, so one bridge counts as +1 bridge for both objects
	//for each bridge traverse both directions, mark bridges as visited

	


}
