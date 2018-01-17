//Author: Sivert Andresen Cubedo

#include "MapEditor.hpp"

MapEditor::MapEditor() {
}

void MapEditor::createMap(wykobi::vector2d<float> chunk_size, wykobi::vector2d<int> map_size, MapGroundType default_type) {
	m_map = std::unique_ptr<Map>(new Map(chunk_size, map_size));
	applyChunkRelation(*m_map);
	fillMap(*m_map, default_type);
}

Map* MapEditor::getMap() {
	return m_map.get();
}

bool MapEditor::empty() {
	return m_map_is_loaded;
}

void MapEditor::clear() {
	m_map_is_loaded = false;
}

void MapEditor::applyChunkRelation(Map & map) {
	for (int x = 0; x < map.getMapSize().x; ++x) {
		for (int y = 0; y < map.getMapSize().y; ++y) {
			wykobi::point2d<int> pos = wykobi::make_point(x, y);
			std::array<wykobi::point2d<int>, 8> rel{
				wykobi::make_point(x - 1, y - 1),
				wykobi::make_point(x, y - 1),
				wykobi::make_point(x + 1, y - 1),
				wykobi::make_point(x + 1, y),
				wykobi::make_point(x + 1, y + 1),
				wykobi::make_point(x, y + 1),
				wykobi::make_point(x - 1, y + 1),
				wykobi::make_point(x - 1, y)
			};
			for (wykobi::point2d<int> & p : rel) {
				if (map.chunkExist(p)) {
					map.getChunk(pos)->getRelations().push_back(map.getChunk(p));
				}
			}
		}
	}
}

void MapEditor::fillChunk(MapChunk & chunk, MapGroundType type) {
	clearChunk(chunk);
	Path path = outerChunkPath(chunk);
	
	//for (MapPoint* p : path) {
	//	std::cout << p->getPos().x << "," << p->getPos().y << "->";
	//}
	//std::cout << "\n";
	
	std::vector<Triangle> tri_vec = triangulatePath(path);

	//std::cout << tri_vec.size() << "\n";

	for (Triangle & tri : tri_vec) {
		chunk.addTriangle(tri, type);
	}
}

void MapEditor::fillMap(Map & map, MapGroundType type) {
	clearSharedPoints(map);
	for (int x = 0; x < map.getMapSize().x; ++x) {
		for (int y = 0; y < map.getMapSize().y; ++y) {
			wykobi::point2d<int> p = wykobi::make_point(x, y);
			fillChunk(*map.getChunk(p), type);
		}
	}
}

void MapEditor::clearChunk(MapChunk & chunk) {
	for (auto it = chunk.getTriangles().begin(); it != chunk.getTriangles().end(); ++it) {
		(*it)->release();
	}
	chunk.getTriangles().clear();
	chunk.getInternalPoints().clear();
}

void MapEditor::clearSharedPoints(Map & map) {
	for (int x = 0; x < map.getMapSize().x; ++x) {
		for (int y = 0; y < map.getMapSize().y; ++y) {
			wykobi::point2d<int> p = wykobi::make_point(x, y);
			map.getChunk(p)->getSharedPoints().clear();
		}
	}
}

Path MapEditor::outerChunkPath(MapChunk & chunk) {
	Path path;
	std::set<MapPoint*> shared_points;
	for (auto & ptr : chunk.getSharedPoints()) {
		shared_points.emplace(ptr.get());
	}
	auto & corner_points = chunk.getCornerPoints();
	for (std::size_t i = 0; i < 4; ++i) {
		std::size_t next = (i < 3) ? i + 1 : 0;
		wykobi::segment<float, 2> seg = wykobi::make_segment(corner_points[i]->getPos(), corner_points[next]->getPos());
		std::vector<MapPoint*> between;
		auto it = shared_points.begin();
		while (it != shared_points.end()) {
			if (wykobi::point_on_segment((*it)->getPos(), seg)) {
				between.push_back((*it));
				it = shared_points.erase(it);
			}
			else {
				++it;
			}
		}
		if (between.size() > 1) {
			std::sort(between.begin(), between.end(), [&](MapPoint* p1, MapPoint* p2)
			{ return (wykobi::distance(corner_points[i]->getPos(), p1->getPos()) < wykobi::distance(corner_points[i]->getPos(), p2->getPos())); }
			);
		}
		path.push_back(corner_points[i]);
		path.insert(path.end(), between.begin(), between.end());
	}
	return path;
}

