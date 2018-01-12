//Author: Sivert Andresen Cubedo

#include "MapEditor.hpp"

MapEditor::MapEditor() {
}

void MapEditor::createMap(wykobi::vector2d<float> & chunk_size, wykobi::vector2d<int> & map_size, MapGroundType number_of_types, MapGroundType default_type) {
	m_map = std::unique_ptr<Map>(new Map(chunk_size, map_size, number_of_types));

	applyChunkRelation(*m_map);

	//make default triangles
	
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

//void MapEditor::fillChunk(MapChunk & chunk, MapGroundType type) {
//
//}

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
	if (wykobi::robust_collinear(triangle[0], triangle[1], triangle[0])) {
		return false;
	}
	for (std::size_t i = 0; i < path.size(); ++i)
	{
		if ((i != prev) && (i != next) && (i != index))
		{
			if (point_in_triangle(path[i]->getPos(), triangle))
			{
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

//end