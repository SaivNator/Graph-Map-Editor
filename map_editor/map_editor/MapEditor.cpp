//Author: Sivert Andresen Cubedo

#include "MapEditor.hpp"

void MapEditor::test() {

	//setDrawTool(Global::Forrest, wykobi::make_polygon<float>(wykobi::make_triangle<float>(-100, -100, 100, 100, -100, 100)));
	setDrawTool(Global::Forrest, wykobi::make_polygon<float>(wykobi::make_rectangle<float>(-100, -100, 100, 100)));

	enableDrawTool();

}

MapEditor::MapEditor() {

}
MapEditor::MapEditor(Global::TextureContainer * tex) {
	this->textureContainer = tex;
}

void MapEditor::clearChunkMapTriangles(MapChunk * chunk) {
	std::vector<MapTriangle*> tempMapTriangles = chunk->triangles;
	for (MapTriangle * mapTriangle : tempMapTriangles) {
		deleteMapTriangle(mapTriangle);
	}
	removeUnusedMapPoints();
}

void MapEditor::applyPolygonsToChunk(MapChunk * chunk, std::vector<std::vector<wykobi::polygon<float, 2>>> & sorted_polygons) {
	clearChunkMapTriangles(chunk);
	for (int i = 0; i < sorted_polygons.size(); i++) {
		for (wykobi::polygon<float, 2> poly : sorted_polygons[i]) {
			poly = Math::removeDuplicates<wykobi::polygon<float, 2>>(poly);
			std::vector<wykobi::triangle<float, 2>> triangles = Math::triangulatePolygon(poly);
			for (wykobi::triangle<float, 2> & tri : triangles) {
				makeMapTriangle(chunk, i, tri, 1.f);
			}
		}
	}
	generateTriangleRelationsInChunk(chunk);
}

void MapEditor::mouseLeftClick() {
	if (drawToolEnabled) {
		actionDrawTool();
	}
}

wykobi::point2d<float> MapEditor::screenPosToGamePos(sf::RenderWindow & window, sf::View & view, wykobi::point2d<float> p) {
	float x = p.x;
	float y = p.y;
	x = Math::normalizeFloat(x, 0.f, (float)window.getSize().x);
	y = Math::normalizeFloat(y, 0.f, (float)window.getSize().y);
	x = Math::deNormalizeFloat(x, 0, view.getSize().x);
	y = Math::deNormalizeFloat(y, 0, view.getSize().y);
	x = x + (view.getCenter().x - view.getSize().x / 2);
	y = y + (view.getCenter().y - view.getSize().y / 2);
	return wykobi::make_point<float>(x, y);
}
wykobi::point2d<float> MapEditor::screenPosToGamePos(sf::RenderWindow & window, sf::View & view, int x, int y) {
	return screenPosToGamePos(window, view, wykobi::make_point<float>((float)x, (float)y));
}

void MapEditor::updateMousePosition(wykobi::point2d<float> & pos) {
	updateDrawTool(pos);
}

void MapEditor::setDrawTool(int type, wykobi::polygon<float, 2> shape) {
	drawToolType = type;
	drawToolShape = shape;
}

void MapEditor::enableDrawTool() {
	drawToolEnabled = true;
}

void MapEditor::disableDrawTool() {
	drawToolEnabled = false;
}

void MapEditor::updateDrawTool(wykobi::point2d<float> pos) {
	drawToolPosition = pos;
	wykobi::vector2d<float> transVector = wykobi::make_vector<float>(drawToolPosition);
	drawToolTranslatedAndTriangulated.clear();
	for (wykobi::triangle<float, 2> triangle : Math::triangulatePolygon(drawToolShape)) {
		drawToolTranslatedAndTriangulated.push_back(wykobi::translate<float>(transVector, triangle));
	}
}

void MapEditor::actionDrawTool() {
	//std::cout << mapTriangleVector.size() << "\t";
	for (wykobi::triangle<float, 2> & triangle : drawToolTranslatedAndTriangulated) {
		insertTriangle(triangle, drawToolType, 1.f);
	}
	//std::cout << mapTriangleVector.size() << "\n";
}

void MapEditor::removeUnusedMapPoints() {
	std::vector<bool> temp_point_vector(mapPointVector.size(), false);
	//gather unused points
	for (auto it = mapTriangleVector.begin(); it != mapTriangleVector.end(); ++it) {
		MapTriangle * triangle = (*it).get();
		for (int i = 0; i < 3; i++) {
			MapPoint * point = triangle->points[i];
			temp_point_vector[point->index] = true;
		}
	}
	//delete unused points
	int offset = 0;
	int i = 0;
	while (i < temp_point_vector.size()) {
		if (!temp_point_vector[i]) {
			auto it = mapPointVector.begin() + (i - offset);
			mapPointVector.erase(it);
			offset++;
		}
		i++;
	}
	//re-apply index
	i = 0;
	for (auto it = mapPointVector.begin(); it != mapPointVector.end(); ++it) {
		(*it)->index = i;
		i++;
	}
}

void MapEditor::addRelationToMapTriangle(MapTriangle * triangle, MapTriangle * target) {
	auto target_it = std::find(target->relations.begin(), target->relations.end(), triangle);
	if (target_it == target->relations.end()) {
		target->relations.push_back(triangle);
	}
	auto triangle_it = std::find(triangle->relations.begin(), triangle->relations.end(), target);
	if (triangle_it == triangle->relations.end()) {
		triangle->relations.push_back(target);
	}
}

void MapEditor::removeAllRelationsFromMapTriangle(MapTriangle * triangle) {
	for (MapTriangle * relation : triangle->relations) {
		auto rel_it = std::find(relation->relations.begin(), relation->relations.end(), triangle);
		if (rel_it != relation->relations.end()) {
			relation->relations.erase(rel_it);
		}
	}
	triangle->relations.clear();
}

