//Author: Sivert Andresen Cubedo

#include "Graph.hpp"

//sauce: https://www.geometrictools.com/Documentation/MinimalCycleBasis.pdf

typedef Graph::UndirectedGraph<wykobi::point2d<float>>::Node Node;
typedef Graph::UndirectedGraph<wykobi::point2d<float>>::Edge Edge;

Node* getClockwiseMost(Node* prev_node, Node* current_node) {
	//check if prev exist
	bool exist = false;
	for (Edge* e : prev_node->edges) {
		Node* n = (e->a != prev_node) ? e->a : e->b;
		if (n == current_node) {
			exist = true;
			break;
		}
	}
	if (!exist) return nullptr;
	//find most Clockwise node
	float origin_angle = Math::angleTowardsPoint<float>(current_node->obj, prev_node->obj);
	float low_angle = wykobi::infinity<float>();
	Node* low_node = nullptr;
	for (Edge* e : current_node->edges) {
		Node* n = (e->a != current_node) ? e->a : e->b;
		if (n == prev_node) continue;
		float test_angle = Math::angleTowardsPoint<float>(current_node->obj, n->obj);
		if (test_angle < low_angle) {
			low_angle = test_angle;
			low_node = n;
		}
	}
	return low_node;
}

Node* getCounterClockwiseMost(Node* prev_node, Node* current_node) {
	//check if prev exist
	bool exist = false;
	for (Edge* e : prev_node->edges) {
		Node* n = (e->a != prev_node) ? e->a : e->b;
		if (n == current_node) {
			exist = true;
			break;
		}
	}
	if (!exist) return nullptr;
	//find most counter Clockwise node
	float origin_angle = Math::angleTowardsPoint<float>(current_node->obj, prev_node->obj);
	float high_angle = -1.f;
	Node* high_node = nullptr;
	for (Edge* e : current_node->edges) {
		Node* n = (e->a != current_node) ? e->a : e->b;
		if (n == prev_node) continue;
		float test_angle = Math::angleTowardsPoint<float>(current_node->obj, n->obj);
		if (test_angle > high_angle) {
			high_angle = test_angle;
			high_node = n;
		}
	}
	return high_node;
}

wykobi::vector2d<float> perp(wykobi::vector2d<float> & vector) {
	return wykobi::make_vector<float>(vector.y, -vector.x);
}

struct Tree {
	std::vector<int> cycle;
	std::vector<Tree> children;
};

Tree extractBasis(Graph::UndirectedGraph<wykobi::point2d<float>> & component);

std::vector<int> extractCycle(std::vector<Node*> & closed_walk) {

}

