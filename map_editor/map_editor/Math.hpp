//Author: Sivert Andresen Cubedo
#pragma once

#ifndef Math_HEADER
#define Math_HEADER

//C++ Headers
#include <string>
#include <iostream>
#include <cmath>
#include <limits>
#include <algorithm>
#include <sstream>
#include <memory>
#include <vector>
#include <iterator>

//SFML
#include <SFML\Graphics.hpp>

//wykobi
#include <wykobi.hpp>
#include <wykobi_algorithm.hpp>

//clipper lib
#include <clipper.hpp>

namespace Math {

	const float FloatInf = std::numeric_limits<float>::max();

	const float FloatPI = 3.1415927f;
	const double DoublePI = 3.141592653589793;
	const long double LongDoublePI = 3.141592653589793238L;

	float normalizeFloat(float value, float min, float max);
	float deNormalizeFloat(float value, float min, float max);

	float distanceBetween2Points(wykobi::point2d<float> & p1, wykobi::point2d<float> & p2);

	wykobi::point2d<float> averageCentreOfTriangle2d(wykobi::triangle<float, 2> & triangle);
	wykobi::point2d<float> avarageCentreOfRectangle2d(wykobi::rectangle<float> & rectangle);

	
	
	namespace Clipper {
		/*
		Find first legal "cut" of polygon from existing verticies
		start = start of search
		if return.empty() then no solution was found.
		*/
		std::vector<int> findFirstLegalPolygonCut(wykobi::polygon<float, 2> & poly, int start);

		/*
		Divide polygon it two with two verticies in polygon.
		Segment the two verticies are creating must be fully inside polygon.
		if segment is fully inside then return two polygons in vector.
		if segment is illegal then return empty vector.
		*/
		std::vector<wykobi::polygon<float, 2>> dividePolygonFromExistingPoints(wykobi::polygon<float, 2> & poly, int v1, int v2);

		/*
		Remove Sub Polygons from polygon.
		This will divide the polygon from existing points and return the surviveing polygons.
		*/
		std::vector<wykobi::polygon<float, 2>> removeSubPolygon(wykobi::polygon<float, 2> & poly);

		/*
		Split polygon so hull does not exist.
		return vector of splitted polygons.
		if hull is not inside polygon then return vector with original poly.
		*/
		std::vector<wykobi::polygon<float, 2>> removePolygonHull(wykobi::polygon<float, 2> & poly, wykobi::polygon<float, 2> & hull);
		std::vector<wykobi::polygon<float, 2>> removePolygonHull(wykobi::polygon<float, 2> & poly, std::vector<wykobi::polygon<float, 2>> & hull_vec);
		std::vector<wykobi::polygon<float, 2>> removePolygonHull(std::vector<wykobi::polygon<float, 2>> & poly_vec, std::vector<wykobi::polygon<float, 2>> & hull_vec);

		/*
		Divide triangle with point.
		Make line with one corner and point.
		then intersect line and triangle and make two new triangles and return them.
		if line does not intersect then return only return original triangle.
		*/
		std::vector<wykobi::triangle<float, 2>> divideTriangleWithPoint(wykobi::triangle<float, 2> & tri, wykobi::point2d<float> & point, int corner);

		/*
		Divide triangle with point inside triangle.
		*/
		std::vector<wykobi::triangle<float, 2>> divideTriangleWithPointInside(wykobi::triangle<float, 2> & tri, wykobi::point2d<float> & point);

		/*
		Convert wykobi line to clipper path.
		*/
		ClipperLib::Path wykobiSegmentToClipperPath(wykobi::segment<float, 2> & segment);

		/*
		Convert wykobi polygon to clipper path.
		*/
		ClipperLib::Path wykobiPolygonToClipperPath(wykobi::polygon<float, 2> & poly);

		/*
		Convert clipper path to wykobi polygon.
		*/
		wykobi::polygon<float, 2> clipperPathToWykobiPolygon(ClipperLib::Path & path);

		/*
		Merge polygons together.
		*/
		std::vector<wykobi::polygon<float, 2>> mergePolygons(std::vector<wykobi::polygon<float, 2>> & polygons);
		std::vector<wykobi::polygon<float, 2>> mergePolygons(wykobi::polygon<float, 2> & poly1, wykobi::polygon<float, 2> & poly2);

