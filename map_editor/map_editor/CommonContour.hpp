//Author: Sivert Andresen Cubedo

#pragma once

#ifndef CommonContour_HEADER
#define CommonContour_HEADER

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

#include <wykobi.hpp>
#include <wykobi_algorithm.hpp>

#include "Math.hpp"

namespace CommonContour {

	struct Graph {
		struct Node {
			std::vector<Node*> edges;
			wykobi::point2d<float> point;
		};
		std::vector<std::unique_ptr<Node>> nodes;
		Node* makeNode(wykobi::point2d<float> & p) {
			Node n;
			n.point = p;
			nodes.push_back(std::unique_ptr<Node>(new Node(n)));
			return nodes.back().get();
		}
	};

	wykobi::vector2d<float> perp(wykobi::vector2d<float> & vector) {
		return wykobi::make_vector<float>(vector.y, -vector.x);
	}

	Graph::Node* getClockwiseMost(Graph::Node* prev_node, Graph::Node* current_node) {
#if 0
		//find most Clockwise node
		float origin_angle = Math::angleTowardsPoint<float>(current_node->point, prev_node->point);
		float low_angle = wykobi::infinity<float>();
		Graph::Node* low_node = nullptr;
		for (Graph::Node* n : current_node->edges) {
			if (n == prev_node) continue;
			float test_angle = Math::getRelativeAngle<float>(origin_angle, Math::angleTowardsPoint<float>(current_node->point, n->point));
			if (test_angle < low_angle) {
				low_angle = test_angle;
				low_node = n;
			}
		}
		return low_node;
#elif 1
		std::vector<Graph::Node*> edges = current_node->edges;
		auto it = std::find(edges.begin(), edges.end(), prev_node);
		if (it != edges.end()) edges.erase(it);
		if (edges.empty()) return nullptr;
		it = edges.begin();
		wykobi::vector2d<float> current_dir = current_node->point - prev_node->point;
		Graph::Node* next_node = (*it);
		++it;
		wykobi::vector2d<float> next_dir = next_node->point - current_node->point;
		bool is_current_convex = (wykobi::dot_product(next_dir, perp(current_dir)) <= 0);
		while (it != edges.end()) {
			Graph::Node* test_node = (*it);
			wykobi::vector2d<float> test_dir = test_node->point - current_node->point;
			if (is_current_convex) {
				//(Dot(dcurr, Perp(dadj)) < 0 or Dot(dnext, Perp(dadj)) < 0)
				if (wykobi::dot_product(current_dir, perp(test_dir)) < 0 || wykobi::dot_product(next_dir, perp(test_dir)) < 0) {
					next_dir = test_dir;
					next_node = test_node;
					is_current_convex = (wykobi::dot_product(next_dir, perp(current_dir)) <= 0);
				}
			}
			else {
				if (wykobi::dot_product(current_dir, perp(test_dir)) < 0 && wykobi::dot_product(next_dir, perp(test_dir)) < 0) {
					next_dir = test_dir;
					next_node = test_node;
					is_current_convex = (wykobi::dot_product(next_dir, perp(current_dir)) <= 0);
				}
			}
			++it;
		}
		return next_node;
#endif
	}
	Graph::Node* getCounterClockwiseMost(Graph::Node* prev_node, Graph::Node* current_node) {
#if 0
		//find most Clockwise node
		float origin_angle = Math::angleTowardsPoint<float>(current_node->point, prev_node->point);
		float high_angle = -1.f;
		Graph::Node* high_node = nullptr;
		for (Graph::Node* n : current_node->edges) {
			if (n == prev_node) continue;
			float test_angle = Math::getRelativeAngle<float>(origin_angle, Math::angleTowardsPoint<float>(current_node->point, n->point));
			if (test_angle > high_angle) {
				high_angle = test_angle;
				high_node = n;
			}
		}
		return high_node;
#elif 1
		std::vector<Graph::Node*> edges = current_node->edges;
		auto it = std::find(edges.begin(), edges.end(), prev_node);
		if (it != edges.end()) edges.erase(it);
		if (edges.empty()) return nullptr;
		it = edges.begin();
		wykobi::vector2d<float> current_dir = current_node->point - prev_node->point;
		Graph::Node* next_node = (*it);
		++it;
		wykobi::vector2d<float> next_dir = next_node->point - current_node->point;
		bool is_current_convex = (wykobi::dot_product(next_dir, perp(current_dir)) <= 0);
		while (it != edges.end()) {
			Graph::Node* test_node = (*it);
			wykobi::vector2d<float> test_dir = test_node->point - current_node->point;
			if (is_current_convex) {
				if (wykobi::dot_product(current_dir, perp(test_dir)) > 0 && wykobi::dot_product(next_dir, perp(test_dir)) > 0) {
					next_dir = test_dir;
					next_node = test_node;
					is_current_convex = (wykobi::dot_product(next_dir, perp(current_dir)) <= 0);
				}
			}
			else {
				if (wykobi::dot_product(current_dir, perp(test_dir)) > 0 || wykobi::dot_product(next_dir, perp(test_dir)) > 0) {
					next_dir = test_dir;
					next_node = test_node;
					is_current_convex = (wykobi::dot_product(next_dir, perp(current_dir)) <= 0);
				}
			}
			++it;
		}
		return next_node;
#endif
	}

