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
#include <list>
#include <iterator>

//SFML
#include <SFML\Graphics.hpp>

//wykobi
#include <wykobi.hpp>
#include <wykobi_algorithm.hpp>

//clipper lib
#include <clipper.hpp>

namespace Math {
	/*
	TODO: make generic
	*/
	float normalizeFloat(float value, float min, float max);
	
	/*
	TODO: make generic
	*/
	float deNormalizeFloat(float value, float min, float max);

	/*
	TODO: make generic
	*/
	float distanceBetween2Points(wykobi::point2d<float> & p1, wykobi::point2d<float> & p2);

	/*
	TODO: make generic
	*/
	wykobi::point2d<float> averageCentreOfTriangle2d(wykobi::triangle<float, 2> & triangle);
	
	/*
	TODO: make generic
	*/
	wykobi::point2d<float> avarageCentreOfRectangle2d(wykobi::rectangle<float> & rectangle);
	
	/*
	Contains functions to clip shapes
	(makes use of clipper.hpp)
	(!!!verticies will be rounded to nearest integer!!!)
	*/
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
	Check if two indexes of points are adjacent in a shape.
	template:
		T:		wykobi shape type
	input:
		shape:	wykobi shape
		p1:		index 1
		p2:		index 2
	return:
		if points are adjacent or not

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
	template:
		T:		basic type
	input:
		point:	wykobi::point2d<T>
	return:
		negated point
	*/
	template <typename T>
	wykobi::point2d<T> negateWykobiPoint(wykobi::point2d<T> & point) {
		return wykobi::make_point<T>(-point.x, -point.y);
	}

	/*
	Return points that are on segments that make shape
	template:
		T:	wykobi shape
	input:
		obj:	wykobi shape
		points:	vector of wykobi::point2d<float>
	return:
		vector of points
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

	/*
	Find shortest rotation from angle1 (start) to angle2 (goal).
	input:
		start:	start angle
		goal:	goal angle
		max:	maximum degrees (default 360)
	return:
		shortes angle towards goal from start
	*/
	template <typename T>
	T shortestRotation(T start, T goal, T max) {
		T angle;
		if (start < goal) {
			T plusTarget = goal - start;
			T minusTarget = (start + max) - (goal);
			if (plusTarget < minusTarget) {
				angle = plusTarget;
			}
			else {
				angle = -minusTarget;
			}
		}
		else {
			T plusTarget = start - goal;
			T minusTarget = (goal + max) - (start);
			if (plusTarget < minusTarget) {
				angle = -plusTarget;
			}
			else {
				angle = minusTarget;
			}
		}
		return angle;
	}
	template <typename T>
	T shortestRotation(T start, T goal) {
		return shortestRotation<T>(start, goal, 360.f);
	}

	/*
	Get angle towards point.
	returns in degrees.
	input:
		origin:	origin angle
	return:
		angle towards point
	*/
	template <typename T>
	T angleTowardsPoint(wykobi::point2d<T> origin, wykobi::point2d<T> target) {
		return wykobi::cartesian_angle<T>(target, origin);
	}

	/*
	Get relative angle
	Input:
		origin:	origin angle
		target:	target angle
		max:	maximum degree (default 360)
	return:
		how many degrees point relative
	*/
	template <typename T>
	T getRelativeAngle(T origin, T target, T max) {
		T out = origin - target;
		if (out < 0) {
			out += max;
		}
		return out;
	}
	template <typename T>
	T getRelativeAngle(T origin, T target) {
		return getRelativeAngle<T>(origin, target, 360.f);
	}

	

	namespace Debug {
		/*
		String contents.
		*/
		std::string toString(wykobi::point2d<float> & point);
		std::string toString(wykobi::vector2d<float> & vector);
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