MapTriangle * MapEditor::makeMapTriangle(MapChunk * chunk, int type, wykobi::triangle<float, 2> & tri) {
	std::vector<MapPoint*> points;
	points.reserve(3);
	for (int i = 0; i < 3; i++) {
		points.push_back(getMapPointAtPoint(tri[i]));
	}
	mapTriangleVector.push_back(std::unique_ptr<MapTriangle>(new MapTriangle(mapTriangleVector.size(), type, points)));
	MapTriangle * pointer = mapTriangleVector.back().get();
	chunk->triangles.push_back(pointer);
	pointer->mapChunk = chunk;
	return pointer;
}
MapTriangle * MapEditor::makeMapTriangle(MapChunk * chunk, int type, wykobi::triangle<float, 2> & tri, float error) {
	if (error == 0.f) {
		return makeMapTriangle(chunk, type, tri);
	}
	std::vector<MapPoint*> points;
	points.reserve(3);
	for (int i = 0; i < 3; i++) {
		points.push_back(getMapPointAtPoint(tri[i], error));
	}
	mapTriangleVector.push_back(std::unique_ptr<MapTriangle>(new MapTriangle(mapTriangleVector.size(), type, points)));
	MapTriangle * pointer = mapTriangleVector.back().get();
	chunk->triangles.push_back(pointer);
	pointer->mapChunk = chunk;
	return pointer;
}


void MapEditor::deleteMapTriangle(MapTriangle * triangle) {
	MapChunk * chunk = triangle->mapChunk;
	{
		auto it = std::find(chunk->triangles.begin(), chunk->triangles.end(), triangle);
		if (it == chunk->triangles.end()) {
			return;
		}
		chunk->triangles.erase(it);
	}
	{
		int index = triangle->index;
		//remove relations
		for (MapTriangle * rel : triangle->relations) {
			auto iter = std::find(rel->relations.begin(), rel->relations.end(), triangle);
			if (iter != rel->relations.end()) {
				rel->relations.erase(iter);
			}
		}
		auto it = mapTriangleVector.begin() + index;
		it = mapTriangleVector.erase(it);
		while (it != mapTriangleVector.end()) {
			(*it)->index = index;
			index++;
			++it;
		}
	}
}

void MapEditor::insertPolygon(wykobi::polygon<float, 2> insert_polygon, int type) {
	//Gather relevant chunks
	insert_polygon = Math::roundWykobiPolygon(insert_polygon);
	insert_polygon = Math::removeDuplicates<wykobi::polygon<float, 2>>(insert_polygon);
	std::vector<MapChunk*> chunks_vector_1;
	std::vector<MapChunk*> chunks_vector_2;
	chunks_vector_1 = getChunkInsideRectangle(Math::createRectangleFromPolygon(insert_polygon));
	for (MapChunk * mapChunk : chunks_vector_1) {
		if (Math::isPolygonsOverlapping(wykobi::make_polygon<float>(mapChunk->rectangle), insert_polygon)) {
			chunks_vector_2.push_back(mapChunk);
		}
	}
	//Handle chunks
	for (MapChunk * mapChunk : chunks_vector_2) {
		std::vector<std::vector<wykobi::triangle<float, 2>>> triangle_vector_1(Global::numberOfTypes);
		std::vector<std::vector<wykobi::triangle<float, 2>>> triangle_vector_2(Global::numberOfTypes);
		std::vector<std::vector<wykobi::polygon<float, 2>>> polygon_vector_1(Global::numberOfTypes);
		std::vector<std::vector<wykobi::polygon<float, 2>>> polygon_vector_2(Global::numberOfTypes);
		//Sort triangles
		for (MapTriangle * mapTriangle : mapChunk->triangles) {
			triangle_vector_1[mapTriangle->type].push_back(mapTriangle->getWykobiTriangle());
		}
		//Merge triangles in to polygons
		for (int i = 0; i < Global::numberOfTypes; i++) {
			polygon_vector_1[i] = Math::Clipper::mergeTriangles(triangle_vector_1[i]);
		}
		
		//Remove possible hull
		for (int i = 0; i < Global::numberOfTypes; i++) {
			wykobi::polygon<float, 2> current_outside;
			wykobi::polygon<float, 2> current_inside;
		}
		
		
		//Add insert_polygon to polygon_vector_1
		//polygon_vector_1[type].push_back(insert_polygon);
		

	}
}

