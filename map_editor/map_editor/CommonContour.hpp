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

	//std::vector<wykobi::polygon<float, 2>> merge(wykobi::polygon<float, 2> & poly1, wykobi::polygon<float, 2> & poly2) {
	std::vector<wykobi::segment<float, 2>> merge(wykobi::polygon<float, 2> & poly1, wykobi::polygon<float, 2> & poly2) {
		//std::vector<wykobi::polygon<float, 2>> poly_vec;
		//if (Math::isPolygonInsidePolygon(poly1, poly2)) {
		//	poly_vec.push_back(poly2);
		//	return poly_vec;
		//}
		//else if (Math::isPolygonInsidePolygon(poly2, poly1)) {
		//	poly_vec.push_back(poly1);
		//	return poly_vec;
		//}
		//else if (!Math::isPolygonsOverlapping(poly1, poly2)) {
		//	poly_vec.push_back(poly1);
		//	poly_vec.push_back(poly2);
		//	return poly_vec;
		//}

		//make graph
		Graph graph;
		std::vector<std::vector<Graph::Node*>> intersection_vec(poly2.size());	//edge of poly2, Node
		Graph::Node* current_node = nullptr;
		Graph::Node* next_node = nullptr;
		current_node = graph.makeNode(poly1[0]);
		std::size_t i = 1;
		do {
			if (i == poly1.size()) i = 0;
			wykobi::point2d<float> & next_point = poly1[i];
			wykobi::segment<float, 2> current_segment = wykobi::make_segment<float>(current_node->point, next_point);
			//std::vector<wykobi::point2d<float>> point_vec;
			std::vector<std::pair<int, wykobi::point2d<float>>> point_vec;
			for (std::size_t j = 0; j < poly2.size(); ++j) {
				wykobi::segment<float, 2> test_segment = wykobi::edge<float>(poly2, j);
				if (wykobi::intersect(current_segment, test_segment)) {
					//point_vec.push_back(wykobi::intersection_point(current_segment, test_segment));
					point_vec.push_back(std::pair<int, wykobi::point2d<float>>(j, wykobi::intersection_point(current_segment, test_segment)));
				}
			}
			//std::sort(point_vec.begin(), point_vec.end(), [&](wykobi::point2d<float> & p1, wykobi::point2d<float> & p2) {
			//	return wykobi::distance(current_node->point, p1) < wykobi::distance(current_node->point, p2);
			//});
			std::sort(point_vec.begin(), point_vec.end(), [&](auto & p1, auto & p2) {
				return wykobi::distance(current_node->point, p1.second) < wykobi::distance(current_node->point, p2.second);
			});
			//for (wykobi::point2d<float> & p : point_vec) {
			//	next_node = graph.makeNode(p);
			//	current_node->edges.push_back(next_node);
			//	current_node = next_node;
			//}
			for (auto & pair : point_vec) {
				next_node = graph.makeNode(pair.second);
				intersection_vec[pair.first].push_back(next_node);
				current_node->edges.push_back(next_node);
				current_node = next_node;
			}
			next_node = graph.makeNode(next_point);
			current_node->edges.push_back(next_node);
			current_node = next_node;
			++i;
		} while (i != 1);
		//current_node = graph.makeNode(poly2[0]);
		//next_node = nullptr;
		//i = 1;
		//do {
		//	if (i == poly1.size()) i = 0;
		//	wykobi::point2d<float> & next_point = poly1[i];
		//
		//	if (!intersection_vec[i].empty()) {
		//		if (intersection_vec[i].size() > 1) {
		//			std::sort(intersection_vec[i].begin(), intersection_vec[i].end(), [&](Graph::Node* n1, Graph::Node* n2) {
		//				return wykobi::distance(current_node->point, n1->point) < wykobi::distance(current_node->point, n2->point);
		//			});
		//		}
		//		for (Graph::Node* n : intersection_vec[i]) {
		//			next_node = n;
		//			current_node->edges.push_back(next_node);
		//			current_node = next_node;
		//		}
		//	}
		//	next_node = graph.makeNode(next_point);
		//	current_node->edges.push_back(next_node);
		//	++i;
		//} while (i != 1);
		

		
		return getWykobiSegmentsFromGraph(graph);
		//return poly_vec;
	}

}

#endif // !CommonContour_HEADER
