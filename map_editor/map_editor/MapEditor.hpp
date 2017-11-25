//Author: Sivert Andresen Cubedo
#pragma once

#ifndef MapEditor_HEADER
#define MapEditor_HEADER

#define DEBUG 0

#include <iostream>
#include <memory>

#include <SFML\Graphics.hpp>
#include <wykobi.hpp>

#include "Global.hpp"
#include "MapObjects.hpp"
#include "Math.hpp"

class MapEditor : public sf::Drawable, public sf::Transformable {
private:
	/*
	Map size in chunks.
	*/
	int mapWidth, mapHeight;

	/*
	Chunk size.
	*/
	float chunkWidth, chunkHeight;

	/*
	Total map size.
	*/
	float mapTotalWidth, mapTotalHeight;

	/*
	MapPoints.
	*/
	std::vector<std::unique_ptr<MapPoint>> mapPointVector;

	/*
	MapTriangles.
	*/
	std::vector<std::unique_ptr<MapTriangle>> mapTriangleVector;

	/*
	MapChunks.
	*/
	std::vector<std::unique_ptr<MapChunk>> mapChunkVector;

	/*
	List of MapChunk pointers.
	*/
	std::list<MapChunk*> mapChunkList;

	/*
	Matrix of MapChunk pointers.
	*/
	std::vector<std::vector<MapChunk*>> mapChunkMatrix;

	/*
	Texture container pointer.
	*/
	Global::TextureContainer * textureContainer;

	/*
	Ground VertexArray vector.
	*/
	std::vector<sf::VertexArray> groundVertexArrayVector = std::vector<sf::VertexArray>(Global::numberOfTypes, sf::VertexArray(sf::Triangles));

	/*
	Tool VertexArray.
	*/
	sf::VertexArray toolVertexArray = sf::VertexArray(sf::Triangles);

	/*
	Debug line VertexArray.
	*/
	sf::VertexArray debugLineVertexArray = sf::VertexArray(sf::Lines);

	/*
	View
	*/
	sf::View editorView;

	/*
	Tool shape
	*/
	bool drawToolEnabled = false;
	int drawToolType = 0;
	wykobi::point2d<float> drawToolPosition;
	wykobi::polygon<float, 2> drawToolShape;
	//std::vector<wykobi::triangle<float, 2>> drawToolTranslatedAndTriangulated;

	/*
	Debug.
	*/
	bool showTriangleRelations = false;
	bool showChunkRelations = false;
	bool showTriangleEdges = false;
	bool showChunkEdges = false;

	/*
	Draw.
	*/
	virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

	/*
	Set draw tool.
	*/
	void setDrawTool(int type, wykobi::polygon<float, 2> shape);

	/*
	Enable draw tool.
	*/
	void enableDrawTool();

	/*
	Disable tool.
	*/
	void disableDrawTool();

	/*
	Draw tool update.
	Update position of draw tool.
	*/
	void updateDrawTool(wykobi::point2d<float> pos);

	/*
	Draw tool action.
	*/
	void actionDrawTool();

	/*
	Add relation to MapTriangle.
	Also add relation to target pointing back to triangle.
	*/
	void addRelationToMapTriangle(MapTriangle * triangle, MapTriangle * target);

	/*
	Remove all relations from MapTriangle.
	Also remove relations from relations pointing back to triangle.
	*/
	void removeAllRelationsFromMapTriangle(MapTriangle * triangle);

	/*
	Generate chunk relations for all chunks.
	*/
	void generateChunkRelations();

	/*
	Generate triangle relations.
	*/
	void generateTriangleRelations();

	/*
	Generate triangle relations in chunk.
	*/
	void generateTriangleRelationsInChunk(MapChunk * chunk);

	/*
	Generate rectangle for chunk.
	*/
	void generateChunkRectangles();

	/*
	Remove unused points.
	*/
	void removeUnusedMapPoints();

	/*
	Insert polygon on map.
	*/
	void insertPolygon(wykobi::polygon<float, 2> insert_polygon, int type);

