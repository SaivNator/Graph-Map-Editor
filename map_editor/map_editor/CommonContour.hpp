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

	/*
	Graph container
	*/
	struct Graph {
		struct Node {
			//out_edges
			std::vector<Node*> edges;

			std::vector<Node*> in_edges;

			wykobi::point2d<float> point;
			bool visited = false;
			void addEdge(Node* e) {
				if (std::find(edges.begin(), edges.end(), e) == edges.end()) {
					edges.push_back(e);
					e->in_edges.push_back(this);
				}
			}
			void removeEdge(Node* e) {
				auto it = std::find(edges.begin(), edges.end(), e);
				if (it != edges.end()) {
					edges.erase(it);
					e->in_edges.erase(std::find(e->in_edges.begin(), e->in_edges.end(), this));
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
		void removeNode(Node* n) {
			for (Node* e : n->edges) {
				n->removeEdge(e);
			}
			for (Node* i_e : n->in_edges) {
				i_e->removeEdge(n);
			}
			auto it = std::find_if(nodes.begin(), nodes.end(), [&](std::unique_ptr<Node> & ptr) {return ptr.get() == n;});
			nodes.erase(it);
		}
		void resetVisited() {
			for (auto & n : nodes) {
				n->visited = false;
			}
		}
	};

	/*
	Perpendicular or someshit?!?!?
	*/
	wykobi::vector2d<float> perp(wykobi::vector2d<float> & vector);

	/*
	Find hull
	*/
	std::vector<std::vector<Graph::Node*>> findHull(Graph & graph);

	/*
	Traverse outside of graph
	*/
	std::vector<Graph::Node*> traverseUnion(Graph & graph);

	/*
	Remove nodes in path from graph
	*/
	void removePathFromGraph(Graph & graph, std::vector<Graph::Node*> subject_path, std::vector<Graph::Node*> clip_path);
	
	/*
	Return vector of edges as segments from graph
	*/
	std::vector<wykobi::segment<float, 2>> getWykobiSegmentsFromGraph(Graph & graph);
	
	/*
	Return wykobi polygon of path
	*/
	wykobi::polygon<float, 2> pathToWykobiPolygon(std::vector<Graph::Node*> & path);
	
	/*
	Return path with nodes inserted
	*/
	std::vector<Graph::Node*> insertNodesOnPath(std::vector<Graph::Node*> & path, std::vector<std::vector<Graph::Node*>> & nodes);
	
	/*
	Return graph constructed from poly1 and poly2
	*/
	Graph makeGraphFromPolygons(wykobi::polygon<float, 2> poly1, wykobi::polygon<float, 2> poly2);
	
	/*
	Clip diffrence
	*/
	std::vector<wykobi::polygon<float, 2>> clipDifference(wykobi::polygon<float, 2> poly1, wykobi::polygon<float, 2> poly2);
	
	/*
	Clip union
	*/
	std::vector<wykobi::polygon<float, 2>> clipUnion(wykobi::polygon<float, 2> poly1, wykobi::polygon<float, 2> poly2);
	
	/*
	Get most clockwise node
	*/
	Graph::Node* getClockwiseMost(Graph::Node* prev_node, Graph::Node* current_node);
	
	/*
	Get most counterclockwise node
	*/
	Graph::Node* getCounterClockwiseMost(Graph::Node* prev_node, Graph::Node* current_node);
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