Tree extractCycleFromClosedWalk(Graph::UndirectedGraph<wykobi::point2d<float>> & graph, std::vector<Node*> & closed_walk) {
	Tree tree;
	//3
	std::unordered_map<Node*, int> duplicates;
	std::vector<int> detachments;
	for (int i = 1; i < closed_walk.size() - 1; i++) {
		auto it = duplicates.find(closed_walk[i]);
		if (it != duplicates.end()) {
			int min_i = (*it).second;
			int max_i = i;
			detachments.push_back(min_i);
			for (int j = min_i + 1; j < max_i; j++) {
				duplicates.erase(closed_walk[j]);
				detachments.erase(std::find(detachments.begin(), detachments.end(), j));
			}
			closed_walk.erase(closed_walk.begin() + min_i + 1, closed_walk.begin() + max_i);
			i = min_i;
		}
		else {
			duplicates.emplace(closed_walk[i], i);
		}
	}

	//4
	if (closed_walk.size() > 3) {
		detachments.push_back(0);
		std::sort(detachments.begin(), detachments.end());
		for (int i : detachments) {
			//need to understand listing 4 @https://www.geometrictools.com/Documentation/MinimalCycleBasis.pdf
			Node* original_node = closed_walk[i];
			Node* max_node = closed_walk[i + 1];
			Node* min_node = (i > 0) ? closed_walk[i - 1] : closed_walk[closed_walk.size() - 2];
			wykobi::vector2d<float> direction_min = min_node->obj - original_node->obj;
			wykobi::vector2d<float> direction_max = max_node->obj - original_node->obj;

			bool is_convex = (wykobi::dot_product<float>(direction_max, perp(direction_min)) >= 0);

			std::vector<Node*> in_wedge;
			std::vector<Node*> adjacent_nodes;
			adjacent_nodes.insert(adjacent_nodes.end(), original_node->edges.begin(), original_node->edges.end());
			for (Node* a_node : adjacent_nodes) {
				if (a_node == min_node || a_node == max_node) continue;
				wykobi::vector2d<float> direction = a_node->obj - original_node->obj;
				bool contains_node;
				if (is_convex) {
					contains_node = (wykobi::dot_product<float>(direction, direction_min) > 0 && wykobi::dot_product<float>(direction, direction_max) < 0);
				}
				else {
					(wykobi::dot_product<float>(direction, direction_min) > 0 || wykobi::dot_product<float>(direction, direction_max) < 0);
				}
				if (contains_node) {
					in_wedge.push_back(a_node);
				}
			}
			if (in_wedge.size() > 0) {
				Node temp = *original_node;
				temp.edges.clear();
				Node* clone_node = graph.makeNode(temp);
				for (Node* n : in_wedge) {
					graph.deleteEdge(original_node->getEdge(n));
					graph.makeEdge(clone_node, n);
				}
				Graph::UndirectedGraph<wykobi::point2d<float>> component = graph.depthFirstSearch(clone_node);
				tree.children.push_back(extractBasis(component));
			}	
		}
		tree.cycle = 
	}

	return tree;
}

Tree ExtractCycleFromComponent(Graph::UndirectedGraph<wykobi::point2d<float>> & component) {
	//find left most node
	Node* min_node = component.nodes.front().get();
	for (auto it = component.nodes.begin() + 1; it != component.nodes.end(); ++it) {
		Node* n = (*it).get();
		if (n->obj.x < min_node->obj.x) {
			min_node = n;
		}
	}


	//traverse???
	std::vector<Node*> closed_walk;
	closed_walk.push_back(min_node);
	Node* current_node = min_node;
	std::unique_ptr<Node> artificial_node(new Node(*current_node));
	Node* prev_node = artificial_node.get();
	prev_node->obj.y += 1.f;
	Node* next_node = getClockwiseMost(prev_node, current_node);

	while (next_node != min_node) {
		closed_walk.push_back(next_node);
		prev_node = current_node;
		current_node = next_node;
		next_node = getCounterClockwiseMost(prev_node, current_node);
	}
	closed_walk.push_back(min_node);
	
	Tree tree = extractCycleFromClosedWalk(component, closed_walk);
	return tree;
}

void removeFilaments(Graph::UndirectedGraph<wykobi::point2d<float>> & component) {
	std::list<Node*> end_points;
	for (auto it = component.nodes.begin(); it != component.nodes.end(); ++it) {
		Node* n = (*it).get();
		if (n->edges.size() == 1) {
			end_points.push_back(n);
		}
	}
	if (end_points.size() > 0) {
		auto it = end_points.begin();
		while (it != end_points.end()) {
			Node* n = (*it);
			Edge* e = n->edges.front();
			Node* next = (e->a != n) ? e->a : e->b;
			it = end_points.erase(it);
			component.deleteNode(n);
			if (next->edges.size() < 2) {
				end_points.push_back(next);
			}
		}
	}
}

Tree extractBasis(Graph::UndirectedGraph<wykobi::point2d<float>> & component) {
	Tree tree;
}

//void minimalCycleBasis(Graph::EdgeGraph<wykobi::point2d<float>> & graph, std::vector<Tree> & forest) {
//	forest.insert()
//}