		/*
		Merge triangles together.
		*/
		std::vector<wykobi::polygon<float, 2>> mergeTriangles(std::vector<wykobi::triangle<float, 2>> & triangles);

		/*
		Clip polygon intersection.
		*/
		std::vector<wykobi::polygon<float, 2>> clipPolygonIntersection(wykobi::polygon<float, 2> & subject, wykobi::polygon<float, 2> & clip);
		std::vector<wykobi::polygon<float, 2>> clipPolygonIntersection(std::vector<wykobi::polygon<float, 2>> & subject, std::vector<wykobi::polygon<float, 2>> & clip);

		/*
		Clip polygon difference.
		*/
		std::vector<wykobi::polygon<float, 2>> clipPolygonDifference(wykobi::polygon<float, 2> & subject, wykobi::polygon<float, 2> & clip);
		std::vector<wykobi::polygon<float, 2>> clipPolygonDifference(std::vector<wykobi::polygon<float, 2>> & subject, std::vector<wykobi::polygon<float, 2>> & clip);

		/*
		Divide triangle in two pieces.
		*/
		std::vector<wykobi::triangle<float, 2>> divideTriangleInTwo(wykobi::triangle<float, 2> & tri, int point);
	}

	
	/*
	Check if segment points are equal regardless of direction.
	*/
	bool isSegmentPointsEqual(wykobi::segment<float, 2> & seg1, wykobi::segment<float, 2> & seg2);

	/*
	Check if segment from two verticies in a polygon is inside polygon.
	*/
	bool isSegmentFromPolygonInsidePolygon(wykobi::polygon<float, 2> & poly, int v1, int v2);

	/*
	Check if segment from vertex in polygon to point is intersecting any segment of the polygon.
	index = point index in polygon.
	*/
	bool isSegmentFromPolygonSelfIntersecting(wykobi::polygon<float, 2> & poly, int index, wykobi::point2d<float> point);

	/*
	Add intersection points to polygon.
	If point is on an edge in polygon then insert point in polygon.
	*/
	wykobi::polygon<float, 2> insertPointsOnEdgesOfPolygon(wykobi::polygon<float, 2> poly, std::vector<wykobi::point2d<float>> points);

	/*
	Merge triangles that share vertices.
	*/
	std::vector<wykobi::polygon<float, 2>> mergeTrianglesSharedVertices(std::vector<wykobi::triangle<float, 2>> & triangles);

	/*
	Insert triangle in polygon if triangle share 2 vertices.
	else return input polygon.
	*/
	wykobi::polygon<float, 2> insertTriangleInPolygonSharedVerticies(wykobi::polygon<float, 2> poly, wykobi::triangle<float, 2> tri);

	/*
	Insert point in polygon at index, points after will pushed up.
	If index is illegal then return input polygon.
	*/
	wykobi::polygon<float, 2> insertPointInPolygon(wykobi::polygon<float, 2> poly, wykobi::point2d<float> & point, int index);

	/*
	Round polygon
	*/
	wykobi::polygon<float, 2> roundWykobiPolygon(wykobi::polygon<float, 2> poly);

	/*
	Round triangle.
	*/
	wykobi::triangle<float, 2> roundWykobiTriangle(wykobi::triangle<float, 2> tri);

	/*
	Round point.
	*/
	wykobi::point2d<float> roundWykobiPoint(wykobi::point2d<float> p);

	/*
	Check if triangles share an edge.
	*/
	bool trianglesSharingEdge(wykobi::triangle<float, 2> & tri1, wykobi::triangle<float, 2> & tri2);

	/*
	Check if triangles are overlapping.
	*/
	bool isTrianglesOverlapping(wykobi::triangle<float, 2> & tri1, wykobi::triangle<float, 2> & tri2);

	/*
	Check if polygons are overlapping.
	*/
	bool isPolygonsOverlapping(wykobi::polygon<float, 2> & poly1, wykobi::polygon<float, 2> & poly2);