	std::vector<wykobi::segment<float, 2>> getWykobiSegmentsFromGraph(Graph & graph) {
		std::vector<wykobi::segment<float, 2>> out_vec;
		for (std::size_t i = 0; i < graph.nodes.size(); ++i) {
			Graph::Node* n = graph.nodes[i].get();
			for (Graph::Node* e : n->edges) {
				out_vec.push_back(wykobi::make_segment<float>(n->point, e->point));
			}
		}
		return out_vec;
	}

	std::vector<wykobi::polygon<float, 2>> mergeUnion(wykobi::polygon<float, 2> poly1, wykobi::polygon<float, 2> poly2) {
	//std::vector<wykobi::segment<float, 2>> mergeUnion(wykobi::polygon<float, 2> poly1, wykobi::polygon<float, 2> poly2) {
		std::vector<wykobi::polygon<float, 2>> poly_vec;
		if (Math::isPolygonInsidePolygon(poly1, poly2)) {
			poly_vec.push_back(poly2);
			return poly_vec;
		}
		else if (Math::isPolygonInsidePolygon(poly2, poly1)) {
			poly_vec.push_back(poly1);
			return poly_vec;
		}
		else if (!Math::isPolygonsOverlapping(poly1, poly2)) {
			poly_vec.push_back(poly1);
			poly_vec.push_back(poly2);
			return poly_vec;
		}

		//make sure both polygons are clockwise
		if (wykobi::polygon_orientation(poly1) != wykobi::CounterClockwise) {
			poly1.reverse();
		}
		if (wykobi::polygon_orientation(poly2) != wykobi::CounterClockwise) {
			poly2.reverse();
		}

		//std::cout << Math::Debug::toString(poly1) << "\n";
		//std::cout << Math::Debug::toString(poly2) << "\n";

		//make graph
		Graph graph;
		std::vector<std::vector<Graph::Node*>> intersection_vec(poly2.size());	//edge of poly2, Node
		Graph::Node* start_node = nullptr;
		Graph::Node* current_node = nullptr;
		Graph::Node* next_node = nullptr;
		Graph::Node* prev_node = nullptr;
		for (std::size_t i = 0; i <= poly1.size(); ++i) {
			if (i == 0) {
				current_node = graph.makeNode(poly1[i]);
				start_node = current_node;
			}
			else {
				prev_node = current_node;
				wykobi::point2d<float> next_poly_point;
				if (i == poly1.size()) {
					next_poly_point = start_node->point;
				}
				else {
					next_poly_point = poly1[i];
				}
				wykobi::segment<float, 2> current_segment = wykobi::make_segment<float>(prev_node->point, next_poly_point);
				std::vector<std::pair<int, wykobi::point2d<float>>> point_vec;
				for (std::size_t j = 0; j < poly2.size(); ++j) {
					wykobi::segment<float, 2> test_segment = wykobi::edge<float>(poly2, j);
					if (wykobi::intersect(current_segment, test_segment)) {
						point_vec.push_back(std::pair<int, wykobi::point2d<float>>(j, wykobi::intersection_point(current_segment, test_segment)));
					}
				}
				std::sort(point_vec.begin(), point_vec.end(), [&](auto & p1, auto & p2) {
					return wykobi::distance(prev_node->point, p1.second) < wykobi::distance(prev_node->point, p2.second);
				});
				for (auto & pair : point_vec) {
					current_node = graph.makeNode(pair.second);
					intersection_vec[pair.first].push_back(current_node);
					prev_node->edges.push_back(current_node);
					prev_node = current_node;
				}
				if (i == poly1.size()) {
					current_node->edges.push_back(start_node);
				}
				else {
					current_node = graph.makeNode(next_poly_point);
					prev_node->edges.push_back(current_node);
				}
			}
		}
		current_node = nullptr;
		next_node = nullptr;
		start_node = nullptr;
		for (std::size_t i = 0; i < poly2.size(); ++i) {
			if (i == 0) {
				current_node = graph.makeNode(poly2[i]);
				start_node = current_node;
			}
			else {
				next_node = graph.makeNode(poly2[i]);
				current_node->edges.push_back(next_node);
				current_node = next_node;
			}
			if (!intersection_vec[i].empty()) {
				if (intersection_vec[i].size() > 1) {
					std::sort(intersection_vec[i].begin(), intersection_vec[i].end(), [&](Graph::Node* n1, Graph::Node* n2) {
						return wykobi::distance(current_node->point, n1->point) < wykobi::distance(current_node->point, n2->point);
					});
				}
				for (Graph::Node* n : intersection_vec[i]) {
					next_node = n;
					current_node->edges.push_back(next_node);
					current_node = next_node;
				}
			}
		}
		current_node->edges.push_back(start_node);

		//find most top-left node
		std::vector<Graph::Node*> node_path;
		current_node = nullptr;
		next_node = nullptr;
		start_node = nullptr;
		prev_node = nullptr;
		wykobi::point2d<float> edge_point = wykobi::make_point<float>(wykobi::infinity<float>(), wykobi::infinity<float>());
		float low_dist = wykobi::infinity<float>();
		for (std::size_t i = 0; i < graph.nodes.size(); ++i) {
			Graph::Node* n = graph.nodes[i].get();
			if (n->point.x < edge_point.x) edge_point.x = n->point.x;
			if (n->point.y < edge_point.y) edge_point.y = n->point.y;
		}
		for (std::size_t i = 0; i < graph.nodes.size(); ++i) {
			Graph::Node* n = graph.nodes[i].get();
			float test_dist = wykobi::distance<float>(edge_point, n->point);
			if (test_dist < low_dist) {
				low_dist = test_dist;
				start_node = n;
			}
		}
		//traverse graph
		Graph::Node fake_node;
		fake_node.point = start_node->point - wykobi::make_vector<float>(1.f, 0.f);
		current_node = getClockwiseMost(&fake_node, start_node);
		//current_node = start_node->edges.front();
		//std::cout << Math::Debug::toString(fake_node.point) << "->" << Math::Debug::toString(start_node->point) << "\n";
		//std::cout << Math::Debug::toString(start_node->point) << "	" << start_node << "\n";
		prev_node = start_node;
		node_path.push_back(start_node);
		while (current_node != start_node) {
			//std::cout << Math::Debug::toString(current_node->point) << "	" << current_node << "\n";
			node_path.push_back(current_node);
			next_node = getClockwiseMost(prev_node, current_node);
			prev_node = current_node;
			current_node = next_node;
		}
		wykobi::polygon<float, 2> out_poly;
		out_poly.reserve(node_path.size());
		for (Graph::Node* n : node_path) {
			out_poly.push_back(n->point);
		}
		poly_vec.push_back(out_poly);
		return poly_vec;
		

		//Graph test_graph;
		//prev_node = nullptr;
		//for (Graph::Node* n : node_path) {
		//	Graph::Node* t = test_graph.makeNode(n->point);
		//	if (prev_node != nullptr) {
		//		prev_node->edges.push_back(t);
		//	}
		//	prev_node = t;
		//}
		//test_graph.nodes.back()->edges.push_back(test_graph.nodes.front().get());
		//std::cout << graph.nodes.front().get() << "\n";
		//std::cout << (graph.nodes.begin() + 5)->get()->edges.front() << "\n";

		//return getWykobiSegmentsFromGraph(test_graph);
		//return getWykobiSegmentsFromGraph(graph);
		//return poly_vec;
	}

}

#endif // !CommonContour_HEADER
