//Author: Sivert Andresen Cubedo

#include "RemoveHull.hpp"

RemoveHull::Edge::Edge(Node* a, Node* b) :
	m_a(a),
	m_b(b)
{
	assert(a != b);
}

RemoveHull::Node* RemoveHull::Edge::getNode(Node* n) {
	assert(n == m_a || n == m_b);
	return (n == m_a) ? m_b : m_a;
}

bool RemoveHull::Edge::clockVisit(Node* n) {
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

bool RemoveHull::Edge::counterClockVisit(Node* n) {
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

RemoveHull::Node::Node(MapPoint* point_ptr) :
	m_point(point_ptr)
{
}

wykobi::point2d<float> RemoveHull::Node::getPos() {
	return m_point->getPos();
}

RemoveHull::Edge* RemoveHull::Node::getClockwiseMost(Edge* prev_edge) {
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

RemoveHull::Edge* RemoveHull::Node::getCounterClockwiseMost(Edge* prev_edge) {
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

RemoveHull::Graph::Graph(EditorPath & path) {
	for (MapPoint* p : path) {
		m_node_vec.push_back(std::unique_ptr<Node>(new Node(p)));
		m_outer_path.push_back(m_node_vec.back().get());
	}
	for (auto it = m_outer_path.begin(); it != m_outer_path.end(); ++it) {
		auto next_it = (it + 1 != m_outer_path.end()) ? it + 1 : m_outer_path.begin();
		addEdge((*it), (*next_it));
	}
	for (EditorPath & hull : path.getHulls()) {
		m_hull_vec.push_back(Hull());
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

void RemoveHull::Graph::addEdge(Node* a, Node* b) {
	m_edge_vec.push_back(std::unique_ptr<Edge>(new Edge(a, b)));
	a->m_edges.push_back(m_edge_vec.back().get());
	b->m_edges.push_back(m_edge_vec.back().get());
}

bool RemoveHull::Graph::isEdgeLegal(Node* a, Node* b) {
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

RemoveHull::Path RemoveHull::Graph::traverseBridgeClockwise(Edge* edge, Node* start_node) {
	assert(start_node == edge->m_a || start_node == edge->m_b);
	Path out_vec;
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
RemoveHull::Path RemoveHull::Graph::traverseBridgeCounterClockwise(Edge* edge, Node* start_node) {
	assert(start_node == edge->m_a || start_node == edge->m_b);
	Path out_vec;
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


EditorPath RemoveHull::Graph::makeEditorPath(Path node_vec) {
	EditorPath path;
	for (Node* node : node_vec) {
		path.push_back(node->m_point);
	}
	return path;
}

std::pair<RemoveHull::Path::iterator, RemoveHull::Path::iterator> RemoveHull::Graph::closestNodes(Path & path_1, Path & path_2) {
	std::pair<Path::iterator, Path::iterator> pair;
	auto path_1_centroid = path_1.centroid();
	auto path_2_centroid = path_2.centroid();
	pair.first = path_1.begin();
	float low = wykobi::distance((*pair.first)->getPos(), path_2_centroid);
	for (auto it = path_1.begin() + 1; it != path_1.end(); ++it) {
		float test = wykobi::distance((*it)->getPos(), path_2_centroid);
		if (test < low) {
			low = test;
			pair.first = it;
		}
	}
	pair.second = path_2.begin();
	low = wykobi::distance((*pair.second)->getPos(), path_1_centroid);
	for (auto it = path_2.begin() + 1; it != path_2.end(); ++it) {
		float test = wykobi::distance((*it)->getPos(), path_1_centroid);
		if (test < low) {
			low = test;
			pair.second = it;
		}
	}
	return pair;
}

RemoveHull::Edge* RemoveHull::Graph::connectPath(Path & p1, Path & p2) {
	auto it_pair = closestNodes(p1, p2);
	Path::iterator it_1 = it_pair.first;
	do {
		Path::iterator it_2 = it_pair.second;
		do {
			if (isEdgeLegal((*it_1), (*it_2))) {
				addEdge((*it_1), (*it_2));
				return m_edge_vec.back().get();
			}
			it_2 = (it_2 + 1 != p2.end()) ? it_2 + 1 : p2.begin();
		} while (it_2 != it_pair.second);
		it_1 = (it_1 + 1 != p1.end()) ? it_1 + 1 : p1.begin();
	} while (it_1 != it_pair.first);
	return nullptr;
}

RemoveHull::Edge* RemoveHull::Graph::connectPath(Path & p1, Path & p2, std::vector<Node*> & exclude) {
	auto it_pair = closestNodes(p1, p2);
	Path::iterator it_1 = it_pair.first;
	do {
		if (std::find(exclude.begin(), exclude.end(), (*it_1)) != exclude.end()) {
			it_1 = (it_1 + 1 != p1.end()) ? it_1 + 1 : p1.begin();
			continue;
		}
		Path::iterator it_2 = it_pair.second;
		do {
			if (std::find(exclude.begin(), exclude.end(), (*it_2)) != exclude.end()) {
				it_2 = (it_2 + 1 != p2.end()) ? it_2 + 1 : p2.begin();
				continue;
			}
			if (isEdgeLegal((*it_1), (*it_2))) {
				addEdge((*it_1), (*it_2));
				return m_edge_vec.back().get();
			}
			it_2 = (it_2 + 1 != p2.end()) ? it_2 + 1 : p2.begin();
		} while (it_2 != it_pair.second);
		it_1 = (it_1 + 1 != p1.end()) ? it_1 + 1 : p1.begin();
	} while (it_1 != it_pair.first);
	return nullptr;
}

bool RemoveHull::Graph::connectHull(Hull & hull) {
	bool outer_path_failed = false;
	bool hull_failed = false;

	std::vector<Hull*> connected_queue;

	while (hull.m_connect_count < 2 && !hull_failed) {
		Edge* result = nullptr;
		if (!outer_path_failed) {
			//try to connect to outer path
			if (hull.m_exclude.empty()) {
				result = connectPath(hull, m_outer_path);
			}
			else {
				result = connectPath(hull, m_outer_path, hull.m_exclude);
			}
			if (result != nullptr) {
				++hull.m_connect_count;
				hull.m_exclude.push_back(result->m_a);
				hull.m_exclude.push_back(result->m_b);
				m_hull_bridge_vec.push_back(m_edge_vec.back().get());
			}
			else {
				outer_path_failed = true;
				if (m_connected_hulls.empty()) {
					hull_failed = true;
				}
				else {
					if (m_connected_hulls.size() > 1) {
						wykobi::point2d<float> hull_centroid = hull.centroid();
						connected_queue.insert(connected_queue.end(), m_connected_hulls.begin(), m_connected_hulls.end());
						std::sort(connected_queue.begin(), connected_queue.end(),
							[&](Hull* h1, Hull* h2)
						{ return wykobi::distance(hull_centroid, h1->centroid()) > wykobi::distance(hull_centroid, h2->centroid()); }
						);
					}
					else {
						connected_queue.push_back(m_connected_hulls.front());
					}
				}
			}
		}
		else {
			Hull* attempt_hull = connected_queue.back();
			if (hull.m_exclude.empty()) {
				result = connectPath(hull, *attempt_hull);
			}
			else {
				result = connectPath(hull, *attempt_hull, hull.m_exclude);
			}
			if (result != nullptr) {
				++hull.m_connect_count;
				hull.m_exclude.push_back(result->m_a);
				hull.m_exclude.push_back(result->m_b);
				m_hull_bridge_vec.push_back(m_edge_vec.back().get());
			}
			else {
				connected_queue.pop_back();
				if (connected_queue.empty()) {
					hull_failed = true;
				}
			}
		}
	}

	if (hull.m_connect_count == 2) {
		return true;
	}
	else {
		return false;
	}
}

wykobi::point2d<float> RemoveHull::Path::centroid() {
	wykobi::polygon<float, 2> poly;
	poly.reserve(size());
	for (Node* n : *this) {
		poly.push_back(n->getPos());
	}
	return wykobi::centroid(poly);
}
