//Author: Sivert Andresen Cubedo
#pragma once

//C++ Headers
#include <string>
#include <iostream>
#include <cmath>
#include <limits>
#include <algorithm>
#include <sstream>
#include <memory>
#include <vector>
#include <list>
#include <iterator>

//wykobi
#include <wykobi.hpp>
#include <wykobi_algorithm.hpp>

//local
#include "Math.hpp"

namespace Graph {
	/*
	Generic Edge Graph class
	*/
	template <typename T>
	class EdgeGraph {
	public:
		class Node;
		class Edge;
		class Node {
		public:
			std::vector<Edge*> edges;
			T obj;
			int visited = 0;
			void eraseEdge(Edge* e) {
				edges.erase(std::find(edges.begin(), edges.end(), e));
			}
		};
		class Edge {
		public:
			Node* a;
			Node* b;
			int visited = 0;
		};

		std::vector<std::unique_ptr<Node>> nodes;
		std::vector<std::unique_ptr<Edge>> edges;

		//Make Edge
		Edge* makeEdge(Node* a, Node* b) {
			edges.push_back(std::unique_ptr<Edge>(new Edge()));
			Edge* e = edges.back().get();
			e->a = a;
			e->b = b;
			a->edges.push_back(e);
			b->edges.push_back(e);
			return e;
		}
		Edge* makeEdge(int a, int b) {
			return makeEdge(nodes[a].get(), nodes[b].get());
		}

		//Make Node
		Node* makeNode(Node n) {
			nodes.push_back(std::unique_ptr<Node>(new Node(n)));
			return nodes.back().get();
		}
		Node* makeNode(T obj) {
			Node n;
			n.obj = obj;
			return makeNode(n);
		}
		Node* makeNode(T obj, std::vector<int> edges) {
			Node* n = makeNode(obj);
			for (int node_index : edges) {
				Node* = nodes[node_index].get();
				makeEdge(n, point_node);
			}
			return n;
		}
		Node* makeNode(T obj, std::vector<Node*> edges) {
			Node* n = makeNode(obj);
			for (Node* point_node : edges) {
				makeEdge(n, point_node);
			}
			return n;
		}

		//Delete Edge
		void deleteEdge(typename std::vector<std::unique_ptr<Edge>>::iterator it) {
			Edge* e = (*it).get();
			e->a->eraseEdge(e);
			e->b->eraseEdge(e);
			edges.erase(it);
		}
		void deleteEdge(int i) {
			deleteEdge(edges.begin() + i);
		}
		void deleteEdge(Edge* e) {
			deleteEdge(std::find_if(edges.begin(), edges.end(), [&](std::unique_ptr<Edge> & p) { return p.get() == e; }));
		}

