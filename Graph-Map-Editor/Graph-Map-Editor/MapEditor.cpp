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
	EditorPath path = outerChunkPath(chunk);
	
	std::vector<EditorTriangle> tri_vec = path.triangulate();

	for (EditorTriangle & tri : tri_vec) {
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

EditorPath MapEditor::outerChunkPath(MapChunk & chunk) {
	EditorPath path;
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

std::map<MapGroundType, std::vector<EditorPath>> MapEditor::mergeTrianglesInChunk(MapChunk & chunk) {
	std::map<MapGroundType, std::vector<EditorPath>> out_map;

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
			EditorPath current_path;
			EditorTriangle current_triangle = triangle_order.front()->getPoints();
			current_path.insert(current_path.end(), current_triangle.begin(), current_triangle.end());
			for (auto it_2 = triangle_order.begin() + 1; it_2 != triangle_order.end(); ++it_2) {
				current_triangle = (*it_2)->getPoints();
				auto shared_edge_vec = current_path.sharedEdge(current_triangle);
				if (shared_edge_vec.size() == 1) {
					//if share one edge, then insert not-included point inbetween iterators
					MapPoint* p = current_triangle.getOddPoint((*shared_edge_vec.front().first), (*shared_edge_vec.front().second));
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