void MapEditor::insertTriangle(wykobi::triangle<float, 2> insert_triangle, int type) {
	insertTriangle(insert_triangle, type, 0.f);
}
void MapEditor::insertTriangle(wykobi::triangle<float, 2> insert_triangle, int type, float error) {
#if 1
	/*
	Pseudo code:
	1. Round insert_triangle
	2. Gather chunks insert_triangle is overlapping and put them in intersected_chunks_list.
	3. For each chunk in intersected_chunks_list:
		1. Merge all triangles in chunk in to polygons sorted by type, call it merged_polygons.
		2. if insert_triangle is inside one polygon in merged_polygons then:
			1. if insert_triangle and polygon has same type, then break (finish). (this case will only happen if insert_triangle is in one chunk).
			2. else, then remove hull insert_triangle is makeing from polygon, then triangulate append to chunk, then break
		3. else:
			1. Merge insert_triangle with polygons that are the same type, and clip difference polygons with != type.
			2. if insert_triangle was not merged, then make insert_triangle it's own polygon
			3. then triangulate and yadi yadi ya.
	*/
#if DEBUG
	std::cout << "insertTriangle_begin" << "\n";
#endif
#if DEBUG
	std::cout << "\t" << "roundInsertTriangle_begin" << "\n";
#endif
	//Round inser_triangle
	insert_triangle = Math::roundWykobiTriangle(insert_triangle);
	wykobi::polygon<float, 2> insert_polygon = wykobi::make_polygon(insert_triangle);
#if DEBUG
	std::cout << "\t" << "roundInsertTriangle_end" << "\n";
#endif
#if DEBUG
	std::cout << "\t" << "findChunks_begin" << "\n";
#endif
	//Gather chunks triangle is overlapping.
	std::vector<MapChunk*> chunks = getChunkInsideRectangle(Math::createRectangleFromTriangle(insert_triangle));
	std::vector<MapChunk*> intersected_chunks;
	for (MapChunk * chunk : chunks) {
		if (wykobi::intersect<float>(insert_triangle, chunk->rectangle) || Math::isPolygonInsidePolygon(wykobi::make_polygon<float>(chunk->rectangle), wykobi::make_polygon<float>(insert_triangle))) {
			intersected_chunks.push_back(chunk);
		}
	}
#if DEBUG
	std::cout << "\t" << "findChunks_end" << "\n";
#endif
#if DEBUG
	std::cout << "\t" << "makeSpace_begin" << "\n";
#endif
	//Make space for insert_triangle.
	for (MapChunk * chunk : intersected_chunks) {
		std::vector<std::vector<wykobi::polygon<float, 2>>> polygons_sorted_in_types(Global::numberOfTypes);
		std::vector<std::vector<wykobi::polygon<float, 2>>> temp_polygons_sorted_in_types(Global::numberOfTypes);
		for (int i = 0; i < Global::numberOfTypes; i++) {
			std::vector<wykobi::triangle<float, 2>> temp_triangles;
			for (MapTriangle * mapTriangle : chunk->triangles) {
				if (mapTriangle->type == i) {
					temp_triangles.push_back(mapTriangle->getWykobiTriangle());
				}
			}
			std::vector<wykobi::point2d<float>> temp_point_vector = Math::makeWykobiPointVectorFromShape<wykobi::triangle<float, 2>>(temp_triangles);
			polygons_sorted_in_types[i] = Math::Clipper::mergeTriangles(temp_triangles);
			for (int j = 0; j < polygons_sorted_in_types[i].size(); j++) {
				polygons_sorted_in_types[i][j] = Math::insertPointsOnEdgesOfPolygon(polygons_sorted_in_types[i][j], temp_point_vector);
			}
		}
		bool isInsertDone = false;
		std::vector<wykobi::polygon<float, 2>> insert_polygon_clipped_in_chunk = Math::Clipper::clipPolygonIntersection(insert_polygon, wykobi::make_polygon<float>(chunk->rectangle));
		for (int i = 0; i < Global::numberOfTypes; i++) {
			for (int j = 0; j < polygons_sorted_in_types[i].size(); j++) {
				wykobi::polygon<float, 2> & polygon = polygons_sorted_in_types[i][j];
				std::vector<wykobi::point2d<float>> polygon_points = Math::makeWykobiPointVectorFromShape(polygon);
				//check if insert_triangle is inside a polygon with simular types	(this can only happend once)
				if (Math::isPolygonInsidePolygon(insert_polygon, polygon)) {
					if (i == type) {
						return;	//if this then there is nothing to insert to the map.
					}
					else {
						//if this then remove hull from polygon then insert insert_polygon as an independent polygon
						std::vector<wykobi::polygon<float, 2>> temp_poly = Math::Clipper::removePolygonHull(polygon, insert_polygon);
						temp_polygons_sorted_in_types[i].insert(temp_polygons_sorted_in_types[i].end(), temp_poly.begin(), temp_poly.end());
						temp_polygons_sorted_in_types[type].push_back(insert_polygon);
						isInsertDone = true;
					}
				}
				//check if insert_triangle is overlapping a polygon
				else if (Math::isPolygonsOverlapping(polygon, insert_polygon)) {
					if (i == type && !isInsertDone) {
						//if this merge insert_triangle with polygon.
						std::vector<wykobi::polygon<float, 2>> temp_poly;
						temp_poly.push_back(polygon);
						temp_poly.insert(temp_poly.end(), insert_polygon_clipped_in_chunk.begin(), insert_polygon_clipped_in_chunk.end());
						temp_poly = Math::Clipper::mergePolygons(temp_poly);
						for (int k = 0; k < temp_poly.size(); k++) {
							temp_poly[k] = Math::insertPointsOnEdgesOfPolygon(temp_poly[k], polygon_points);
						}
						temp_polygons_sorted_in_types[i].insert(temp_polygons_sorted_in_types[i].end(), temp_poly.begin(), temp_poly.end());
						isInsertDone = true;
					}
					else {
						//if this clip polygon against insert_triangle
						std::vector<wykobi::polygon<float, 2>> temp_poly = Math::Clipper::clipPolygonDifference(polygon, insert_polygon);
						for (int k = 0; k < temp_poly.size(); k++) {
							temp_poly[k] = Math::insertPointsOnEdgesOfPolygon(temp_poly[k], polygon_points);
						}
						temp_polygons_sorted_in_types[i].insert(temp_polygons_sorted_in_types[i].begin(), temp_poly.begin(), temp_poly.end());
					}
				}
				else {
					temp_polygons_sorted_in_types[i].push_back(polygon);
				}
			}
		}
		if (!isInsertDone) {
			temp_polygons_sorted_in_types[type].insert(temp_polygons_sorted_in_types[type].end(), insert_polygon_clipped_in_chunk.begin(), insert_polygon_clipped_in_chunk.end());
		}
		applyPolygonsToChunk(chunk, temp_polygons_sorted_in_types);
	}
#if DEBUG
	std::cout << "\t" << "makeSpace_end" << "\n";
#endif

#elif 0
	/*
	Pseudo code:
	1. Gather chunks insert_triangle is overlapping and put them in intersected_chunks_list.
	2. For each chunk in intersected_chunks_list:
		1. Make space for insert_triangle.
	3. For each chunk in intersected_chunks_list:
		1. Clip insert_triangle inside chunk->rectangle, call it clipped_insert_polygon.
		2. If there are any points in chunk that is on any segment of clipped_insert_polygon, insert them to clipped_insert_polygon.
		3. Triangulate clipped_insert_polygon and make MapTriangles of the triangles.
	*/
#if DEBUG
	std::cout << "insertTriangle_begin" << "\n"; 
#endif
#if DEBUG
	std::cout << "\t" << "findChunks_begin" << "\n";
#endif
	//Gather chunks triangle is overlapping.
	std::vector<MapChunk*> chunks = getChunkInsideRectangle(Math::createRectangleFromTriangle(insert_triangle));
	std::vector<MapChunk*> intersected_chunks;
	for (MapChunk * chunk : chunks) {
		if (wykobi::intersect<float>(insert_triangle, chunk->rectangle) || Math::isPolygonInsidePolygon(wykobi::make_polygon<float>(chunk->rectangle), wykobi::make_polygon<float>(insert_triangle))) {
			intersected_chunks.push_back(chunk);
		}
	}
#if DEBUG
	std::cout << "\t" << "findChunks_end" << "\n";
#endif
#if DEBUG
	std::cout << "\t" << "handleChunk_begin" << "\n";
#endif
	//Make space for insert_triangle.
	for (MapChunk * chunk : intersected_chunks) {
#if DEBUG
		std::cout << "\t\t" << "handleTriangles_begin" << "\n";
#endif
		std::list<MapTriangle*> mapTriangles_to_clip;
		std::list<MapTriangle*> mapTriangles_to_delete;
		for (MapTriangle * current_mapTriangle : chunk->triangles) {
			wykobi::triangle<float, 2> current_triangle = current_mapTriangle->getWykobiTriangle();
			if (Math::isTrianglesOverlapping(current_triangle, insert_triangle)) {
				if (Math::isTriangleInsideTriangle(current_triangle, insert_triangle)) {
					mapTriangles_to_delete.push_back(current_mapTriangle);
				}
				else {
					mapTriangles_to_clip.push_back(current_mapTriangle);
				}
			}
		}
#if DEBUG
		std::cout << "\t\t" << "handleTriangles_end" << "\n";
#endif
#if DEBUG
		std::cout << "\t\t" << "deleteTriangles_begin" << "\n";
#endif
		//delete unwanted triangles
		for (MapTriangle * current_mapTriangle : mapTriangles_to_delete) {
			deleteMapTriangle(current_mapTriangle);
		}
#if DEBUG
		std::cout << "\t\t" << "deleteTriangles_end" << "\n";
#endif
		//clip triangles
#if DEBUG
		std::cout << "\t\t" << "gatherTriangles_begin" << "\n";
#endif
		//gather triangles by type
		std::vector<std::vector<wykobi::triangle<float, 2>>> triangles_sorted_in_types(Global::numberOfTypes, std::vector<wykobi::triangle<float, 2>>());
		for (MapTriangle * current_mapTriangle : mapTriangles_to_clip) {
			triangles_sorted_in_types[current_mapTriangle->type].push_back(current_mapTriangle->getWykobiTriangle());
			deleteMapTriangle(current_mapTriangle);
		}
#if DEBUG
		std::cout << "\t\t" << "gatherTriangles_end" << "\n";
#endif
#if DEBUG
		std::cout << "\t\t" << "mergeTriangles_begin" << "\n";
#endif
		//merge triangles by type and make space for insert triangle
		std::vector<std::vector<wykobi::polygon<float, 2>>> polygons_sorted_in_types(Global::numberOfTypes);
		for (int i = 0; i < triangles_sorted_in_types.size(); i++) {
#if DEBUG
			std::cout << "\t\t\t" << "makePolygons_begin" << "\n";
#endif
			//merge triangles in to polygons.
			polygons_sorted_in_types[i] = Math::Clipper::mergeTriangles(triangles_sorted_in_types[i]);
#if DEBUG
			std::cout << "\t\t\t" << "makePolygons_end" << "\n";
#endif
#if DEBUG
			std::cout << "\t\t\t" << "clipPolygons_begin" << "\n";
#endif
			//clip polygons (remove parts that are under insert_triangle).
			std::vector<wykobi::polygon<float, 2>> temp_polygon_vector;
			for (wykobi::polygon<float, 2> poly1 : polygons_sorted_in_types[i]) {
				if (Math::isPolygonInsidePolygon(wykobi::make_polygon<float>(insert_triangle), poly1)) {
					std::vector<wykobi::polygon<float, 2>> temp_vec = Math::Clipper::removePolygonHull(poly1, wykobi::make_polygon<float>(insert_triangle));
					temp_polygon_vector.insert(temp_polygon_vector.end(), temp_vec.begin(), temp_vec.end());
					break;
				}
				for (wykobi::polygon<float, 2> poly2 : Math::Clipper::clipPolygonDifference(poly1, wykobi::make_polygon<float>(insert_triangle))) {
					temp_polygon_vector.push_back(poly2);
				}
			}
#if DEBUG
			std::cout << "\t\t\t" << "clipPolygons_end" << "\n";
#endif
#if DEBUG
			std::cout << "\t\t\t" << "insertLostPoints_begin" << "\n";
#endif
			polygons_sorted_in_types[i].swap(temp_polygon_vector);
			//insert possible lost points.
			std::vector<wykobi::point2d<float>> temp_point_vector = Math::makeWykobiPointVectorFromShape<wykobi::triangle<float, 2>>(triangles_sorted_in_types[i]);
			for (int j = 0; j < polygons_sorted_in_types[i].size(); j++) {
				polygons_sorted_in_types[i][j] = Math::insertPointsOnEdgesOfPolygon(polygons_sorted_in_types[i][j], temp_point_vector);
			}
#if DEBUG
			std::cout << "\t\t\t" << "insertLostPoints_end" << "\n";
#endif
#if DEBUG
			std::cout << "\t\t\t" << "triangulatePolygons_begin" << "\n";
#endif
			//triangulate polygons and make MapTriangles of triangles.
			for (wykobi::polygon<float, 2> poly : polygons_sorted_in_types[i]) {
#if DEBUG
				std::cout << "\t\t\t\t" << "insidePoly_begin " << Math::Debug::toCode(poly) << "\n";
#endif
				poly = Math::removeDuplicates<wykobi::polygon<float, 2>>(poly);	//PATCH (will leave empty spots).
				for (wykobi::triangle<float, 2> tri : Math::triangulatePolygon(poly)) {
#if DEBUG
					std::cout << "\t\t\t\t\t" << "insideTri_begin" << "\n";
#endif
					makeMapTriangle(chunk, i, tri, error);
#if DEBUG
					std::cout << "\t\t\t\t\t" << "insideTri_end" << "\n";
#endif
				}
#if DEBUG
				std::cout << "\t\t\t\t" << "insidePoly_end" << "\n";
#endif
			}
#if DEBUG
			std::cout << "\t\t\t" << "triangulatePolygons_end" << "\n";
#endif
		}
#if DEBUG
		std::cout << "\t\t" << "mergeTriangles_end" << "\n";
#endif
#if DEBUG
		std::cout << "\t\t" << "addTriangles_begin" << "\n";
#endif
		//Add insert_triangle.
		for (wykobi::polygon<float, 2> poly : Math::Clipper::clipPolygonIntersection(wykobi::make_polygon<float>(insert_triangle), wykobi::make_polygon<float>(chunk->rectangle))) {
			for (wykobi::triangle<float, 2> tri : Math::triangulatePolygon(poly)) {
				makeMapTriangle(chunk, type, tri, error);
			}
		}
#if DEBUG		
		std::cout << "\t\t" << "addTriangles_end" << "\n";
#endif
	}
#if DEBUG
	std::cout << "\t" << "handleChunks_end" << "\n";
#endif
#if DEBUG
	std::cout << "\t" << "triangleRelations_begin" << "\n";
#endif
	//relations
	for (MapChunk * chunk : intersected_chunks) {
		generateTriangleRelationsInChunk(chunk);
	}
#if DEBUG
	std::cout << "\t" << "triangleRelations_end" << "\n";
#endif
#if DEBUG
	std::cout << "\t" << "removeUnusedPoints_begin" << "\n";
#endif	
	removeUnusedMapPoints();
#if DEBUG
	std::cout << "\t" << "removeUnusedPoints_end" << "\n";
#endif
#if DEBUG
	std::cout << "insertTriangle_end" << "\n\n";
#endif

#elif 0
	//Gather chunks triangle is overlapping.
	std::vector<MapChunk*> chunks = getChunkInsideRectangle(Math::createRectangleFromTriangle(insert_triangle));
	std::vector<MapChunk*> intersected_chunks;
	for (MapChunk * chunk : chunks) {
		if (wykobi::intersect<float>(insert_triangle, chunk->rectangle)) {
			intersected_chunks.push_back(chunk);
		}
	}
	for (MapChunk * chunk : intersected_chunks) {	
		//make space for new triangle
		std::vector<MapTriangle*> test_triangles = chunk->triangles;
		for (auto it = test_triangles.begin(); it != test_triangles.end(); ++it) {
			MapTriangle * mapTriangle = (*it);
			wykobi::triangle<float, 2> current_triangle = mapTriangle->getWykobiTriangle();
			//if (wykobi::intersect<float>(current_triangle, insert_triangle)) {
			if (Math::isTrianglesOverlapping(current_triangle, insert_triangle)) {
				std::vector<wykobi::triangle<float, 2>> pending_triangles;
				int current_type = mapTriangle->type;
				//if current_triangle is inside insert_triangle then delete current_triangle
				if (Math::isTriangleInsideTriangle(current_triangle, insert_triangle)) {
					deleteMapTriangle(mapTriangle);
					continue;
				}
				//if insert_triangle is inside current_triangle then divide current_triangle at centre of current_triangle
				else if (Math::isTriangleInsideTriangle(insert_triangle, current_triangle)) {
					wykobi::point2d<float> point = Math::averageCentreOfTriangle2d(insert_triangle);
					auto vec = Math::Clipper::divideTriangleWithPointInside(current_triangle, point);	
					pending_triangles.insert(pending_triangles.end(), vec.begin(), vec.end());
					deleteMapTriangle(mapTriangle);
				}
				//else insert_triangle is overlapping one or more edges of current_triangle
				else {
					pending_triangles.push_back(current_triangle);
					deleteMapTriangle(mapTriangle);
				}
				//clip
				for (wykobi::triangle<float, 2> tri : pending_triangles) {
					wykobi::polygon<float, 2> current_polygon = wykobi::make_polygon<float>(tri);
					std::vector<wykobi::polygon<float, 2>> vec = Math::Clipper::clipPolygonDifference(current_polygon, wykobi::make_polygon<float>(insert_triangle));
					for (wykobi::polygon<float, 2> clipped_current_polygon : vec) {
						std::vector<wykobi::triangle<float, 2>> small_triangles = Math::triangulatePolygon(clipped_current_polygon);
						for (wykobi::triangle<float, 2> small_tri : small_triangles) {
							makeMapTriangle(chunk, current_type, small_tri, 2.f);	//MAY ADJUST ERROR
						}
					}
				}
			}
		}
		//insert new triangle here:
		wykobi::polygon<float, 2> clipped_insert_polygon;
		wykobi::algorithm::sutherland_hodgman_polygon_clipper<wykobi::point2d<float>>(chunk->rectangle, wykobi::make_polygon<float>(insert_triangle), clipped_insert_polygon);
		if (clipped_insert_polygon.size() < 3) {
			continue;
		}
		clipped_insert_polygon = Math::removeDuplicates<wykobi::polygon<float, 2>>(clipped_insert_polygon);
		auto vec = Math::triangulatePolygon(clipped_insert_polygon);
		for (auto tri : vec) {
			makeMapTriangle(chunk, type, tri, 2);
		}
		generateTriangleRelationsInChunk(chunk);
	}
#endif
}

