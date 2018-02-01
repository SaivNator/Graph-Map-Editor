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

//#include "RemoveHull.hpp"

class EditorPath : public std::vector<MapPoint*> {
public:
	std::vector<EditorPath> m_hulls;


	/*
	Constructor
	*/
	EditorPath();
	EditorPath(const std::vector<MapPoint*> outer);
	EditorPath(const std::vector<MapPoint*> outer, const std::vector<EditorPath> hulls);

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
	TO STRING FOR DEBUG
	*/
	std::string toString();

	/*
	TO POINTER STRING FOR DEBUG
	*/
	std::string toPointerString();
};

#endif // !


//end