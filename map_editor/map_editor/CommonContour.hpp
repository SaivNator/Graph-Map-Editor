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

			bool visited = false;

			void addEdge(Node* e) {
				if (std::find(edges.begin(), edges.end(), e) == edges.end()) {
					edges.push_back(e);
				}
			}
		};
		std::vector<std::unique_ptr<Node>> nodes;
		Node* makeNode(wykobi::point2d<float> & p) {
			Node n;
			n.point = p;
			nodes.push_back(std::unique_ptr<Node>(new Node(n)));
			return nodes.back().get();
		}
	};

	//Forward declaration
	std::vector<std::vector<Graph::Node*>> findHull(Graph & graph);
	std::vector<Graph::Node*> traverseUnion(Graph & graph);
	std::vector<wykobi::segment<float, 2>> getWykobiSegmentsFromGraph(Graph & graph);
	wykobi::polygon<float, 2> pathToWykobiPolygon(std::vector<Graph::Node*> & path);
	std::vector<Graph::Node*> insertNodesOnPath(std::vector<Graph::Node*> & path, std::vector<std::vector<Graph::Node*>> & nodes);
	Graph makeGraphFromPolygons(wykobi::polygon<float, 2> poly1, wykobi::polygon<float, 2> poly2);
	std::vector<wykobi::polygon<float, 2>> mergeUnion(wykobi::polygon<float, 2> poly1, wykobi::polygon<float, 2> poly2);
	Graph::Node* getClockwiseMost(Graph::Node* prev_node, Graph::Node* current_node);
	Graph::Node* getCounterClockwiseMost(Graph::Node* prev_node, Graph::Node* current_node);

	/*
	Perpendicular or someshit?!?!?
	*/
	wykobi::vector2d<float> perp(wykobi::vector2d<float> & vector) {
		return wykobi::make_vector<float>(vector.y, -vector.x);
	}

	/*
	Insert nodes in path from 2d vector
	Ordered by distance to existing node in path
	*/
	std::vector<Graph::Node*> insertNodesOnPath(std::vector<Graph::Node*> & path, std::vector<std::vector<Graph::Node*>> & nodes) {
		std::vector<Graph::Node*> out_path;
		for (std::size_t i = 0; i < path.size(); ++i) {
			out_path.push_back(path[i]);
			if (!nodes[i].empty()) {
				if (nodes[i].size() > 1) {
					std::sort(nodes[i].begin(), nodes[i].end(), [&](Graph::Node* n1, Graph::Node* n2)
						{ return (wykobi::distance(path[i]->point, n1->point) < wykobi::distance(path[i]->point, n2->point)); }
					);
				}
				for (Graph::Node* n : nodes[i]) {
					out_path.push_back(n);
				}
			}
		}
		return out_path;
	}

	/*
	Find hull in graph
	*/
	std::vector<std::vector<Graph::Node*>> findHull(Graph & graph) {
		//find all nodes with more then one edge
		std::vector<Graph::Node*> outer_nodes = traverseUnion(graph);
		std::vector<Graph::Node*> pending_nodes;
		for (std::unique_ptr<Graph::Node> & n : graph.nodes) {
			if (n->edges.size() > 1 && std::find(outer_nodes.begin(), outer_nodes.end(), n.get()) == outer_nodes.end()) {
				pending_nodes.push_back(n.get());
			}
		}


		std::vector<std::vector<Graph::Node*>> hull_vec;

		//for each edge of start node, go most clockwise until start node is reached (then we found hull), or until an already visited node is reached (then no hull)
		//Graph::Node* start_node;

		for (Graph::Node* start_node : pending_nodes) {
			if (start_node->visited) continue;
			for (Graph::Node* current_branch : start_node->edges) {
				std::vector<Graph::Node*> current_path;
				Graph::Node* last_node = start_node;
				Graph::Node* current_node = current_branch;
				current_path.push_back(start_node);
				while (true) {
					current_path.push_back(current_node);
					current_node->visited = true;
					Graph::Node* next_node = getClockwiseMost(last_node, current_node);

					if (next_node->visited) {
						for (Graph::Node* n : current_path) {
							n->visited = false;
						}
						break;
					}
					else if (next_node == start_node) {
						for (Graph::Node* n : current_path) {
							
						}
						hull_vec.push_back(current_path);
						break;
					}
					last_node = current_node;
					current_node = next_node;
				}
			}
		}
		return hull_vec;
	}

	/*
	Make graphs from vector of polygons
	*/
	Graph makeGraphFromPolygons(wykobi::polygon<float, 2> poly1, wykobi::polygon<float, 2> poly2) {
		//make sure polys has same orientation
		if (wykobi::polygon_orientation(poly1) != wykobi::CounterClockwise) {
			poly1.reverse();
		}
		if (wykobi::polygon_orientation(poly2) != wykobi::CounterClockwise) {
			poly2.reverse();
		}
		Graph graph;
		std::vector<Graph::Node*> poly1_path;
		std::vector<Graph::Node*> poly2_path;
		for (std::size_t i = 0; i < poly1.size(); ++i) {
			poly1_path.push_back(graph.makeNode(poly1[i]));
		}
		for (std::size_t i = 0; i < poly2.size(); ++i) {
			auto it = std::find_if(poly1_path.begin(), poly1_path.end(), [&](Graph::Node* n1) { return n1->point == poly2[i]; });
			if (it != poly1_path.end()) {
				poly2_path.push_back((*it));
			}
			else {
				poly2_path.push_back(graph.makeNode(poly2[i]));
			}
		}
		std::vector<std::vector<Graph::Node*>> poly1_intersections(poly1_path.size());
		std::vector<std::vector<Graph::Node*>> poly2_intersections(poly2_path.size());
		//find
		for (std::size_t i = 0; i < poly1_path.size(); ++i) {
			Graph::Node* outer_node_a = poly1_path[i];
			Graph::Node* outer_node_b = (i + 1 < poly1_path.size()) ? poly1_path[i + 1] : poly1_path[0];
			wykobi::segment<float, 2> outer_segment = wykobi::make_segment(outer_node_a->point, outer_node_b->point);
			for (std::size_t j = 0; j < poly2_path.size(); ++j) {
				Graph::Node* inner_node_a = poly2_path[j];
				Graph::Node* inner_node_b = (j + 1 < poly2_path.size()) ? poly2_path[j + 1] : poly2_path[0];
				wykobi::segment<float, 2> inner_segment = wykobi::make_segment(inner_node_a->point, inner_node_b->point);
				if (wykobi::intersect(outer_segment, inner_segment)) {
					wykobi::point2d<float> intersection = wykobi::intersection_point(outer_segment, inner_segment);
					if (
						intersection == outer_node_a->point ||
						intersection == outer_node_b->point ||
						intersection == inner_node_a->point ||
						intersection == inner_node_b->point
						)
					{
						//???
					}
					else {
						//add to intersecton vectors
						Graph::Node* n = graph.makeNode(intersection);
						poly1_intersections[i].push_back(n);
						poly2_intersections[j].push_back(n);
					}
				}
			}
		}
		//insert intersections
		poly1_path = insertNodesOnPath(poly1_path, poly1_intersections);
		poly2_path = insertNodesOnPath(poly2_path, poly2_intersections);
		//insert potential nodes that are "on" existing edges
		std::vector<std::vector<Graph::Node*>> poly1_on_edge(poly1_path.size());
		std::vector<std::vector<Graph::Node*>> poly2_on_edge(poly2_path.size());
		for (std::size_t i = 0; i < poly1_path.size(); ++i) {
			Graph::Node* outer_node_a = poly1_path[i];
			Graph::Node* outer_node_b = (i + 1 < poly1_path.size()) ? poly1_path[i + 1] : poly1_path[0];
			wykobi::segment<float, 2> outer_segment = wykobi::make_segment(outer_node_a->point, outer_node_b->point);
			for (std::size_t j = 0; j < poly2_path.size(); ++j) {
				Graph::Node* inner_node = poly2_path[j];
				if ((inner_node != outer_node_a) && (inner_node != outer_node_b) && wykobi::point_on_segment(inner_node->point, outer_segment)) {
					poly1_on_edge[i].push_back(inner_node);
				}
			}
		}
		for (std::size_t i = 0; i < poly2_path.size(); ++i) {
			Graph::Node* outer_node_a = poly2_path[i];
			Graph::Node* outer_node_b = (i + 1 < poly2_path.size()) ? poly2_path[i + 1] : poly2_path[0];
			wykobi::segment<float, 2> outer_segment = wykobi::make_segment(outer_node_a->point, outer_node_b->point);
			for (std::size_t j = 0; j < poly1_path.size(); ++j) {
				Graph::Node* inner_node = poly1_path[j];
				if ((inner_node != outer_node_a) && (inner_node != outer_node_b) && wykobi::point_on_segment(inner_node->point, outer_segment)) {
					poly2_on_edge[i].push_back(inner_node);
				}
			}
		}
		//insert points on edges
		poly1_path = insertNodesOnPath(poly1_path, poly1_on_edge);
		poly2_path = insertNodesOnPath(poly2_path, poly2_on_edge);
		//add edges to graph
		std::vector<Graph::Node*>::iterator it_1 = poly1_path.begin();
		while (it_1 != poly1_path.end()) {
			Graph::Node* outer_node_a = (*it_1);
			Graph::Node* outer_node_b = (std::next(it_1) != poly1_path.end()) ? (*std::next(it_1)) : (*poly1_path.begin());
			outer_node_a->addEdge(outer_node_b);
			++it_1;
		}
		it_1 = poly2_path.begin();
		while (it_1 != poly2_path.end()) {
			Graph::Node* outer_node_a = (*it_1);
			Graph::Node* outer_node_b = (std::next(it_1) != poly2_path.end()) ? (*std::next(it_1)) : (*poly2_path.begin());
			outer_node_a->addEdge(outer_node_b);
			++it_1;
		}
		return graph;
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
	wykobi::polygon<float, 2> pathToWykobiPolygon(std::vector<Graph::Node*> & path) {
		wykobi::polygon<float, 2> poly;
		poly.reserve(path.size());
		for (Graph::Node* n : path) {
			poly.push_back(n->point);
		}
		return poly;
	}

	std::vector<Graph::Node*> traverseUnion(Graph & graph) {
		std::vector<Graph::Node*> node_path;
		Graph::Node* current_node = nullptr;
		Graph::Node* next_node = nullptr;
		Graph::Node* start_node = nullptr;
		Graph::Node* prev_node = nullptr;
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
		Graph::Node fake_node;
		fake_node.point = start_node->point - wykobi::make_vector<float>(1.f, 0.f);
		current_node = getClockwiseMost(&fake_node, start_node);
		prev_node = start_node;
		node_path.push_back(start_node);
		while (current_node != start_node) {
			node_path.push_back(current_node);
			next_node = getClockwiseMost(prev_node, current_node);
			prev_node = current_node;
			current_node = next_node;
		}
		return node_path;
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

		Graph graph = makeGraphFromPolygons(poly1, poly2);
		std::vector<Graph::Node*> node_path = traverseUnion(graph);
		std::vector<std::vector<Graph::Node*>> hull_vec = findHull(graph);
		wykobi::polygon<float, 2> out_poly = pathToWykobiPolygon(node_path);
		std::vector<wykobi::polygon<float, 2>> temp_vec;
		if (!hull_vec.empty()) {
			for (auto p : hull_vec) {
				temp_vec.push_back(pathToWykobiPolygon(p));
			}
			poly_vec = Math::Clipper::removePolygonHull(out_poly, temp_vec);
			return poly_vec;
		}
		else {
			poly_vec.push_back(out_poly);
			return poly_vec;
		}
	}
}

