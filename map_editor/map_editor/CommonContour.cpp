//Author: Sivert Andresen Cubedo

#include "CommonContour.hpp"

using namespace CommonContour;

void CommonContour::insertPathToGraph(std::vector<Graph::Node*> & path) {
	std::vector<Graph::Node*>::iterator it = path.begin();
	while (it != path.end()) {
		Graph::Node* outer_node_a = (*it);
		Graph::Node* outer_node_b = (std::next(it) != path.end()) ? (*std::next(it)) : (*path.begin());
		outer_node_a->addEdge(outer_node_b);
		++it;
	}
}



std::vector<wykobi::polygon<float, 2>> CommonContour::clipDifference(wykobi::polygon<float, 2> subject_poly, wykobi::polygon<float, 2> clip_poly) {
	std::vector<wykobi::polygon<float, 2>> out_vec;
	Graph graph = makeGraphFromPolygons(subject_poly, clip_poly);

	std::vector<std::vector<Graph::Node*>> path_vec = clipDifference(graph);

	for (auto & p : path_vec) {
		out_vec.push_back(pathToWykobiPolygon(p));
	}
	
	return out_vec;
}

std::vector<std::vector<Graph::Node*>> CommonContour::clipDifference(Graph & graph) {

	std::vector<Graph::Node*> & subject_path = graph.subject_path;
	std::vector<Graph::Node*> & clip_path = graph.clip_path;

	wykobi::polygon<float, 2> subject_path_poly = pathToWykobiPolygon(subject_path);
	wykobi::polygon<float, 2> clip_path_poly = pathToWykobiPolygon(clip_path);

	//remove nodes from subject_path that are inside clip_path, but not part of clip_path
	auto it = subject_path.begin();
	while (it != subject_path.end()) {
		Graph::Node* n = (*it);
		if (std::find(clip_path.begin(), clip_path.end(), n) == clip_path.end() && wykobi::point_in_polygon(n->point, clip_path_poly)) {
			it = subject_path.erase(it);
			graph.removeNode(n);
		}
		else {
			++it;
		}
	}

	//remove edges from clip_path that are part of clip_path, but are not in the order of the clip_path
	//it = clip_path.begin();
	//while (it != clip_path.end()) {
	//	Graph::Node* n = (*it);
	//	Graph::Node* correct_edge = (it + 1 != clip_path.end()) ? (*(it + 1)) : (*clip_path.begin());
	//	std::vector<Graph::Node*> edges_to_remove;
	//	for (Graph::Node* e : n->edges) {
	//		if (e != correct_edge && std::find(clip_path.begin(), clip_path.end(), e) != clip_path.end()) {
	//			edges_to_remove.push_back(e);
	//		}
	//	}
	//	for (Graph::Node* e : edges_to_remove) {
	//		n->removeEdge(e);
	//	}
	//	++it;
	//}

	//remove nodes from clip_path that are not inside subject_path, and are not part of subject_path
	it = clip_path.begin();
	while (it != clip_path.end()) {
		Graph::Node* n = (*it);
		if (std::find(subject_path.begin(), subject_path.end(), n) == subject_path.end() && !wykobi::point_in_polygon(n->point, subject_path_poly)) {
			it = clip_path.erase(it);
			graph.removeNode(n);
		}
		else {
			++it;
		}
	}
	
	//(at this point, subject_path and clip_path are both invalid)
	
	//create new independent paths
	std::vector<std::vector<Graph::Node*>> path_vec;
	while (true) {
		//find node with more then 2 out edges, and is not visited
		Graph::Node* start_node = nullptr;
		for (Graph::Node* n : clip_path) {
			if (n->edges.size() > 1 && !n->visited) {
				start_node = n;
				break;
			}
		}
		if (start_node == nullptr) break;
		
		start_node->visited = true;
		std::vector<Graph::Node*> search_path;

		search_path.push_back(start_node);

		//search until in_edges.size() > 1 then go in_edges back the other "way"
		Graph::Node* current_node = start_node->edges.front();
		Graph::Node* last_node = start_node;
		Graph::Node* next_node = nullptr;
		while (current_node->in_edges.size() == 1) {
			search_path.push_back(current_node);
			last_node = current_node;
			current_node = current_node->edges.front();
		}
		search_path.push_back(current_node);
		next_node = (current_node->in_edges.front() != last_node) ? current_node->in_edges.front() : current_node->in_edges.back();
		current_node = next_node;
		while (current_node != start_node) {
			search_path.push_back(current_node);
			last_node = current_node;
			current_node = current_node->in_edges.front();
		}
		path_vec.push_back(search_path);
	}

	//clear all edges and reapply according to paths
	graph.clearEdges();
	for (auto & path : path_vec) {
		insertPathToGraph(path);
	}

	//reverse the direction of the edges that are left in clip_path
	//std::vector<std::pair<Graph::Node*, Graph::Node*>> edges_to_reverse;
	//for (Graph::Node* n : clip_path) {
	//	for (Graph::Node* e : n->edges) {
	//		if (std::find(clip_path.begin(), clip_path.end(), e) != clip_path.end()) {
	//			edges_to_reverse.push_back(std::pair<Graph::Node*, Graph::Node*>(n, e));
	//		}
	//	}
	//}
	//for (std::pair<Graph::Node*, Graph::Node*> & pair : edges_to_reverse) {
	//	Graph::Node* n = pair.first;
	//	Graph::Node* e = pair.second;
	//
	//	n->removeEdge(e);
	//	e->addEdge(n);
	//}
	return path_vec;
}