std::vector<MapChunk*> MapEditor::getChunkInsideRectangle(wykobi::rectangle<float> & rect) {
	std::vector<MapChunk*> chunks;
	wykobi::point2d<float> point1 = rect[0];
	wykobi::point2d<float> point2 = rect[1];
	int x1 = (int)(point1.x / chunkWidth);
	int x2 = (int)(point2.x / chunkWidth);
	int y1 = (int)(point1.y / chunkHeight);
	int y2 = (int)(point2.y / chunkHeight);
	if (x1 > x2) {
		int temp = x1;
		x1 = x2;
		x2 = temp;
	}
	if (y1 > y2) {
		int temp = y1;
		y1 = y2;
		y2 = temp;
	}
	for (int x = x1; x <= x2; x++) {
		for (int y = y1; y <= y2; y++) {
			if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight) {
				chunks.push_back(mapChunkMatrix[x][y]);
			}
		}
	}
	return chunks;
}

void MapEditor::generateTriangleRelationsInChunk(MapChunk * chunk) {
#if 0
	std::vector<MapTriangle*> triangles_around_chunk;
	for (MapChunk * rel_chunk : chunk->relations) {
		triangles_around_chunk.insert(triangles_around_chunk.end(), rel_chunk->triangles.begin(), rel_chunk->triangles.end());
	}
	for (MapTriangle * mapTriangle : chunk->triangles) {
		for (MapTriangle * rel : mapTriangle->relations) {
			auto it = std::find(rel->relations.begin(), rel->relations.end(), mapTriangle);
			if (it != rel->relations.end()) {
				rel->relations.erase(it);
			}
		}
		mapTriangle->relations.clear();
	}
	//triangles inside chunk
	for (MapTriangle * mapTriangle : chunk->triangles) {
		wykobi::triangle<float, 2> triangle = mapTriangle->getWykobiTriangle();
		for (MapTriangle * test_mapTriangle : chunk->triangles) {
			if (mapTriangle == test_mapTriangle) {
				continue;
			}
			wykobi::triangle<float, 2> test_triangle = test_mapTriangle->getWykobiTriangle();
			if (Math::trianglesSharingEdge(triangle, test_triangle)) {
				mapTriangle->relations.push_back(test_mapTriangle);
			}
		}
		for (MapTriangle * test_mapTriangle : triangles_around_chunk) {
			wykobi::triangle<float, 2> test_triangle = test_mapTriangle->getWykobiTriangle();
			if (Math::trianglesSharingEdge(triangle, test_triangle)) {
				mapTriangle->relations.push_back(test_mapTriangle);
				if (std::find(test_mapTriangle->relations.begin(), test_mapTriangle->relations.end(), mapTriangle) == test_mapTriangle->relations.end()) {
					test_mapTriangle->relations.push_back(mapTriangle);
				}
			}
		}
	}
#else
	std::vector<MapTriangle*> triangles_surrounding_chunk;
	triangles_surrounding_chunk.insert(triangles_surrounding_chunk.end(), chunk->triangles.begin(), chunk->triangles.end());
	for (MapChunk * rel_chunk : chunk->relations) {
		triangles_surrounding_chunk.insert(triangles_surrounding_chunk.end(), rel_chunk->triangles.begin(), rel_chunk->triangles.end());
	}
	for (MapTriangle * triangle : chunk->triangles) {
		removeAllRelationsFromMapTriangle(triangle);
		for (MapTriangle * test : triangles_surrounding_chunk) {
			if (Math::numberOfEqualElements<MapPoint*>(triangle->points, test->points) == 2) {
				addRelationToMapTriangle(triangle, test);
			}
		}
	}
#endif
}

