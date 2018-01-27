//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MergeTriangles_HEADER
#define MergeTriangles_HEADER

#include <vector>
#include <map>
#include <memory>

#include "EditorPath.hpp"
#include "EditorTriangle.hpp"

namespace MergeTriangles {
	/*
	Merge triangles to EditorPath
	*/
	EditorPath mergeTriangle(std::vector<EditorTriangle> & triangle_vec);
	
	struct Node;
	struct Graph;

	struct Node {
		MapPoint* m_point;
		std::vector<Node*> m_edges;
		std::vector<Node*> m_in_edges;
		bool m_visited = false;

		/*
		Constructor
		*/
		Node(MapPoint* point_ptr);

		/*
		Add edge
		*/
		void addEdge(Node* n);
	};

	struct Graph {
		std::vector<std::unique_ptr<Node>> m_nodes;

		/*
		Constructor
		This will build the graph in the order or the triangles in the vector
		if a triangle is not connected to the first triangle in the vector then it will be dropped
		*/
		Graph(std::vector<EditorTriangle> & triangles);

		/*
		Traverse outer path and find hulls
		return EditorPath containing outer path and hulls
		*/

	};
}

#endif // !MergeTriangles_HEADER


//end