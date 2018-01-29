//Author: Sivert Andresen Cubedo
#pragma once

#ifndef RemoveHull_HEADER
#define RemoveHull_HEADER

#include <vector>

#include <wykobi.hpp>

#include "EditorPath.hpp"

class EditorPath;

namespace RemoveHull {
	/*
	Remove hull from path
	*/
	std::vector<EditorPath> removeHull(EditorPath & path);

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
		Check if next clockwise edge has been traversed
		*/
		bool clockVisit(Node* n);

		/*
		Check if next counter clockwise edge has been traversed
		*/
		bool counterClockVisit(Node* n);
	};

	struct Node {
		MapPoint* m_point;
		std::vector<Edge*> m_edges;

		/*
		Constructor
		*/
		Node(MapPoint* point_ptr);

		/*
		Get pos
		*/
		wykobi::point2d<float> getPos();

		/*
		Get clockwise mose
		*/
		Edge* getClockwiseMost(Edge* prev_edge);
		
		/*
		Get counter clockwise most
		*/
		Edge* getCounterClockwiseMost(Edge* prev_edge);
	};

	struct Path : public std::vector<Node*> {
		/*
		Get centroid
		*/
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

		/*
		Try to connect hull to outer path or to a hull connected to outer path
		*/
		bool connectHull(Hull & hull);
	};
}

#endif // !RemoveHull_HEADER


//end