//Author: Sivert Andresen Cubedo
#pragma once

#ifndef EditorPath_HEADER
#define EditorPath_HEADER

#include <vector>
#include <algorithm>

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
};

#endif // !


//end