//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MergeTriangles_HEADER
#define MergeTriangles_HEADER

#include <vector>
#include <map>
#include <memory>

#include <wykobi.hpp>

#include "MapPoint.hpp"
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
		//std::vector<Node*> m_in_edges;
		bool m_is_outer = false;
		bool m_visited = false;
		bool m_is_searched = false;

		/*
		Constructor
		*/
		Node(MapPoint* point_ptr);

		/*
		Add edge
		*/
		void addEdge(Node* n);

		/*
		Get pos
		*/
		wykobi::point2d<float> getPos();

		/*
		Get clockwisemost edge
		*/
		Node* getClockwiseMost(Node* prev_node);

		/*
		Get counterclockwisemost edge
		*/
		Node* getCounterClockwiseMost(Node* prev_node);
	};

	struct Graph {
		std::vector<std::unique_ptr<Node>> m_nodes;

		/*
		Constructor
		This will build the graph in the order or the triangles in the vector
		NOT IMPLEMENTED -> if a triangle is not connected to the first triangle in the vector then it will be dropped
		*/
		Graph(std::vector<EditorTriangle> & triangles);

		/*
		Traverse outer path and find hulls
		return EditorPath containing outer path and hulls
		*/
		EditorPath traverse();
	
		/*
		Find hulls
		*/
		std::vector<EditorPath> findHull();
	};
}

#endif // !MergeTriangles_HEADER


//end