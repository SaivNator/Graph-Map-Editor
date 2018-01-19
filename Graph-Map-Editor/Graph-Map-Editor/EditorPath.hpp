//Author: Sivert Andresen Cubedo
#pragma once

#ifndef EditorPath_HEADER
#define EditorPath_HEADER

#include <vector>
#include <map>
#include <algorithm>
#include <functional>

#include <iostream>
#include <sstream>

#include <wykobi.hpp>

#include "MapPoint.hpp"
#include "EditorTriangle.hpp"

class EditorPath : public std::vector<MapPoint*> {
	std::vector<EditorPath> m_hulls;

public:
	/*
	Check if path and triangle has a shared edge
	Return vector of pairs of iterators that make up edges
	Return empty vector if no shared edge is found
	*/
	std::vector<std::pair<EditorPath::iterator, EditorPath::iterator>> sharedEdge(EditorTriangle & triangle);

	/*
	Triangulate path
	*/
	std::vector<EditorTriangle> triangulate();

	/*
	Make triangle from path and index
	*/
	EditorTriangle vertexTriangle(const std::size_t index);

	/*
	Check if vertex is ear (for triangulation)
	*/
	bool vertexIsEar(const std::size_t index);

	/*
	Check if vertex is convex
	*/
	bool convexVertex(const std::size_t index, const int wykobi_orientation);

	/*
	Get wykobi path orientation
	see wykobi::orientation
	*/
	int orientation();

	/*
	Reverse objects in container
	*/
	void reverse();

	/*
	Get hulls
	*/
	std::vector<EditorPath> & getHulls();

	/*
	Remove hulls
	Return paths without hulls that represent this path
	If no hulls then return copy of this
	*/
	std::vector<EditorPath> removeHull();

	/*
	TO STRING FOR DEBUG
	*/
	std::string toString();


private:
	struct Edge;
	struct Node;
	struct Graph;

	struct Edge {
		Node* m_a;
		Node* m_b;

		bool m_is_bridge = false;
		bool m_visited = false;

		/*
		Constructor
		*/
		Edge(Node* a, Node* b);

		/*
		Get connected node
		*/
		Node* getNode(Node* n);

		/*
		Check if next edge has been traversed
		*/
		bool clockVisit(Node* n);
		bool counterClockVisit(Node* n);
	};

	struct Node {
		MapPoint* m_point;
		std::vector<Edge*> m_edges;

		Node(MapPoint* point_ptr);

		wykobi::point2d<float> getPos();

		Edge* getClockwiseMost(Edge* prev_edge);

		Edge* getCounterClockwiseMost(Edge* prev_edge);
	};

	struct Graph {
		std::vector<std::unique_ptr<Node>> m_node_vec;
		std::vector<Node*> m_outer_path;
		std::vector<std::vector<Node*>> m_hull_vec;
		std::vector<std::unique_ptr<Edge>> m_edge_vec;

		Graph(EditorPath & path);

		void addEdge(Node* a, Node* b);

		/*
		Return true if edge between a and b in not intersectiong
		any edges exept the edges that is connected to a and b.
		*/
		bool isEdgeLegal(Node* a, Node* b);

		/*
		Traverse bridge
		*/
		std::vector<Node*> traverseBridgeClockwise(Edge* edge, Node* start_node);
		std::vector<Node*> traverseBridgeCounterClockwise(Edge* edge, Node* start_node);

		EditorPath makeEditorPath(std::vector<Node*> node_vec);
	};



};

#endif // !


//end