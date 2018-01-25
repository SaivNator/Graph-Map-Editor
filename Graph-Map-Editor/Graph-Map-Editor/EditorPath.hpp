//Author: Sivert Andresen Cubedo
#pragma once

#ifndef EditorPath_HEADER
#define EditorPath_HEADER

#include <vector>
#include <list>
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
	struct Hull;
	struct Graph;

	struct Edge {
		Node* m_a;
		Node* m_b;

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

	struct Path : public std::vector<Node*> {
		wykobi::point2d<float> centroid();
	};

	struct Hull : public Path {
		std::size_t m_connect_count = 0;
		std::vector<Node*> m_exclude;
	};

	struct Graph {
		std::vector<std::unique_ptr<Node>> m_node_vec;
		Path m_outer_path;
		std::vector<Hull> m_hull_vec;
		std::vector<std::unique_ptr<Edge>> m_edge_vec;

		std::vector<Hull*> m_connected_hulls;	//BEEP BOOP
		std::vector<Edge*> m_hull_bridge_vec;

		/*
		Constructor
		*/
		Graph(EditorPath & path);

		/*
		Add edge
		*/
		void addEdge(Node* a, Node* b);

		/*
		Return true if edge between a and b in not intersectiong
		any edges exept the edges that is connected to a and b.
		*/
		bool isEdgeLegal(Node* a, Node* b);

		/*
		Traverse bridge
		*/
		Path traverseBridgeClockwise(Edge* edge, Node* start_node);
		Path traverseBridgeCounterClockwise(Edge* edge, Node* start_node);

		/*
		Make editor path
		*/
		EditorPath makeEditorPath(Path node_vec);
	
		/*
		Return two points that are close to each other in each path,
		not guaranteed to be the closest points 
		*/
		std::pair<Path::iterator, Path::iterator> closestNodes(Path & path_1, Path & path_2);
		
		/*
		Try to connect two Paths
		Fill test if legal
		Return: 
			Edge* if successful
			nullptr if unsuccessful
		*/
		Edge* connectPath(Path & p1, Path & p2);
		Edge* connectPath(Path & p1, Path & p2, std::vector<Node*> & exclude);

		bool connectHull(Hull & hull);
	};



};

#endif // !


//end