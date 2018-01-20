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



EditorPath::Edge::Edge(Node* a, Node* b) :
	m_a(a),
	m_b(b)
{
	assert(a != b);
}

	
EditorPath::Node* EditorPath::Edge::getNode(Node* n) {
	assert(n == m_a || n == m_b);
	return (n == m_a) ? m_b : m_a;
}
	
bool EditorPath::Edge::clockVisit(Node* n) {
	assert(n == m_a || n == m_b);
	Node* next_node = this->getNode(n);
	Edge* next_edge = next_node->getClockwiseMost(this);
	
	if (!next_edge->m_visited) {
		return false;
	}
	else {
		return true;
	}
}

bool EditorPath::Edge::counterClockVisit(Node* n) {
	assert(n == m_a || n == m_b);
	Node* next_node = this->getNode(n);
	Edge* next_edge = next_node->getCounterClockwiseMost(this);

	if (!next_edge->m_visited) {
		return false;
	}
	else {
		return true;
	}
}




EditorPath::Node::Node(MapPoint* point_ptr) : 
m_point(point_ptr)
{
}

wykobi::point2d<float> EditorPath::Node::getPos() {
	return m_point->getPos();
}

EditorPath::Edge* EditorPath::Node::getClockwiseMost(Edge* prev_edge) {
	Node* prev_node = prev_edge->getNode(this);
	std::vector<Edge*> edges = m_edges;
	std::vector<Edge*>::iterator it = std::find(edges.begin(), edges.end(), prev_edge);
	assert(it != edges.end());
	edges.erase(it);
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

EditorPath::Edge* EditorPath::Node::getCounterClockwiseMost(Edge* prev_edge) {
	Node* prev_node = prev_edge->getNode(this);
	std::vector<Edge*> edges = m_edges;
	std::vector<Edge*>::iterator it = std::find(edges.begin(), edges.end(), prev_edge);
	assert(it != edges.end());
	edges.erase(it);
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



EditorPath::Graph::Graph(EditorPath & path) {
	for (MapPoint* p : path) {
		m_node_vec.push_back(std::unique_ptr<Node>(new Node(p)));
		m_outer_path.push_back(m_node_vec.back().get());
	}
	for (auto it = m_outer_path.begin(); it != m_outer_path.end(); ++it) {
		auto next_it = (it + 1 != m_outer_path.end()) ? it + 1 : m_outer_path.begin();
		addEdge((*it), (*next_it));
	}
	for (EditorPath & hull : path.getHulls()) {
		m_hull_vec.push_back(std::vector<Node*>());
		std::vector<Node*> & temp_vec = m_hull_vec.back();
		for (MapPoint* p : hull) {
			m_node_vec.push_back(std::unique_ptr<Node>(new Node(p)));
			temp_vec.push_back(m_node_vec.back().get());
		}
		for (auto it = temp_vec.begin(); it != temp_vec.end(); ++it) {
			auto next_it = (it + 1 != temp_vec.end()) ? it + 1 : temp_vec.begin();
			addEdge((*it), (*next_it));
		}
	}
}

void EditorPath::Graph::addEdge(Node* a, Node* b) {
	m_edge_vec.push_back(std::unique_ptr<Edge>(new Edge(a, b)));
	a->m_edges.push_back(m_edge_vec.back().get());
	b->m_edges.push_back(m_edge_vec.back().get());
}

bool EditorPath::Graph::isEdgeLegal(Node* a, Node* b) {
	wykobi::segment<float, 2> ab_segment = wykobi::make_segment(a->getPos(), b->getPos());
	std::vector<Edge*> exclude_edges;
	exclude_edges.insert(exclude_edges.end(), a->m_edges.begin(), a->m_edges.end());
	exclude_edges.insert(exclude_edges.end(), b->m_edges.begin(), b->m_edges.end());
	for (auto & edge_ptr : m_edge_vec) {
		if (std::find(exclude_edges.begin(), exclude_edges.end(), edge_ptr.get()) == exclude_edges.end()) {
			wykobi::segment<float, 2> test_segment = wykobi::make_segment(edge_ptr->m_a->getPos(), edge_ptr->m_b->getPos());
			if (wykobi::intersect(ab_segment, test_segment)) {
				return false;
			}
		}
	}
	return true;
}

std::vector<EditorPath::Node*> EditorPath::Graph::traverseBridgeClockwise(Edge* edge, Node* start_node) {
	assert(start_node == edge->m_a || start_node == edge->m_b);
	std::vector<Node*> out_vec;
	out_vec.push_back(start_node);
	Edge* current_edge = edge;
		
	current_edge->m_visited = true;
		
	Node* current_node = edge->getNode(start_node);
	while (current_node != start_node) {
		out_vec.push_back(current_node);
			
		current_edge->m_visited = true;
			
		current_edge = current_node->getClockwiseMost(current_edge);
		current_node = current_edge->getNode(current_node);
	}
	return out_vec;
}
std::vector<EditorPath::Node*> EditorPath::Graph::traverseBridgeCounterClockwise(Edge* edge, Node* start_node) {
	assert(start_node == edge->m_a || start_node == edge->m_b);
	std::vector<Node*> out_vec;
	out_vec.push_back(start_node);
	Edge* current_edge = edge;

	current_edge->m_visited = true;

	Node* current_node = edge->getNode(start_node);
	while (current_node != start_node) {
		out_vec.push_back(current_node);
			
		current_edge->m_visited = true;
			
		current_edge = current_node->getCounterClockwiseMost(current_edge);
		current_node = current_edge->getNode(current_node);
	}
	return out_vec;
}


EditorPath EditorPath::Graph::makeEditorPath(std::vector<Node*> node_vec) {
	EditorPath path;
	for (Node* node : node_vec) {
		path.push_back(node->m_point);
	}
	return path;
}




std::vector<EditorPath> EditorPath::removeHull() {
	//EXPEREMENTAL
	if (m_hulls.empty()) {
		return std::vector<EditorPath>({ *this });
	}

	if (orientation() != wykobi::Clockwise) {
		this->reverse();
	}
	
	//make graph of path and hulls
	//keep track of what objects nodes belong to
	//for each hull, find two edges, edge must either bridge to other object or outer path
	//avoid nodes having more then 3 edges
	//bridges are undirected, so one bridge counts as +1 bridge for both objects
	//for each bridge traverse both directions, mark bridges as visited

	Graph graph(*this);

	std::vector<Edge*> hull_bridge_vec;
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
				if (std::find(hull.begin(), hull.end(), node_ptr.get()) == hull.end() && graph.isEdgeLegal((*hull_it), node_ptr.get())) {
					graph.addEdge((*hull_it), node_ptr.get());
					graph.m_edge_vec.back()->m_is_bridge = true;
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
}

std::string EditorPath::toString() {
	std::ostringstream s;
	for (auto it = begin(); it != end(); ++it) {
		s << "(" << (*it)->getPos().x << "," << (*it)->getPos().y << ")";
	}
	return s.str();
}
