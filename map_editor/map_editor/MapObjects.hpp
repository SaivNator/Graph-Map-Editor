//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MapObjects_HEADER
#define MapObjects_HEADER

#include <vector>
#include <list>
#include <algorithm>

#include <SFML\Graphics.hpp>

#include <wykobi.hpp>

#include "Math.hpp"

class MapPoint;
class MapTriangle;
class MapChunk;
class Unit;

class MapPoint : public wykobi::point2d<float> {
public:
	/*
		Where object is located in vector.
	*/
	int index;

	/*
		Constructor.
	*/
	MapPoint(int index, float x, float y) {
		this->index = index;
		this->x = x;
		this->y = y;
	}
};

class MapTriangle {
public:
	/*
		Where object is located in vector.
	*/
	int index;
	
	/*
		Ground type.
	*/
	int type = 0;

	/*
		Chunk triangle is inside.
	*/
	MapChunk * mapChunk;

	/*
		Points construction triangle.
	*/
	std::vector<MapPoint*> points = std::vector<MapPoint*>(3);
	
	/*
		Triangles triangle is shareing edges with.
	*/
	std::vector<MapTriangle*> relations;

	/*
		Constructor.
	*/
	MapTriangle(int index, std::vector<MapPoint*> points) {
		this->index = index;
		
		for (int i = 0; i < 3; i++) {
			this->points[i] = points[i];
		}
	}
	MapTriangle(int index, int type, std::vector<MapPoint*> points) {
		this->index = index;
		this->type = type;
		for (int i = 0; i < 3; i++) {
			this->points[i] = points[i];
		}
	}
	MapTriangle(int index, MapPoint * a, MapPoint * b, MapPoint * c) {
		this->index = index;
		this->points[0] = a;
		this->points[1] = b;
		this->points[2] = c;
	}
	MapTriangle(int index, int type, MapPoint * a, MapPoint * b, MapPoint * c) {
		this->index = index;
		this->type = type;
		this->points[0] = a;
		this->points[1] = b;
		this->points[2] = c;
	}

	/*
		Get wykobi triangle.
	*/
	wykobi::triangle<float, 2> getWykobiTriangle() {
	//	wykobi::point2d<float> p1 = *points[0];
	//	wykobi::point2d<float> p2 = *points[1];
	//	wykobi::point2d<float> p3 = *points[2];
	//	return wykobi::make_triangle<float>(p1, p2, p3);
		return wykobi::make_triangle<float>(*points[0], *points[1], *points[2]);
	}
	
	/*
		Get sf triangle.
	*/
	std::vector<sf::Vector2f> getSfTriangle() {
		std::vector<sf::Vector2f> vec(3);
		vec[0] = sf::Vector2f(points[0]->x, points[0]->y);
		vec[1] = sf::Vector2f(points[1]->x, points[1]->y);
		vec[2] = sf::Vector2f(points[2]->x, points[2]->y);
		return vec;
	}

	/*
		Get centre of triangle.
	*/
	wykobi::point2d<float> getCentre() {
		return Math::averageCentreOfTriangle2d(getWykobiTriangle());
	}
};

class MapChunk {
public:
	/*
		Where object is located in vector.
	*/
	int index;
	
	/*
		x and y cord.
	*/
	int x, y;

	/*
	Rectangle describing area of chunk.
	*/
	wykobi::rectangle<float> rectangle;

	/*
	Points inside chunk.
	*/
	//std::vector<MapPoint*> points;

	/*
		Triangles inside chunk.
	*/
	std::vector<MapTriangle*> triangles;

	/*
		Chunks shareing edges.
	*/
	std::vector<MapChunk*> relations;

	/*
		Constructor.
	*/
	MapChunk(int index, int x, int y) {
		this->index = index;
		this->x = x;
		this->y = y;
	}
};

#endif // !MapObjects_HEADER