CommonContour::Graph::Node* CommonContour::getClockwiseMost(Graph::Node* prev_node, Graph::Node* current_node) {
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

CommonContour::Graph::Node* CommonContour::getCounterClockwiseMost(Graph::Node* prev_node, Graph::Node* current_node) {
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




#endif // !CommonContour_HEADER

/*
std::vector<Graph::Node*> temp_path;
for (std::size_t i = 0; i < poly1_path.size(); ++i) {
Graph::Node* current_node = poly1_path[i];
temp_path.push_back(current_node);
if (!poly1_intersections[i].empty()) {
if (poly1_intersections[i].size() > 1) {
std::sort(poly1_intersections[i].begin(), poly1_intersections[i].end(), [&](Graph::Node* n1, Graph::Node* n2)
{ return (wykobi::distance(current_node->point, n1->point) < wykobi::distance(current_node->point, n2->point)); }
);
}
for (Graph::Node* n : poly1_intersections[i]) {
temp_path.push_back(n);
}
}
}
poly1_path.swap(temp_path);
temp_path.clear();
for (std::size_t i = 0; i < poly2_path.size(); ++i) {
Graph::Node* current_node = poly2_path[i];
temp_path.push_back(current_node);
if (!poly2_intersections[i].empty()) {
if (poly2_intersections[i].size() > 1) {
std::sort(poly2_intersections[i].begin(), poly2_intersections[i].end(), [&](Graph::Node* n1, Graph::Node* n2)
{ return (wykobi::distance(current_node->point, n1->point) < wykobi::distance(current_node->point, n2->point)); }
);
}
for (Graph::Node* n : poly2_intersections[i]) {
temp_path.push_back(n);
}
}
}
*/

/*
//make graph
Graph graph;
//std::vector<Graph::Node*> equal_vec(poly2.size(), nullptr);				//contains nodes/verticies that are equal
std::vector<std::vector<Graph::Node*>> intersection_vec(poly2.size());	//contains nodes for intersecion points on edges
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
*/

/*
auto it_1 = poly1_path.begin();
while (it_1 != poly1_path.end()) {
Graph::Node* outer_node_a = (*it_1);
Graph::Node* outer_node_b = (std::next(it_1) != poly1_path.end()) ? (*std::next(it_1)) : (*poly1_path.begin());
wykobi::segment<float, 2> outer_segment = wykobi::make_segment(outer_node_a->point, outer_node_b->point);
//std::vector<Graph::Node*> intersection_vec;
//auto it_2 = poly2_path.begin();
while (it_2 != poly2_path.end()) {
Graph::Node* inner_node_a = (*it_2);
Graph::Node* inner_node_b = (std::next(it_2) != poly2_path.end()) ? (*std::next(it_2)) : (*poly2_path.begin());
wykobi::segment<float, 2> inner_segment = wykobi::make_segment(inner_node_a->point, inner_node_b->point);
if (wykobi::intersect(outer_segment, inner_segment)) {
wykobi::point2d<float> intersection = wykobi::intersection_point(outer_segment, inner_segment);
if (
intersection == outer_node_a->point ||
intersection == outer_node_b->point ||
intersection == inner_node_a->point ||
intersection == inner_node_b->point
)
{
std::next(it_2);
}
else {
Graph::Node* n = graph.makeNode(intersection);
intersection_vec.push_back(n);
//it_2 = poly2_path.insert(it_2, n);
it_2 = std::next(it_2);
}
}
else {
it_2 = std::next(it_2);
}
}
if (!intersection_vec.empty()) {
if (intersection_vec.size() > 1) {
std::sort(intersection_vec.begin(), intersection_vec.end(), [&](Graph::Node* n1, Graph::Node* n2)
{ return wykobi::distance(outer_node_a->point, n1->point) < wykobi::distance(outer_node_a->point, n2->point); }
);
}
for (Graph::Node* n : intersection_vec) {
it_1 = poly1_path.insert(it_1, n);
}
}
it_1 = std::next(it_1);
}
*/

/*
Graph graph;
std::vector<std::vector<Graph::Node*>> intersection_vec(poly2.size());	//contains nodes for intersecion points on edges
//make nodes of all points in poly1
std::vector<Graph::Node*> poly1_path;
for (std::size_t i = 0; i < poly1.size(); ++i) {
poly1_path.push_back(graph.makeNode(poly1[i]));
}
//iterate poly1
//check for intersections and add relations
for (std::size_t i = 0; i < poly1_path.size(); ++i) {
wykobi::segment<float, 2> poly1_segment = wykobi::edge(poly1, i);
Graph::Node* node_a = poly1_path[i];
Graph::Node* node_b = (i < poly1.size() - 1) ? poly1_path[i + 1] : poly1_path[0];
std::vector<std::pair<int, wykobi::point2d<float>>> point_vec;
for (std::size_t j = 0; j < poly2.size(); ++j) {
//check for intersections
wykobi::segment<float, 2> poly2_segment = wykobi::edge(poly2, j);
if (wykobi::intersect(poly1_segment, poly2_segment)) {
wykobi::point2d<float> intersect_point = wykobi::intersection_point(poly1_segment, poly2_segment);
point_vec.push_back(std::pair<int, wykobi::point2d<float>>(j, intersect_point));
}
}
//sort
std::sort(point_vec.begin(), point_vec.end(), [&](auto & p1, auto & p2) {
return wykobi::distance(node_a->point, p1.second) < wykobi::distance(node_a->point, p2.second);
});
//add edges to nodes
Graph::Node* current_node = node_a;
for (std::pair<int, wykobi::point2d<float>> & pair : point_vec) {
Graph::Node* next_node = graph.makeNode(pair.second);
intersection_vec[pair.first].push_back(next_node);
current_node->edges.push_back(next_node);
current_node = next_node;
}
current_node->edges.push_back(node_b);	//add last edge
}
//make nodes of points in poly2 that are not discovered
std::vector<Graph::Node*> poly2_path;
for (std::size_t i = 0; i < poly2.size(); ++i) {
auto equal_it = std::find_if(poly1_path.begin(), poly1_path.end(), [&](Graph::Node* node) { return node->point == poly2[i]; });
if (equal_it != poly1_path.end()) {
poly2_path.push_back((*equal_it));
}
else {
poly2_path.push_back(graph.makeNode(poly2[i]));
}
}

std::cout << "poly1_path: ";
for (auto n : poly1_path) {
std::cout << n << " ";
std::cout << Math::Debug::toString(n->point) << "->";
}
std::cout << "\n";
std::cout << "poly2_path: ";
for (auto n : poly2_path) {
std::cout << n << " ";
std::cout << Math::Debug::toString(n->point) << "->";
}
std::cout << "\n";

//iterate poly2
//make nodes of not yet discovered verticies and add edges
for (std::size_t i = 0; i < poly2_path.size(); ++i) {
Graph::Node* node_a = poly2_path[i];
Graph::Node* node_b = (i < poly2_path.size() - 1) ? poly2_path[i + 1] : poly2_path[0];

Graph::Node* current_node = node_a;
if (!intersection_vec[i].empty()) {
if (intersection_vec[i].size() > 1) {
std::sort(intersection_vec[i].begin(), intersection_vec[i].end(), [&](Graph::Node* n1, Graph::Node* n2) {
return wykobi::distance(current_node->point, n1->point) < wykobi::distance(current_node->point, n2->point);
});
}
for (Graph::Node* next_node : intersection_vec[i]) {
current_node->edges.push_back(next_node);
current_node = next_node;
}
}
current_node->edges.push_back(node_b);
}
return graph;
*/