void MapEditor::generateTriangleRelations() {
#if 1
	for (MapChunk * chunk : mapChunkList) {
		//std::cout << chunk->x << " " << chunk->y << "\n";
		generateTriangleRelationsInChunk(chunk);
	}
#else
	for (auto it = mapChunkList.begin(); it != mapChunkList.end(); ++it) {
		MapChunk * chunk = (*it);
		std::vector<MapTriangle*> triangle_list;
		triangle_list.insert(triangle_list.end(), chunk->triangles.begin(), chunk->triangles.end());
		for (MapChunk * rel_chunk : chunk->relations) {
			triangle_list.insert(triangle_list.end(), rel_chunk->triangles.begin(), rel_chunk->triangles.end());
		}
		for (MapTriangle * triangle : chunk->triangles) {
			triangle->relations.clear();
			for (MapTriangle * test_triangle : triangle_list) {
				if (triangle == test_triangle) {
					continue;
				}
				if (Math::numberOfEqualElements<MapPoint*>(triangle->points, test_triangle->points) == 2) {
					triangle->relations.push_back(test_triangle);
				}
			}
		}
	}
#endif
}

MapPoint * MapEditor::getMapPointAtPoint(wykobi::point2d<float> & p) {
	//if point is outside map
	//if (p.x < 0 || p.x > mapTotalWidth || p.y < 0 || p.y > mapTotalHeight) {
	//	return nullptr;
	//}

	//check is a MapPoint is equal.
	for (auto it = mapPointVector.begin(); it != mapPointVector.end(); ++it) {
		MapPoint * mapPoint = (*it).get();
		if (*mapPoint == p) {
			return mapPoint;
		}
	}

	//if no equal.
	int index = mapPointVector.size();
	mapPointVector.push_back(std::unique_ptr<MapPoint>(new MapPoint(index, p.x, p.y)));
	return mapPointVector.back().get();
}
MapPoint * MapEditor::getMapPointAtPoint(wykobi::point2d<float> & p, float error) {
	//if point is outside map
	//if (p.x < 0 || p.x > mapTotalWidth || p.y < 0 || p.y > mapTotalHeight) {
	//	return nullptr;
	//}

	//check is a MapPoint is equal.
	wykobi::circle<float> test_circle = wykobi::make_circle<float>(p, error);
	for (auto it = mapPointVector.begin(); it != mapPointVector.end(); ++it) {
		MapPoint * mapPoint = (*it).get();
		if (wykobi::point_in_circle<float>(*mapPoint, test_circle)) {
			return mapPoint;
		}
	}

	//if no equal.
	int index = mapPointVector.size();
	mapPointVector.push_back(std::unique_ptr<MapPoint>(new MapPoint(index, p.x, p.y)));
	return mapPointVector.back().get();
}

