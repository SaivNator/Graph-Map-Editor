//Author: Sivert Andresen Cubedo
#include "MergeTriangles.hpp"

EditorPath MergeTriangles::mergeTriangle(std::vector<EditorTriangle>& triangle_vec) {

	Graph graph(triangle_vec);

	return graph.traverse();
}

MergeTriangles::Node::Node(MapPoint* point_ptr) :
	m_point(point_ptr)
{
}

void MergeTriangles::Node::addEdge(Node * n) {
	if (std::find(m_edges.begin(), m_edges.end(), n) == m_edges.end()) {
		m_edges.push_back(n);
	}
}

wykobi::point2d<float> MergeTriangles::Node::getPos() {
	return  m_point->getPos();
}

MergeTriangles::Node * MergeTriangles::Node::getClockwiseMost(Node * prev_node) {
	std::vector<Node*> edges = m_edges;
	std::vector<Node*>::iterator it = std::find(edges.begin(), edges.end(), prev_node);
	//assert(it != edges.end()); //fake node is not part of the graph
	if (it != edges.end()) {
		edges.erase(it);
	}
	if (edges.empty()) return nullptr;
	it = edges.begin();
	wykobi::vector2d<float> current_dir = this->getPos() - prev_node->getPos();
	Node* next_node = (*it);
	++it;
	wykobi::vector2d<float> next_dir = next_node->getPos() - this->getPos();
	bool is_current_convex = (wykobi::dot_product(next_dir, wykobi::perpendicular(current_dir)) <= 0);
	while (it != edges.end()) {
		Node* test_node = (*it);
		wykobi::vector2d<float> test_dir = test_node->getPos() - this->getPos();
		if (is_current_convex) {
			if (wykobi::dot_product(current_dir, wykobi::perpendicular(test_dir)) < 0 || wykobi::dot_product(next_dir, wykobi::perpendicular(test_dir)) < 0) {
				next_dir = test_dir;
				next_node = test_node;
				is_current_convex = (wykobi::dot_product(next_dir, wykobi::perpendicular(current_dir)) <= 0);
			}
		}
		else {
			if (wykobi::dot_product(current_dir, wykobi::perpendicular(test_dir)) < 0 && wykobi::dot_product(next_dir, wykobi::perpendicular(test_dir)) < 0) {
				next_dir = test_dir;
				next_node = test_node;
				is_current_convex = (wykobi::dot_product(next_dir, wykobi::perpendicular(current_dir)) <= 0);
			}
		}
		++it;
	}
	return next_node;
}

MergeTriangles::Node * MergeTriangles::Node::getCounterClockwiseMost(Node * prev_node) {
	std::vector<Node*> edges = m_edges;
	std::vector<Node*>::iterator it = std::find(edges.begin(), edges.end(), prev_node);
	//assert(it != edges.end()); //fake node is not part of the graph
	if (it != edges.end()) {
		edges.erase(it);
	}
	if (edges.empty()) return nullptr;
	it = edges.begin();
	wykobi::vector2d<float> current_dir = this->getPos() - prev_node->getPos();
	Node* next_node = (*it);
	++it;
	wykobi::vector2d<float> next_dir = next_node->getPos() - this->getPos();
	bool is_current_convex = (wykobi::dot_product(next_dir, wykobi::perpendicular(current_dir)) <= 0);
	while (it != edges.end()) {
		Node* test_node = (*it);
		wykobi::vector2d<float> test_dir = test_node->getPos() - this->getPos();
		if (is_current_convex) {
			if (wykobi::dot_product(current_dir, wykobi::perpendicular(test_dir)) > 0 && wykobi::dot_product(next_dir, wykobi::perpendicular(test_dir)) > 0) {
				next_dir = test_dir;
				next_node = test_node;
				is_current_convex = (wykobi::dot_product(next_dir, wykobi::perpendicular(current_dir)) <= 0);
			}
		}
		else {
			if (wykobi::dot_product(current_dir, wykobi::perpendicular(test_dir)) > 0 || wykobi::dot_product(next_dir, wykobi::perpendicular(test_dir)) > 0) {
				next_dir = test_dir;
				next_node = test_node;
				is_current_convex = (wykobi::dot_product(next_dir, wykobi::perpendicular(current_dir)) <= 0);
			}
		}
		++it;
	}
	return next_node;
}

MergeTriangles::Graph::Graph(std::vector<EditorTriangle>& triangles) {
	std::map<MapPoint*, Node*> point_map;
	//create nodes
	for (EditorTriangle & current_triangle : triangles) {
		if (current_triangle.orientation() != wykobi::Clockwise) {
			current_triangle.reverse();
		}
		for (MapPoint* point : current_triangle) {
			if (point_map.find(point) == point_map.end()) {
				m_nodes.push_back(std::unique_ptr<Node>(new Node(point)));
				point_map.emplace(point, m_nodes.back().get());
			}
		}
	}
	//add edges
	for (EditorTriangle & current_triangle : triangles) {
		for (auto current_it = current_triangle.begin(); current_it != current_triangle.end(); ++current_it) {
			auto next_it = (current_it != current_triangle.end() - 1) ? current_it + 1 : current_triangle.begin();
			auto current_map_it = point_map.find((*current_it));
			assert(current_map_it != point_map.end());
			auto next_map_it = point_map.find((*next_it));
			assert(next_map_it != point_map.end());
			current_map_it->second->addEdge(next_map_it->second);
		}
	}
}

EditorPath MergeTriangles::Graph::traverse() {
	//find lowest cord
	assert(m_nodes.size() >= 3);
	wykobi::point2d<float> low_point = m_nodes.front()->getPos();
	for (auto it = m_nodes.begin() + 1; it != m_nodes.end(); ++it) {
		wykobi::point2d<float> test_point = (*it)->getPos();
		if (test_point.x < low_point.x) low_point.x = test_point.x;
		if (test_point.y < low_point.y) low_point.y = test_point.y;
	}
	Node* start_node = m_nodes.front().get();
	float low_dist = wykobi::distance(low_point, m_nodes.front()->getPos());
	for (auto it = m_nodes.begin() + 1; it != m_nodes.end(); ++it) {
		float test_dist = wykobi::distance(low_point, (*it)->getPos());
		if (test_dist < low_dist) {
			low_dist = test_dist;
			start_node = (*it).get();
		}
	}

	//traverse
	MapPoint fake_point(start_node->getPos() + wykobi::make_point(-1.f, -1.f));
	Node fake_node(&fake_point);

	std::vector<Node*> outer_path;
	outer_path.push_back(start_node);

	Node* current_node = start_node->getCounterClockwiseMost(&fake_node);
	Node* last_node = start_node;
	
	while (current_node != start_node) {
		outer_path.push_back(current_node);
		Node* next_node = current_node->getCounterClockwiseMost(last_node);
		last_node = current_node;
		current_node = next_node;
	}

	EditorPath out_path;
	for (Node* n : outer_path) {
		n->m_is_outer = true;
		out_path.push_back(n->m_point);
	}
	return out_path;
}

std::vector<EditorPath> MergeTriangles::Graph::findHull() {

	std::vector<Node*> pending_nodes;
	for (auto & node_ptr : m_nodes) {
		if (!node_ptr->m_is_outer) {
			pending_nodes.push_back(node_ptr.get());
		}
	}



	return std::vector<EditorPath>();
}