	/*
	Insert triangle on map.
	Clip triangle in to polygons that fit in to the chunks the triangle is overlapping.
	Then clip the intersection polygons so the triangle fits in to the mesh (if that makes any sense???).
	DOES NOT WORK ATM!!!!
	*/
	void insertTriangle(wykobi::triangle<float, 2> insert_triangle, int type);
	void insertTriangle(wykobi::triangle<float, 2> insert_triangle, int type, float error); 

	/*
	Remove all MapTriangles from a chunk.
	This will also delete them and all thir relations.
	*/
	void clearChunkMapTriangles(MapChunk * chunk);

	/*
	Apply polygons to chunk.
	Any existing triangles in chunk will be removed.
	ONLY FOR INTERNAL USE!=!?!?!?!?!
	MUST BE SORTED IN TYPES ACORDING TO GLOBAL (whatever that means)
	*/
	void applyPolygonsToChunk(MapChunk * chunk, std::vector<std::vector<wykobi::polygon<float, 2>>> & sorted_polygons);

	/*
	Delete MapTriangle from map.
	*/
	void deleteMapTriangle(MapTriangle * triangle);

	/*
	Get MapPoint at point.
	if there is a MapPoint equal point then return pointer to MapPoint.
	if there is no MApPoint equal point then create a new MapPoint equal to point.
	if point is outside map then return nullptr.
	*/
	MapPoint * getMapPointAtPoint(wykobi::point2d<float> & p);
	MapPoint * getMapPointAtPoint(wykobi::point2d<float> & p, float error);

	/*
	Add make new MapTriangle.
	*/
	MapTriangle * makeMapTriangle(MapChunk * chunk, int type, wykobi::triangle<float, 2> & tri);
	MapTriangle * makeMapTriangle(MapChunk * chunk, int type, wykobi::triangle<float, 2> & tri, float error);

public:
	/*
	Constructor.
	*/
	MapEditor();
	MapEditor(Global::TextureContainer * tex);

	/*
	Open empty new map.
	*/
	void newMap(int mapWidth, int mapHeight, float chunkWidth, float chunkHeight);

	/*
	Open existing map.
	*/
	bool openMap(std::string filename);

	/*
	Save map to file.
	*/
	bool saveMap(std::string filename);

	/*
	Circle Brush!?!?!?
	*/
	void circleBrushTool(int type, wykobi::point2d<float> centre, float diameter);

	/*
	UpdateMousePosition
	update variables that need updated mouse positions (tools)
	pos needs to be translated to map coordniates
	*/
	void updateMousePosition(wykobi::point2d<float> & pos);

	/*
	Call this if left mouse click.
	*/
	void mouseLeftClick();

	/*
	Update VertexArrays.
	*/
	void updateVertexArrays();

	/*
	Get editorView.
	*/
	sf::View & getEditorView() {
		return editorView;
	}

	/*
	Get pointer to chunk at point.
	*/
	MapChunk * getChunkAtPoint(wykobi::point2d<float> & p);

	/*
	Get pointer to triangle point is inside.
	*/
	MapTriangle * getTriangleAtPoint(wykobi::point2d<float> & p);

	/*
	Get chunks inside rect.
	*/
	std::vector<MapChunk*> getChunkInsideRectangle(wykobi::rectangle<float> & rect);

	

	/*
	Transform screen pos to game pos.
	*/
	wykobi::point2d<float> screenPosToGamePos(sf::RenderWindow & window, sf::View & view, wykobi::point2d<float> p);
	wykobi::point2d<float> screenPosToGamePos(sf::RenderWindow & window, sf::View & view, int x, int y);

	/*
	Debug
	*/
	void test();

	void setShowTriangleRelations(bool v) {
		showTriangleRelations = v;
	}
	void setShowChunkRelations(bool v) {
		showChunkRelations = v;
	}
	void setShowTriangleEdges(bool v) {
		showTriangleEdges = v;
	}
	void setShowChunkEdges(bool v) {
		showChunkEdges = v;
	}
};

#endif // !MapEditor_HEADER