	/*
	Is polygon inside polygon.
	*/
	bool isPolygonInsidePolygon(wykobi::polygon<float, 2> & subject, wykobi::polygon<float, 2> & outer);

	/*
	Is triangle inside triangle.
	*/
	bool isTriangleInsideTriangle(wykobi::triangle<float, 2> & subject, wykobi::triangle<float, 2> & outer);

	/*
	Intersect line with rectangle.
	return intersection points.
	*/
	std::vector<wykobi::point2d<float>> intersectLineRectanglePoints(wykobi::line<float, 2> & line, wykobi::rectangle<float> & rect);

	/*
	Triangulate polygon.
	*/
	std::vector<wykobi::triangle<float, 2>> triangulatePolygon(wykobi::polygon<float, 2> poly);

	/*
		Convert wykobi::point2d<float> to sf::vector2f
	*/
	sf::Vector2f wykobiPointToSfVector(wykobi::point2d<float> & p);

	/*
		Convert wykobi::point2d<float> to sf::vertex
	*/
	sf::Vertex wykobiPointToSfVertex(wykobi::point2d<float> & p);

	/*
		Translates p.
	*/
	wykobi::point2d<float> translateWykobiPoint(wykobi::point2d<float> p, float x, float y);

	/*
		Translates vec.
	*/
	std::vector<wykobi::point2d<float>> translateWykobiPointVector(std::vector<wykobi::point2d<float>> vec, float x, float y);

	/*
		Rotates wykoby::point2d<float> around centre.
		p is point to rotate.
		centre is point to rotate around.
		angle is in degrees.
	*/
	wykobi::point2d<float> rotateWykobiPoint(wykobi::point2d<float> p, wykobi::point2d<float> centre, float angle);

	/*
		Rotates points in vector around centre.
		vec is vector with points.
		centre is point to rotate around.
		angle is in radians.
	*/
	std::vector<wykobi::point2d<float>> rotateWykobiPointVector(std::vector<wykobi::point2d<float>> vec, wykobi::point2d<float> centre, float angle);

	/*
		Find shortest rotation from angle1 (start) to angle2 (goal).
		start is in degrees.
		goal is in degrees.
		returns in degrees.
		return is rotation relative to start.
	*/
	float shortestRotation(float start, float goal);

	/*
		Get angle towards point.
		returns in degrees.
	*/
	float angleTowardsPoint(wykobi::point2d<float> origin, wykobi::point2d<float> target);
	
	//Algorithem
	//std::vector<wykobi::segment<float, 2>> delaunayTriangulatePoints(std::vector<wykobi::point2d<float>> & wykobi_points);
	
	/*
	Make vector of integer points in segment.
	*/
	std::vector<std::pair<int, int>> integerLine2d(int x0, int y0, int x1, int y1);

	/*
	Check if polygon is intersectin rectangle.
	*/
	bool isPolygonIntersectRectangle(wykobi::polygon<float, 2> poly, wykobi::rectangle<float> rect);

	/*
	Order points of rect.
	*/
	wykobi::rectangle<float> orderRectangle(wykobi::rectangle<float> rect);

	/*
	Create rectangle from triangle.
	*/
	wykobi::rectangle<float> createRectangleFromTriangle(wykobi::triangle<float, 2> & tri);
	wykobi::rectangle<float> createRectangleFromTriangle(wykobi::triangle<float, 2> & tri, float increase);

	/*
	Create rectangle from polygon.
	*/
	wykobi::rectangle<float> createRectangleFromPolygon(wykobi::polygon<float, 2> & poly);
	wykobi::rectangle<float> createRectangleFromPolygon(wykobi::polygon<float, 2> & poly, float increase);

	/*
	Check if two indexes of points are adjacent to eachother in a shape. 
	*/
	template <typename T>
	bool isPointsInShapeAdjacent(T & shape, int p1, int p2) {
		if (p1 == p2) {
			return false;
		}
		if ((p1 == shape.size() - 1 && p2 == 0) || (p2 == shape.size() - 1 && p1 == 0)) {
			return true;
		}
		int diff = p1 - p2;
		if (diff == -1 || diff == 1) {
			return true;
		}
		return false;
	}