wykobi::vector2d<float> CommonContour::perp(wykobi::vector2d<float> & vector) {
	return wykobi::make_vector<float>(vector.y, -vector.x);
}

std::vector<Graph::Node*> CommonContour::insertNodesOnPath(std::vector<Graph::Node*> & path, std::vector<std::vector<Graph::Node*>> & nodes) {
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

std::vector<std::vector<Graph::Node*>> CommonContour::findHull(Graph & graph) {
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
	graph.clearVisited();
	return hull_vec;
}

Graph CommonContour::makeGraphFromPolygons(wykobi::polygon<float, 2> subject_poly, wykobi::polygon<float, 2> clip_poly) {
	//make sure polys has same orientation
	if (wykobi::polygon_orientation(subject_poly) != wykobi::CounterClockwise) {
		subject_poly.reverse();
	}
	if (wykobi::polygon_orientation(clip_poly) != wykobi::CounterClockwise) {
		clip_poly.reverse();
	}
	Graph graph;
	std::vector<Graph::Node*> poly1_path;
	std::vector<Graph::Node*> poly2_path;
	for (std::size_t i = 0; i < subject_poly.size(); ++i) {
		poly1_path.push_back(graph.makeNode(subject_poly[i]));
	}
	for (std::size_t i = 0; i < clip_poly.size(); ++i) {
		auto it = std::find_if(poly1_path.begin(), poly1_path.end(), [&](Graph::Node* n1) { return n1->point == clip_poly[i]; });
		if (it != poly1_path.end()) {
			poly2_path.push_back((*it));
		}
		else {
			poly2_path.push_back(graph.makeNode(clip_poly[i]));
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
	insertPathToGraph(poly1_path);
	insertPathToGraph(poly2_path);
	//std::vector<Graph::Node*>::iterator it_1 = poly1_path.begin();
	//while (it_1 != poly1_path.end()) {
	//	Graph::Node* outer_node_a = (*it_1);
	//	Graph::Node* outer_node_b = (std::next(it_1) != poly1_path.end()) ? (*std::next(it_1)) : (*poly1_path.begin());
	//	outer_node_a->addEdge(outer_node_b);
	//	++it_1;
	//}
	//it_1 = poly2_path.begin();
	//while (it_1 != poly2_path.end()) {
	//	Graph::Node* outer_node_a = (*it_1);
	//	Graph::Node* outer_node_b = (std::next(it_1) != poly2_path.end()) ? (*std::next(it_1)) : (*poly2_path.begin());
	//	outer_node_a->addEdge(outer_node_b);
	//	++it_1;
	//}

	graph.subject_path = poly1_path;
	graph.clip_path = poly2_path;

	return graph;
}

std::vector<wykobi::segment<float, 2>> CommonContour::getWykobiSegmentsFromGraph(Graph & graph) {
	std::vector<wykobi::segment<float, 2>> out_vec;
	for (std::size_t i = 0; i < graph.nodes.size(); ++i) {
		Graph::Node* n = graph.nodes[i].get();
		for (Graph::Node* e : n->edges) {
			out_vec.push_back(wykobi::make_segment<float>(n->point, e->point));
		}
	}
	return out_vec;
}

wykobi::polygon<float, 2> CommonContour::pathToWykobiPolygon(std::vector<Graph::Node*> & path) {
	wykobi::polygon<float, 2> poly;
	poly.reserve(path.size());
	for (Graph::Node* n : path) {
		poly.push_back(n->point);
	}
	return poly;
}

std::vector<Graph::Node*> CommonContour::traverseUnion(Graph & graph) {
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

std::vector<wykobi::polygon<float, 2>> CommonContour::clipUnion(wykobi::polygon<float, 2> subject_poly, wykobi::polygon<float, 2> clip_poly) {
	//std::vector<wykobi::segment<float, 2>> mergeUnion(wykobi::polygon<float, 2> poly1, wykobi::polygon<float, 2> poly2) {
	std::vector<wykobi::polygon<float, 2>> poly_vec;
	if (Math::isPolygonInsidePolygon(subject_poly, clip_poly)) {
		poly_vec.push_back(clip_poly);
		return poly_vec;
	}
	else if (Math::isPolygonInsidePolygon(clip_poly, subject_poly)) {
		poly_vec.push_back(subject_poly);
		return poly_vec;
	}
	else if (!Math::isPolygonsOverlapping(subject_poly, clip_poly)) {
		poly_vec.push_back(subject_poly);
		poly_vec.push_back(clip_poly);
		return poly_vec;
	}
	//make sure both polygons are clockwise
	if (wykobi::polygon_orientation(subject_poly) != wykobi::CounterClockwise) {
		subject_poly.reverse();
	}
	if (wykobi::polygon_orientation(clip_poly) != wykobi::CounterClockwise) {
		clip_poly.reverse();
	}
	Graph graph = makeGraphFromPolygons(subject_poly, clip_poly);
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


//end



/*
//std::vector<std::unique_ptr<Graph>> CommonContour::splitGraph(Graph & graph) {
Graph CommonContour::splitGraph(Graph & graph) {
std::vector<std::vector<Graph::Node*>> sub_graph_nodes_vec;

//std::vector<std::unique_ptr<Graph>> graph_vec;

while (true) {
//get node
std::list<Graph::Node*> dfs_stack;
for (auto it = graph.nodes.begin(); it != graph.nodes.end(); ++it) {
Graph::Node* n = (*it).get();
if (!n->visited) {
dfs_stack.push_back(n);
break;
}
}
if (dfs_stack.empty()) break;

std::vector<Graph::Node*> current_sub_graph;

//dfs
while (!dfs_stack.empty()) {
Graph::Node* n = dfs_stack.back();
dfs_stack.pop_back();
if (!n->visited) {
n->visited = true;
current_sub_graph.push_back(n);
for (Graph::Node* e : n->edges) {
dfs_stack.push_back(e);
}
}
}
sub_graph_nodes_vec.push_back(current_sub_graph);
}



//make subgraphs
//copy nodes
//store aliases (pair of original node and copyed node)
//copy edges
//for (std::vector<Graph::Node*> & sub_nodes : sub_graph_nodes_vec) {

std::cout << "Hello 1\n";

//graph_vec.push_back(std::unique_ptr<Graph>(new Graph));
Graph sub_graph;
std::unordered_map<Graph::Node*, Graph::Node*> node_alias;
for (Graph::Node* origin_node : sub_graph_nodes_vec.front()) {
Graph::Node* clone_node = sub_graph.makeNode(origin_node->point);
node_alias.emplace(origin_node, clone_node);
}

std::cout << "Hello 2\n";

for (auto it_1 = node_alias.begin(); it_1 != node_alias.end(); ++it_1) {
Graph::Node* origin_node = it_1->first;
Graph::Node* clone_node = it_1->second;
for (Graph::Node* origin_edge : origin_node->edges) {
Graph::Node* clone_edge = node_alias.find(origin_edge)->second;
clone_node->addEdge(clone_edge);
std::cout << "Hello 3\n";
}
}
std::cout << "Hello 4\n";
//}



graph.clearVisited();

return sub_graph;
}
*/

