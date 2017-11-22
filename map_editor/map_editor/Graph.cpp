//Author: Sivert Andresen Cubedo

#include "Graph.hpp"

std::vector<wykobi::polygon<float, 2>> Graph::getWykobiPolygonsFromEdgeGraph(EdgeGraph<wykobi::point2d<float>> & graph) {
	typedef EdgeGraph<wykobi::point2d<float>>::Node Node;
	typedef EdgeGraph<wykobi::point2d<float>>::Edge Edge;

	std::vector<wykobi::polygon<float, 2>> out_vec;
	//find left-top vertex
	float min_x = graph.nodes[0]->obj.x;
	float min_y = graph.nodes[0]->obj.y;
	for (int i = 1; i < graph.nodes.size(); i++) {
		if (graph.nodes[i]->obj.x < min_x) {
			min_x = graph.nodes[i]->obj.x;
		}
		if (graph.nodes[i]->obj.y < min_y) {
			min_y = graph.nodes[i]->obj.y;
		}
	}
	wykobi::point2d<float> top_left = wykobi::make_point<float>(min_x, min_y);
	Node* start_node = graph.nodes[0].get();
	float current_low_distance = wykobi::distance<float>(start_node->obj, top_left);
	for (int i = 1; i < graph.nodes.size(); i++) {
		Node* test_node = graph.nodes[i].get();
		float test_distance = wykobi::distance<float>(test_node->obj, top_left);
		if (test_distance < current_low_distance) {
			start_node = test_node;
			current_low_distance = test_distance;
		}
	}

	//find polygons

	return out_vec;
}

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