MapChunk * MapEditor::getChunkAtPoint(wykobi::point2d<float> & p) {
	int x = (int)(p.x / chunkWidth);
	int y = (int)(p.y / chunkHeight);
	if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight) {
		return mapChunkMatrix[x][y];
	}
	else {
		return nullptr;
	}
}

MapTriangle * MapEditor::getTriangleAtPoint(wykobi::point2d<float> & p) {
	MapChunk * chunk = getChunkAtPoint(p);
	if (chunk != nullptr) {
		for (MapTriangle * triangle : chunk->triangles) {
			if (wykobi::point_in_triangle<float>(p, triangle->getWykobiTriangle())) {
				return triangle;
			}
		}
	}
	return nullptr;
}

void MapEditor::generateChunkRectangles() {
	for (auto it = mapChunkList.begin(); it != mapChunkList.end(); ++it) {
		MapChunk * chunk = (*it);
		float x = (float)chunk->x * chunkWidth;
		float y = (float)chunk->y * chunkHeight;
		wykobi::point2d<float> p1 = wykobi::make_point<float>(x, y);
		wykobi::point2d<float> p2 = wykobi::make_point<float>(x + chunkWidth, y + chunkHeight);
		chunk->rectangle = wykobi::make_rectangle<float>(p1, p2);
	}
}