std::vector<Triangle> MapEditor::triangulatePath(Path path) {
	std::vector<std::array<MapPoint*, 3>> out_vec;
	if (path.size() < 3) {
		return out_vec;
	}
	else if (path.size() == 3) {
		out_vec.push_back({ path[0], path[1], path[2] });
		return out_vec;
	}
	if (pathOrientation(path) != wykobi::Clockwise) {
		std::reverse(path.begin(), path.end());
	}
	while (path.size() > 3) {
		for (std::size_t i = 0; i < path.size(); ++i) {
			if (convexVertex(i, path, wykobi::Clockwise) && vertexIsEar(i, path)) {
				out_vec.push_back(vertexTriangle(i, path));
				path.erase(path.begin() + i);
				break;
			}
		}
	}
	out_vec.push_back(vertexTriangle(1, path));
	return out_vec;
}

int MapEditor::pathOrientation(Path & path) {
	wykobi::polygon<float, 2> poly;
	poly.reserve(path.size());
	for (MapPoint* p : path) {
		poly.push_back(p->getPos());
	}
	return wykobi::polygon_orientation(poly);
}

bool MapEditor::convexVertex(const std::size_t index, const Path & path, const int wykovi_orientation) {
	wykobi::point2d<float> prev;
	wykobi::point2d<float> current = path[index]->getPos();
	wykobi::point2d<float> next;
	if (index == 0) {
		prev = path.back()->getPos();
		next = path[index + 1]->getPos();
	}
	else if (index == path.size() - 1) {
		prev = path[index - 1]->getPos();
		next = path.front()->getPos();
	}
	else {
		prev = path[index - 1]->getPos();
		next = path[index + 1]->getPos();
	}
	return (wykobi::orientation(prev, current, next) == wykovi_orientation);
}

bool MapEditor::vertexIsEar(const std::size_t index, const Path & path) {
	std::size_t prev;
	std::size_t next;
	if (index == 0) {
		prev = path.size() - 1;
		next = index + 1;
	}
	else if (index == path.size() - 1) {
		prev = index - 1;
		next = 0;
	}
	else {
		prev = index - 1;
		next = index + 1;
	}
	wykobi::triangle<float, 2> triangle = wykobi::make_triangle(path[prev]->getPos(), path[index]->getPos(), path[next]->getPos());
	if (wykobi::robust_collinear(triangle[0], triangle[1], triangle[2])) {
		return false;
	}
	for (std::size_t i = 0; i < path.size(); ++i) {
		if ((i != prev) && (i != next) && (i != index)) {
			if (point_in_triangle(path[i]->getPos(), triangle)) {
				return false;
			}
		}
	}
	return true;
}

Triangle MapEditor::vertexTriangle(const std::size_t index, const Path & path) {
	MapPoint* prev;
	MapPoint* current = path[index];
	MapPoint* next;
	if (index == 0) {
		prev = path.back();
		next = path[index + 1];
	}
	else if (index == path.size() - 1) {
		prev = path[index - 1];
		next = path.front();
	}
	else {
		prev = path[index - 1];
		next = path[index + 1];
	}
	return { prev, current, next };
}