		//Delete Node
		void deleteNode(typename std::vector<std::unique_ptr<Node>>::iterator it) {
			Node* n = (*it).get();
			for (Edge* e : n->edges) {
				deleteEdge(e);
			}
			nodes.erase(it);
		}
		void deleteNode(int i) {
			deleteNode(nodes.begin() + i);
		}
		void deleteNode(Node* n) {
			deleteNode(std::find_if(nodes.begin(), nodes.end(), [&](std::unique_ptr<Node> & p) { return p.get() == n; }));
		}
	};
	/*
	Make EdgeGraph from nodes
	Input:
	nodes:	vector of nodes
	edges:	vector of edges (corresponds with index of nodes)
	Return:
	EdgeGraph<T>
	*/
	template <typename T>
	EdgeGraph<T> makeEdgeGraphFromNodes(std::vector<T> & nodes, std::vector<std::pair<int, int>> & edges) {
		EdgeGraph<T> graph;
		for (int i = 0; i < nodes.size(); i++) {
			graph.makeNode(nodes[i]);
		}
		for (int i = 0; i < edges.size(); i++) {
			graph.makeEdge(edges[i].first, edges[i].second);
		}
		return graph;
	}
	/*
	EdgeGraph<wykobi::point2d<T>> to vector of wykobi::segment<T, 2>
	On a 2d plane
	Input:
	graph:	EdgeGraph<wykobi::point2d<T>>
	Return:
	std::vector<wykobi::segment<T, 2>>
	*/
	template <typename T>
	std::vector<wykobi::segment<T, 2>> getWykobiSegmentsFromEdgeGraph(EdgeGraph<wykobi::point2d<T>> & graph) {
		std::vector<wykobi::segment<T, 2>> out_vec;
		out_vec.reserve(graph.edges.size());
		for (std::unique_ptr<EdgeGraph<wykobi::point2d<T>>::Edge> & pointer : graph.edges) {
			EdgeGraph<wykobi::point2d<T>>::Edge* edge = pointer.get();
			out_vec.push_back(wykobi::make_segment<T>(edge->a->obj, edge->b->obj));
		}
		return out_vec;
	}
	/*
	EdgeGraph<wykobi::point2d<T>> to vector of wykobi::polygon<T, 2>
	On a 2d plane
	Input:
	graph:	EdgeGraph<wykobi::point2d<T>>
	Return:
	std::vector<wykobi::polygon<T, 2>>
	*/
	std::vector<wykobi::polygon<float, 2>> getWykobiPolygonsFromEdgeGraph(EdgeGraph<wykobi::point2d<float>> & graph);
	//template <typename T>
	//std::vector<wykobi::polygon<T, 2>> getWykobiPolygonsFromEdgeGraph(EdgeGraph<wykobi::point2d<T>> & graph) {
	//	/*
	//	An edge can only be part of one(edge of graph) or two polygons(inside)
	//	Psuedo code:
	//		find the left-top vertex in graph
	//	*/
	//	typedef EdgeGraph<wykobi::point2d<T>>::Node Node;
	//	typedef EdgeGraph<wykobi::point2d<T>>::Edge Edge;
	//	std::vector<wykobi::polygon<T, 2>> out_vec;
	//	//find left-top vertex
	//	{
	//		float min_x = 0;
	//		float min_y = 0;
	//		for (int i = 0; i < graph.nodes.size(); i++) {
	//			if (graph.nodes[i]->)
	//		}
	//	}
	//	//std::list<Node*> current_path;
	//	//std::list<Node*> pending_nodes;
	//	//Node* current_start;
	//	//Node* current_node;
	//	//
	//	//pending_nodes.push_back(graph.nodes.front().get());
	//	//while (!pending_nodes.empty()) {
	//	//	current_path.clear();
	//	//	current_start = pending_nodes.front();
	//	//	pending_nodes.pop_front();
	//	//	current_path.push_back(current_start);
	//	//	//while (curr)
	//	//}
	//	return out_vec;
	//}

	///*
	//Generic Graph class
	//*/
	//template <typename T>
	//class Graph {
	//	class Node {
	//	public:
	//		std::vector<Node*> edges;
	//		T obj;
	//	};
	//	std::vector<std::unique_ptr<Node>> nodes;
	//public:
	//	Node* makeNode(Node n) {
	//		nodes.push_back(std::unique_ptr<Node>(new Node(n)));
	//		return nodes.back().get();
	//	}
	//	void makeNode(T obj) {
	//		Node n;
	//		n.obj = obj;
	//		return makeNode(n);
	//	}
	//	void makeNode(T obj, std::vector<int> & edges) {
	//		Node n;
	//		n.obj = obj;
	//		n.edges.reserve(edges.size());
	//		for (int e : edges) {
	//			n.edges.push_back(nodes[e]);
	//		}
	//		return makeNode(n);
	//	}
	//	void makeNode(T obj, std::vector<Node*> edges) {
	//		Node n;
	//		n.obj = obj;
	//		n.edges = edges;
	//		return makeNode(n);
	//	}
	//};
	///*
	//Make EdgeGraph from nodes
	//Input:
	//nodes:	outer vector 'node' (the index describes the id)
	//first in pair is obj of type T
	//second is a vector of edges pointing to other nodes (indexes of outer vector)
	//Return:
	//EdgeGraph<T>
	//*/

}

//end