void MapEditor::generateChunkRelations() {
	//Chunk relations
	for (int x = 0; x < mapWidth; x++) {
		for (int y = 0; y < mapHeight; y++) {
			MapChunk * chunk = mapChunkMatrix[x][y];
			chunk->relations.clear();
			std::vector<std::pair<int, int>> rel = {
				std::pair<int, int>(x - 1, y - 1),
				std::pair<int, int>(x, y - 1),
				std::pair<int, int>(x + 1, y - 1),
				std::pair<int, int>(x + 1, y),
				std::pair<int, int>(x + 1, y + 1),
				std::pair<int, int>(x, y + 1),
				std::pair<int, int>(x - 1, y + 1),
				std::pair<int, int>(x - 1, y)
			};
			for (auto n : rel) {
				if (!(n.first < 0 || n.first >= mapWidth || n.second < 0 || n.second >= mapHeight)) {
					chunk->relations.push_back(mapChunkMatrix[n.first][n.second]);
				}
			}
		}
	}
}

void MapEditor::newMap(int mw, int mh, float cw, float ch) {

	mapWidth = mw;
	mapHeight = mh;
	chunkWidth = cw;
	chunkHeight = ch;
	mapTotalWidth = mapWidth * chunkWidth;
	mapTotalHeight = mapHeight * chunkHeight;

	mapChunkVector = std::vector<std::unique_ptr<MapChunk>>(mapWidth * mapHeight);
	mapChunkList = std::list<MapChunk*>();
	mapChunkMatrix = std::vector<std::vector<MapChunk*>>(mapWidth, std::vector<MapChunk*>(mapHeight));

	int index = 0;
	for (int x = 0; x < mapWidth; x++) {
		for (int y = 0; y < mapHeight; y++) {
			mapChunkVector[index] = std::unique_ptr<MapChunk>(new MapChunk(index, x, y));
			
			mapChunkList.push_back(mapChunkVector[index].get());
			
			mapChunkMatrix[x][y] = mapChunkVector[index].get();

			index++;
		}
	}

	generateChunkRelations();
	generateChunkRectangles();

	//Fill map with empty void triangles.
	int type = Global::GroundTypeNumber::Void;
	//int type = Global::GroundTypeNumber::Desert;
	index = 0;
	for (auto it = mapChunkList.begin(); it != mapChunkList.end(); ++it) {
		MapChunk * chunk = (*it);

		
		MapPoint * p1 = getMapPointAtPoint(wykobi::rectangle_corner<float>(chunk->rectangle, 0));
		MapPoint * p2 = getMapPointAtPoint(wykobi::rectangle_corner<float>(chunk->rectangle, 1));
		MapPoint * p3 = getMapPointAtPoint(wykobi::rectangle_corner<float>(chunk->rectangle, 2));
		MapPoint * p4 = getMapPointAtPoint(wykobi::rectangle_corner<float>(chunk->rectangle, 3));
		

		/*
		MapPoint * p1 = getMapPointAtPoint(wykobi::rectangle_corner<float>(chunk->rectangle, 0), 1500);
		MapPoint * p2 = getMapPointAtPoint(wykobi::rectangle_corner<float>(chunk->rectangle, 1), 1500);
		MapPoint * p3 = getMapPointAtPoint(wykobi::rectangle_corner<float>(chunk->rectangle, 2), 1500);
		MapPoint * p4 = getMapPointAtPoint(wykobi::rectangle_corner<float>(chunk->rectangle, 3), 1500);
		*/
		mapTriangleVector.push_back(std::unique_ptr<MapTriangle>(new MapTriangle(index, type, p1, p2, p3)));
		index++;
		MapTriangle * t1 = mapTriangleVector.back().get();
		mapTriangleVector.push_back(std::unique_ptr<MapTriangle>(new MapTriangle(index, type, p1, p4, p3)));
		index++;
		MapTriangle * t2 = mapTriangleVector.back().get();

		//std::cout << index << "\n";

		t1->mapChunk = chunk;
		t2->mapChunk = chunk;

		chunk->triangles.push_back(t1);
		chunk->triangles.push_back(t2);
	}

	generateTriangleRelations();

}