std::vector<std::pair<Path::iterator, Path::iterator>> MapEditor::sharedEdge(Path & path, Triangle & triangle) {
	std::vector<std::pair<Path::iterator, Path::iterator>> out_vec;
	for (Triangle::iterator tri_it_1 = triangle.begin(); tri_it_1 != triangle.end(); ++tri_it_1) {
		Path::iterator path_it_1 = std::find(path.begin(), path.end(), (*tri_it_1));
		if (path_it_1 != path.end()) {
			Path::iterator path_it_2 = (path_it_1 != path.end() - 1) ? path_it_1 + 1 : path.begin();
			Triangle::iterator tri_it_2 = std::find(triangle.begin(), triangle.end(), (*path_it_2));
			if (tri_it_2 != triangle.end()) {
				out_vec.push_back(std::pair<Path::iterator, Path::iterator>(path_it_1, path_it_2));
			}
		}
	}
	return out_vec;
}

MapPoint* MapEditor::getOddPoint(Triangle & triangle, MapPoint* p1, MapPoint* p2) {
	Triangle::iterator it_1 = std::find(triangle.begin(), triangle.end(), p1);
	Triangle::iterator it_2 = std::find(triangle.begin(), triangle.end(), p2);
	if (it_1 + 1 != triangle.end() && it_1 + 1 != it_2) {
		return (*it_1 + 1);
	}
	if (it_2 + 1 != triangle.end() && it_2 + 1 != it_1) {
		return (*it_2 + 1);
	}
	return (*triangle.begin());
}

std::map<MapGroundType, std::vector<Path>> MapEditor::mergeTrianglesInChunk(MapChunk & chunk) {
	std::map<MapGroundType, std::vector<Path>> out_map;

	//dfs
	for (auto it = chunk.getTriangles().begin(); it != chunk.getTriangles().end(); ++it) {
		if (!(*it)->visited) {
			MapTriangle* origin_triangle = (*it).get();
			std::vector<MapTriangle*> dfs_stack;
			std::vector<MapTriangle*> triangle_order;
			dfs_stack.push_back(origin_triangle);
			while (!dfs_stack.empty()) {
				MapTriangle* v = dfs_stack.back();
				dfs_stack.pop_back();
				if (!v->visited && v->getType() == origin_triangle->getType() && &v->getChunk() == &chunk) {
					v->visited = true;
					triangle_order.push_back(v);
					for (MapTriangle* w : v->getRelations()) {
						dfs_stack.push_back(w);
					}
				}
			}
			Path current_path;
			Triangle current_triangle = triangle_order.front()->getPoints();
			current_path.insert(current_path.end(), current_triangle.begin(), current_triangle.end());
			for (auto it_2 = triangle_order.begin() + 1; it_2 != triangle_order.end(); ++it_2) {
				current_triangle = (*it_2)->getPoints();
				auto shared_edge_vec = sharedEdge(current_path, current_triangle);
				if (shared_edge_vec.size() == 1) {
					//if share one edge, then insert not-included point inbetween iterators
					MapPoint* p = getOddPoint(current_triangle, (*shared_edge_vec.front().first), (*shared_edge_vec.front().second));
					current_path.insert(shared_edge_vec.front().first, p);
				}
				else if (shared_edge_vec.size() == 2) {
					if (shared_edge_vec[0].second == shared_edge_vec[1].first) {
						//if two edges and the edges are consecutive, then erase middle iterator
						current_path.erase(shared_edge_vec[0].second);
					}
					else if (shared_edge_vec[1].second == shared_edge_vec[0].first) {
						//if two edges and the edges are consecutive, then erase middle iterator
						current_path.erase(shared_edge_vec[1].second);
					}
					else {
						//if two edges and are not consecutive, then hull exist ?????

					}
				}
				else if (shared_edge_vec.size() == 3) {
					//if three edges then triangle is a single hull ?????
					assert(false);
				}
				else {
					//if no shared edges, do nothing (should never happen)
					assert(false);
				}
				
			}


		}
	}
	
	return out_map;
}


//end