	/*
	Negate values in wykobi point.
	*/
	template <typename T>
	wykobi::point2d<T> negateWykobiPoint(wykobi::point2d<T> point) {
		return wykobi::make_point<T>(-point.x, -point.y);
	}

	/*
	Return points that are on segments that make shape
	*/
	template <typename T>
	std::vector<wykobi::point2d<float>> getWykobiPointsOnEdges(T & obj, std::vector<wykobi::point2d<float>> & points) {
		std::vector<wykobi::point2d<float>> out_points;
		for (int i = 0; i < obj.size(); i++) {
			wykobi::segment<float, 2> seg = wykobi::edge<float>(obj, i);
			for (wykobi::point2d<float> p : points) {
				if (wykobi::point_on_segment<float>(p, seg)) {
					out_points.push_back(p);
				}
			}
		}
		return out_points;
	}

	/*
	Shapes to vector of points.
	*/
	template <typename T>
	std::vector<wykobi::point2d<float>> makeWykobiPointVectorFromShape(std::vector<T> & vec) {
		std::vector<wykobi::point2d<float>> points;
		for (T & obj : vec) {
			for (int i = 0; i < obj.size(); i++) {
				points.push_back(obj[i]);
			}
		}
		return points;
	}
	template <typename T>
	std::vector<wykobi::point2d<float>> makeWykobiPointVectorFromShape(T & shape) {
		std::vector<T> vec;
		vec.push_back(shape);
		return makeWykobiPointVectorFromShape<T>(vec);
	}

	/*
	Count and return number of equal elements in two vectors of same type of equal size.
	Can not have duplicates in same lists.
	*/
	template <typename T>
	int numberOfEqualElements(std::vector<T> & vec1, std::vector<T> & vec2) {
		if (vec1.size() != vec2.size()) {
			return -1;
		}
		int count = 0;
		for (int i = 0; i < vec1.size(); i++) {
			for (int j = 0; j < vec2.size(); j++) {
				if (vec1[i] == vec2[j]) {
					count++;
					break;
				}
			}
		}
		return count;
	}

	/*
	Remove duplicate points.
	ONLY WORKS ON WYKOBI OBJECTS?!?!?!.
	*/
	template <typename Object>
	Object removeDuplicates(Object obj) {
		int i = 0;
		while (i < obj.size()) {
			int j = i + 1;
			while (j != obj.size()) {
				if (obj[j] == obj[i]) {
					obj.erase(j);
				}
				else {
					j++;
				}
			}
			i++;
		}
		return obj;
	}

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
				void eraseEdge(Edge* e) {
					edges.erase(std::find(edges.begin(), edges.end(), e));
				}
			};
			class Edge {
			public:
				Node* a;
				Node* b;
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
		Input:
		graph:	EdgeGraph<wykobi::point2d<T>>
		Return:
		std::vector<wykobi::segment<T, 2>>
		*/
		template<typename T>
		std::vector<wykobi::segment<T, 2>> getSegmentsFromEdgeGraph(EdgeGraph<wykobi::point2d<T>> & graph) {
			std::vector<wykobi::segment<T, 2>> out_vec;
			out_vec.reserve(graph.edges.size());
			for (std::unique_ptr<EdgeGraph<wykobi::point2d<T>>::Edge> & pointer : graph.edges) {
				EdgeGraph<wykobi::point2d<T>>::Edge* edge = pointer.get();
				out_vec.push_back(wykobi::make_segment<T>(edge->a->obj, edge->b->obj));
			}
			return out_vec;
		}


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

	namespace Debug {
		/*
		String contents.
		*/
		std::string toString(wykobi::point2d<float> & point);
		std::string toString(wykobi::segment<float, 2> & seg);
		std::string toString(wykobi::polygon<float, 2> & poly);
		std::string toString(wykobi::triangle<float, 2> & tri);
		std::string toString(wykobi::rectangle<float> & rect);

		/*
		String code
		*/
		std::string toCode(wykobi::point2d<float> & point);
		std::string toCode(wykobi::polygon<float, 2> & poly);
		std::string toCode(wykobi::triangle<float, 2> & tri);
	}

};


#endif // !Math_HEADER