void MapEditor::updateVertexArrays() {
	
	for (auto it = groundVertexArrayVector.begin(); it != groundVertexArrayVector.end(); ++it) {
		(*it).clear();
	}

	for (auto it = mapChunkList.begin(); it != mapChunkList.end(); ++it) {
		MapChunk * chunk = (*it);

		float texRectOffsetX = (float)chunk->x * chunkWidth;
		float texRectOffsetY = (float)chunk->y * chunkHeight;

		for (MapTriangle * triangle : chunk->triangles) {

			//if (triangle->type == 1) {
			//	continue;
			//}

			std::vector<sf::Vector2f> triangle_vector = triangle->getSfTriangle();
			for (int i = 0; i < 3; i++) {
				sf::Vertex vertex(triangle_vector[i], sf::Vector2f(triangle_vector[i].x - texRectOffsetX, triangle_vector[i].y - texRectOffsetY));
				groundVertexArrayVector[triangle->type].append(vertex);
			}
		}
	}

	//Tools
	toolVertexArray.clear();
	if (drawToolEnabled) {
		for (wykobi::triangle<float, 2> & triangle : drawToolTranslatedAndTriangulated) {
			for (int i = 0; i < 3; i++) {
				toolVertexArray.append(Math::wykobiPointToSfVertex(triangle[i]));
			}
		}
	}

	//Debug
	debugLineVertexArray.clear();
	if (showTriangleRelations) {
		for (MapChunk * chunk : mapChunkList) {
			for (MapTriangle * origin_triangle : chunk->triangles) {
				wykobi::point2d<float> p1 = origin_triangle->getCentre();
				for (MapTriangle * target_triangle : origin_triangle->relations) {
					wykobi::point2d<float> p2 = target_triangle->getCentre();
					debugLineVertexArray.append(Math::wykobiPointToSfVertex(p1));
					debugLineVertexArray.append(Math::wykobiPointToSfVertex(p2));
				}
			}
		}
	}
	if (showChunkRelations) {
		for (MapChunk * origin_chunk : mapChunkList) {
			wykobi::point2d<float> p1 = wykobi::make_point<float>(chunkWidth * (float)origin_chunk->x + chunkWidth / 2, chunkHeight * (float)origin_chunk->y + chunkHeight / 2);
			for (MapChunk * target_chunk : origin_chunk->relations) {
				wykobi::point2d<float> p2 = wykobi::make_point<float>(chunkWidth * (float)target_chunk->x + chunkWidth / 2, chunkHeight * (float)target_chunk->y + chunkHeight / 2);
				debugLineVertexArray.append(Math::wykobiPointToSfVertex(p1));
				debugLineVertexArray.append(Math::wykobiPointToSfVertex(p2));
			}
		}
	}
	if (showTriangleEdges) {
		for (MapChunk * chunk : mapChunkList) {
			for (MapTriangle * triangle : chunk->triangles) {
				auto edges = triangle->getSfTriangle();
				debugLineVertexArray.append(edges[0]);
				debugLineVertexArray.append(edges[1]);
				debugLineVertexArray.append(edges[1]);
				debugLineVertexArray.append(edges[2]);
				debugLineVertexArray.append(edges[2]);
				debugLineVertexArray.append(edges[0]);
			}
		}
	}
	if (showChunkEdges) {
		for (MapChunk * chunk : mapChunkList) {
			float x = (float)chunk->x;
			float y = (float)chunk->y;
			sf::Vertex v1, v2, v3, v4;
			v1.position = sf::Vector2f(x * chunkHeight, y * chunkHeight);
			v2.position = sf::Vector2f(x * chunkHeight + chunkHeight, y * chunkHeight);
			v3.position = sf::Vector2f(x * chunkHeight + chunkHeight, y * chunkHeight + chunkHeight);
			v4.position = sf::Vector2f(x * chunkHeight, y * chunkHeight + chunkHeight);
			debugLineVertexArray.append(v1);
			debugLineVertexArray.append(v2);
			debugLineVertexArray.append(v2);
			debugLineVertexArray.append(v3);
			debugLineVertexArray.append(v3);
			debugLineVertexArray.append(v4);
			debugLineVertexArray.append(v4);
			debugLineVertexArray.append(v1);
		}
	}

}

void MapEditor::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	if (textureContainer == nullptr) {
		return;
	}

	//draw ground
	states.transform *= getTransform();
	for (int i = 0; i < Global::numberOfTypes; i++) {
		states.texture = &textureContainer->groundTextureVector[i];
		target.draw(groundVertexArrayVector[i], states);
	}

	//Tools
	target.draw(toolVertexArray);

	//Debug
	target.draw(debugLineVertexArray);
}