std::vector<wykobi::polygon<float, 2>> Graph::getWykobiPolygonsFromUndirectedGraph(UndirectedGraph<wykobi::point2d<float>> & graph) {
	typedef UndirectedGraph<wykobi::point2d<float>>::Node Node;
	typedef UndirectedGraph<wykobi::point2d<float>>::Edge Edge;

	std::vector<wykobi::polygon<float, 2>> out_vec;



	return out_vec;
}

////find left-top vertex
//float min_x = graph.nodes[0]->obj.x;
//float min_y = graph.nodes[0]->obj.y;
//for (int i = 1; i < graph.nodes.size(); i++) {
//	if (graph.nodes[i]->obj.x < min_x) {
//		min_x = graph.nodes[i]->obj.x;
//	}
//	if (graph.nodes[i]->obj.y < min_y) {
//		min_y = graph.nodes[i]->obj.y;
//	}
//}
//wykobi::point2d<float> top_left = wykobi::make_point<float>(min_x, min_y);
//Node* start_node = graph.nodes[0].get();
//float current_low_distance = wykobi::distance<float>(start_node->obj, top_left);
//for (int i = 1; i < graph.nodes.size(); i++) {
//	Node* test_node = graph.nodes[i].get();
//	float test_distance = wykobi::distance<float>(test_node->obj, top_left);
//	if (test_distance < current_low_distance) {
//		start_node = test_node;
//		current_low_distance = test_distance;
//	}
//}
//find polygons
//std::list<std::pair<Node*, Edge*>> pending_edges;
//for (auto it = start_node->edges.begin(); it != start_node->edges.end(); ++it) {
//	Edge* e = (*it);
//	bool dir = (e->a != start_node) ? false : true;
//	pending_edges.push_back(std::pair<bool, Edge*>(dir, e));
//}
//std::vector<Node*> current_path;
//while (!pending_edges.empty()) {
//	Edge* current_edge;
//	Node* current_node;
//}
//std::list<Node*> pending_nodes;
//pending_nodes.push_back(start_node);
//std::vector<Node*> current_path;
//Edge* current_edge = nullptr;
//Node* current_node = nullptr;
//while (!pending_nodes.empty()) {
//	current_node = pending_nodes.front();
//	pending_nodes.pop_front();
//	current_path.clear();
//	current_edge = nullptr;
//	while (current_node->visited < 1) {
//		Edge* next_edge = nullptr;
//		Node* next_node = nullptr;
//		float last_angle = 0.f;
//		current_node->visited = 1;
//		if (current_edge == nullptr) {
//			for (auto it = current_node->edges.begin(); it != current_node->edges.end(); ++it) {
//				Edge* e = (*it);
//				Node* n = (e->a != current_node) ? e->a : e->b;
//				if (e->visited < 1) {
//					next_edge = e;
//					next_node = n;
//					break;
//				}
//			}
//			if (next_edge == nullptr) break;
//		}
//		else {
//			float low_angle = wykobi::infinity<float>();
//			current_edge->visited++;
//			for (auto it = current_node->edges.begin(); it != current_node->edges.end(); ++it) {
//				Edge* e = (*it);
//				if (e == current_edge) continue;
//				Node* n = (e->a != current_node) ? e->a : e->b;
//				float test_angle = getRelativeAngle<float>(last_angle, angleTowardsPoint<float>(current_node->obj, n->obj));
//				if (test_angle < low_angle) {
//					next_edge = e;
//					next_node = n;
//				}
//			}
//			if (next_edge == nullptr) break;
//			for (auto it = current_node->edges.begin(); it != current_node->edges.end(); ++it) {
//				Edge* e = (*it);
//				if (e != next_edge) {
//
//					Node* n = (e->a != current_node) ? e->a : e->b;
//				}
//			}
//		}
//		last_angle = angleTowardsPoint<float>(next_node->obj, current_node->obj);
//		current_path.push_back(current_node);
//		current_edge = next_edge;
//		current_node = next_node;
//	}
//}
//return out_vec;
//std::list<Node*> pending_nodes;
//std::vector<Node*> current_path;
//pending_nodes.push_back(start_node);
//Node* current_node = nullptr;
//Edge* current_edge = nullptr;
//Node* next_node = nullptr;
//Edge* next_edge = nullptr;
//float last_angle = 0.f;
//while (!pending_nodes.empty()) {
//	current_node = pending_nodes.front();
//	pending_nodes.pop_front();
//	current_edge = nullptr;
//	while (current_node->visited < 1) {
//		current_node->visited = 1;
//		if (current_edge == nullptr) {
//			//if this then current_node is the first node in path
//			//select node from first edge that has visited < 2
//			//put nodes from other edges with visited < 2 in pending nodes
//			for (auto it = current_node->edges.begin(); it != current_node->edges.end(); ++it) {
//				Edge* e = (*it);
//				if (e->visited < 2) {
//					if (next_node == nullptr) {
//						next_node = (e->a != current_node) ? e->a : e->b;
//						next_edge = e;
//					}
//					else {
//						pending_nodes.push_back((e->a != current_node) ? e->a : e->b);
//					}
//				}
//			}
//		}
//		else {
//			//if this then current_node is after first node in path
//			current_edge->visited++;
//			float low_angle = wykobi::infinity<float>();
//			for (auto it = current_node->edges.begin(); it != current_node->edges.end(); ++it) {
//				Edge* e = (*it);
//				if (e == current_edge || e->visited >= 2) continue;
//				Node* n = (e->a != current_node) ? e->a : e->b;
//
//				float test_angle = getRelativeAngle<float>(last_angle, angleTowardsPoint<float>(current_node->obj, n->obj));
//				if (test_angle < low_angle) {
//					if (next_edge != nullptr) pending_nodes.push_back((e->a != current_node) ? e->a : e->b);
//					next_edge = e;
//					next_node = n;
//					low_angle = test_angle;
//				}
//				else {
//					pending_nodes.push_back((e->a != current_node) ? e->a : e->b);
//				}
//			}
//		}
//		last_angle = angleTowardsPoint<float>(next_node->obj, current_node->obj);
//		current_path.push_back(current_node);
//		current_node = next_node;
//		current_edge = next_edge;
//		next_node = nullptr;
//		next_edge = nullptr;
//	}
//	wykobi::polygon<float, 2> poly;
//	for (Node* n : current_path) {
//		poly.push_back(n->obj);
//		n->visited = 0;
//	}
//	out_vec.push_back(poly);
//	current_path.clear();
//}
//return out_vec;
////traverse until start_node if found again
//wykobi::polygon<float, 2> current_polygon;
//Node* current_node = start_node;
//Edge* last_edge = nullptr;
//float last_angle = 45.f;
//while (current_node->visited < 1) {
//	current_node->visited = 1;
//	Edge* current_edge = nullptr;
//	Node* next_node = nullptr;
//	float low_angle = 360.f;
//	for (int i = 0; i < current_node->edges.size(); i++) {
//		Edge* e = current_node->edges[i];
//		if (e == last_edge) continue;
//		Node* n = (e->a != current_node) ? e->a : e->b;
//		float test_angle = getRelativeAngle<float>(last_angle, angleTowardsPoint<float>(current_node->obj, n->obj));
//		if (test_angle < low_angle) {
//			next_node = n;
//			current_edge = e;
//			low_angle = test_angle;
//		}
//	}
//	current_polygon.push_back(current_node->obj);
//	last_angle = angleTowardsPoint<float>(current_node->obj, next_node->obj) - 180.f;
//	if (last_angle < 0.f) last_angle += 360.f;
//	last_edge = current_edge;
//	current_node = next_node;
//}
//out_vec.push_back(current_polygon);
//return out_vec;

//end