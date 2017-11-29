//Author: Sivert Andresen Cubedo

#include "Math.hpp"

std::vector<int> Math::Clipper::findFirstLegalPolygonCut(wykobi::polygon<float, 2> & poly, int start) {
	std::vector<int> vec;
	if (poly.size() <= 3) {
		return vec;
	}
	int pos_start = start + 1;
	if (pos_start == poly.size()) pos_start = 0;
	int neg_start = start - 1;
	if (neg_start == -1) neg_start = poly.size() - 1;
	int a = pos_start;
	int b = neg_start;
	int i;
	while (a != b && !isPointsInShapeAdjacent<wykobi::polygon<float, 2>>(poly, a, b)) {
		//plus side
		i = neg_start;
		do {
			if (isSegmentFromPolygonInsidePolygon(poly, a, i)) {
				//is this then we have a solution.
				vec.push_back(a);
				vec.push_back(i);
				return vec;
			}
			i--;
			if (i == -1) i = poly.size() - 1;
		} while (i != b);
		//minus side
		i = pos_start;
		do {
			if (isSegmentFromPolygonInsidePolygon(poly, b, i)) {
				//is this then we have a solution.
				vec.push_back(b);
				vec.push_back(i);
				return vec;
			}
			i++;
			if (i == poly.size()) i = 0;
		} while (i != a);

		a++;
		if (a == poly.size()) a = 0;
		b--;
		if (b == -1) b = poly.size() - 1;
	}
	return vec;
}

std::vector<wykobi::polygon<float, 2>> Math::Clipper::dividePolygonFromExistingPoints(wykobi::polygon<float, 2> & poly, int v1, int v2) {
	std::vector<wykobi::polygon<float, 2>> vec;
	if (!isSegmentFromPolygonInsidePolygon(poly, v1, v2)) return vec;
	vec.reserve(2);
	int i;
	wykobi::polygon<float, 2> temp_poly;
	//poly1
	//v1 -> v2 -> +
	temp_poly.push_back(poly[v1]);
	i = v2;
	do {
		temp_poly.push_back(poly[i]);
		i++;
		if (i == poly.size()) i = 0;
	} while (i != v1);
	vec.push_back(temp_poly);
	//poly2
	//v1 -> v2 -> -
	temp_poly.clear();
	temp_poly.push_back(poly[v1]);
	i = v2;
	do {
		temp_poly.push_back(poly[i]);
		i--;
		if (i == -1) i = poly.size() - 1;
	} while (i != v1);
	vec.push_back(temp_poly);
	return vec;
}

std::vector<wykobi::polygon<float, 2>> Math::Clipper::removeSubPolygon(wykobi::polygon<float, 2> & poly) {
	//divide one case then call recursevly.
	std::vector<wykobi::polygon<float, 2>> vec;
	int index_1;
	int index_2;
	//find equal points
	int i = 0;
	int j = 0;
	bool b = false;
	while (i < poly.size()) {
		j = i + 2;
		while (j < poly.size()) {
			if (poly[i] == poly[j]) {
				index_1 = i;
				index_2 = j;
				b = true;
				break;
			}
			j++;
		}
		if (b) break;
		i++;
	}
	if (b) {
		//if this then there is a subPolygon between index_1 and index_2
		std::vector<int> seg_indexes = findFirstLegalPolygonCut(poly, index_1);
		if (seg_indexes.empty()) {
			//if this then there was no legal cut
			vec.push_back(poly);
		}
		else {
			//if this then there was a legal cut
			std::vector<wykobi::polygon<float, 2>> temp_polys_1 = dividePolygonFromExistingPoints(poly, seg_indexes[0], seg_indexes[1]);
			for (wykobi::polygon<float, 2> & p : temp_polys_1) {
				std::vector<wykobi::polygon<float, 2>> temp_polys_2 = removeSubPolygon(p);
				vec.insert(vec.end(), temp_polys_2.begin(), temp_polys_2.end());
			}
		}
	}
	else {
		//if this then there is no subPolygon
		vec.push_back(poly);
	}
	return vec;
}
std::vector<wykobi::polygon<float, 2>> Math::Clipper::removeSubPolygon(std::vector<wykobi::polygon<float, 2>> & poly_vec) {
	std::vector<wykobi::polygon<float, 2>> new_poly_vec;
	for (wykobi::polygon<float, 2> & poly : poly_vec) {
		auto temp = removeSubPolygon(poly);
		//std::cout << "hello -> " << temp.size() << "\n";
		new_poly_vec.insert(new_poly_vec.end(), temp.begin(), temp.end());
		//poly = removeDuplicates<wykobi::polygon<float, 2>>(poly);
		//new_poly_vec.push_back(poly);
	}
	return new_poly_vec;
}

bool Math::isSegmentPointsEqual(wykobi::segment<float, 2> & seg1, wykobi::segment<float, 2> & seg2) {
	if ((seg1[0] == seg2[0] && seg1[1] == seg2[1]) || (seg1[0] == seg2[1] && seg1[1] == seg2[0])) {
		return true;
	}
	else {
		return false;
	}
}

bool Math::isSegmentFromPolygonInsidePolygon(wykobi::polygon<float, 2> & poly, int v1, int v2) {
	//if this then false
	if (isPointsInShapeAdjacent<wykobi::polygon<float, 2>>(poly, v1, v2)) {
		return false;
	}

	//edges not to test
	int v1_not_test1 = v1;
	int v1_not_test2 = v1 - 1;
	int v2_not_test1 = v2;
	int v2_not_test2 = v2 - 1;

	wykobi::segment<float, 2> segment = wykobi::make_segment<float>(poly[v1], poly[v2]);

	//check for intersects, if a intersect then it can't be fully inside.
	for (int i = 0; i < poly.size(); i++) {
		if (i == v1_not_test1 || i == v1_not_test2 || i == v2_not_test1 || i == v2_not_test2) {
			continue;
		}
		else {
			wykobi::segment<float, 2> test_segment = wykobi::edge<float>(poly, i);
			if (wykobi::intersect<float>(segment, test_segment)) {
				return false;
			}
		}
	}

	//check if middle point of segment is inside polygon, if true then segment is inside
	wykobi::point2d<float> middle_point = wykobi::segment_mid_point<float>(segment);
	if (!wykobi::point_in_polygon<float>(middle_point, poly)) {
		return false;
	}
	return true;
}

bool Math::isSegmentFromPolygonSelfIntersecting(wykobi::polygon<float, 2> & polygon, int index, wykobi::point2d<float> point) {
	wykobi::segment<float, 2> segment = wykobi::make_segment<float>(polygon[index], point);
	int not_test1 = index;
	int not_test2 = index - 1;
	if (not_test2 == -1) not_test2 = polygon.size() - 1;
	if (isSegmentPointsEqual(segment, wykobi::edge<float>(polygon, not_test1)) || isSegmentPointsEqual(segment, wykobi::edge<float>(polygon, not_test2))) {
		return true;
	}
	if (wykobi::point_on_segment<float>(point, wykobi::edge<float>(polygon, not_test1)) || wykobi::point_on_segment<float>(point, wykobi::edge<float>(polygon, not_test2))) {
		return true;
	}
	for (int i = 0; i < polygon.size(); i++) {
		if (i == not_test1 || i == not_test2) {
			continue;
		}
		else {
			if (wykobi::intersect<float>(segment, wykobi::edge<float>(polygon, i))) {
				return true;
			}
		}
	}
	return false;
}

std::vector<wykobi::polygon<float, 2>> Math::Clipper::removePolygonHull(wykobi::polygon<float, 2> & polygon, wykobi::polygon<float, 2> & hull) {
	std::vector<wykobi::polygon<float, 2>> out_polygons;
	//check if hull is inside polygon
	if (!isPolygonInsidePolygon(hull, polygon)) {
		//if this then hull is not inside polygon.
		out_polygons.push_back(polygon);
		return out_polygons;
	}
	//find two independent segments that go from hull to segment from existing verticies.
	int seg1_hull_side;
	int seg1_polygon_side;
	wykobi::segment<float, 2> seg1;
	bool solution = false;
	for (int i = 0; i < polygon.size(); i++) {
		for (int j = 0; j < hull.size(); j++) {
			//if this then segment is legal
			if (!Math::isSegmentFromPolygonSelfIntersecting(polygon, i, hull[j]) && 
				!Math::isSegmentFromPolygonSelfIntersecting(hull, j, polygon[i])) 
			{
				seg1_hull_side = j;
				seg1_polygon_side = i;
				seg1 = wykobi::make_segment<float>(polygon[seg1_polygon_side], hull[seg1_hull_side]);
				solution = true;
				break;
			}
		}
		if (solution) break;
	}
	int seg2_hull_side;
	int seg2_polygon_side;
	wykobi::segment<float, 2> seg2;
	solution = false;
	for (int i = 0; i < polygon.size(); i++) {
		if (i == seg1_polygon_side) continue;
		for (int j = 0; j < hull.size(); j++) {
			if (j == seg1_hull_side) continue;
			//if this then segment is legal
			if (!Math::isSegmentFromPolygonSelfIntersecting(polygon, i, hull[j]) &&
				!Math::isSegmentFromPolygonSelfIntersecting(hull, j, polygon[i]) &&
				!wykobi::intersect<float>(seg1, wykobi::make_segment<float>(polygon[i], hull[j]))) 
			{	
				seg2_hull_side = j;
				seg2_polygon_side = i;
				seg2 = wykobi::make_segment<float>(polygon[seg2_polygon_side], hull[seg2_hull_side]);
				solution = true;
				break;
			}
		}
		if (solution) break;
	}
	//Divide polygon in to 2 polygons at seg1 and seg2
	out_polygons.reserve(2);
	{
		int i;
		int stage;
		wykobi::polygon<float, 2> forward_forward;
		wykobi::polygon<float, 2> forward_backward;
		wykobi::polygon<float, 2> backward_forward;
		wykobi::polygon<float, 2> backward_backward;
		i = seg1_polygon_side;
		stage = 0;
		//forward -> forward
		while (true) {
			if (stage == 0) {
				if (i == polygon.size()) i = 0;
				forward_forward.push_back(polygon[i]);
				if (i == seg2_polygon_side) {
					stage = 1;
					i = seg2_hull_side;
				}
				else {
					i++;
				}
			}
			else if (stage == 1) {
				if (i == hull.size()) i = 0;
				forward_forward.push_back(hull[i]);
				if (i == seg1_hull_side) {
					break;
				}
				else {
					i++;
				}
			}
		}
		//forward -> backward
		i = seg1_polygon_side;
		stage = 0;
		while (true) {
			if (stage == 0) {
				if (i == polygon.size()) i = 0;
				forward_backward.push_back(polygon[i]);
				if (i == seg2_polygon_side) {
					stage = 1;
					i = seg2_hull_side;
				}
				else {
					i++;
				}
			}
			else if (stage == 1) {
				if (i == -1) i = hull.size() - 1;
				forward_backward.push_back(hull[i]);
				if (i == seg1_hull_side) {
					break;
				}
				else {
					i--;
				}
			}
		}
		//backward -> forward
		i = seg1_polygon_side;
		stage = 0;
		while (true) {
			if (stage == 0) {
				if (i == -1) i = polygon.size() - 1;
				backward_forward.push_back(polygon[i]);
				if (i == seg2_polygon_side) {
					stage = 1;
					i = seg2_hull_side;
				}
				else {
					i--;
				}
			}
			else if (stage == 1) {
				if (i == hull.size()) i = 0;
				backward_forward.push_back(hull[i]);
				if (i == seg1_hull_side) {
					break;
				}
				else {
					i++;
				}
			}
		}
		//backward -> backward
		i = seg1_polygon_side;
		stage = 0;
		while (true) {
			if (stage == 0) {
				if (i == -1) i = polygon.size() - 1;
				backward_backward.push_back(polygon[i]);
				if (i == seg2_polygon_side) {
					stage = 1;
					i = seg2_hull_side;
				}
				else {
					i--;
				}
			}
			else if (stage == 1) {
				if (i == -1) i = hull.size() - 1;
				backward_backward.push_back(hull[i]);
				if (i == seg1_hull_side) {
					break;
				}
				else {
					i--;
				}
			}
		}
		if (wykobi::area<float>(forward_forward) < wykobi::area<float>(forward_backward)) {
			out_polygons.push_back(forward_forward);
		}
		else {
			out_polygons.push_back(forward_backward);
		}
		if (wykobi::area<float>(backward_forward) < wykobi::area<float>(backward_backward)) {
			out_polygons.push_back(backward_forward);
		}
		else {
			out_polygons.push_back(backward_backward);
		}
	}
	return out_polygons;
}

std::vector<wykobi::polygon<float, 2>> Math::Clipper::removePolygonHull(wykobi::polygon<float, 2> & poly, std::vector<wykobi::polygon<float, 2>> & hull_vec) {
	std::list<wykobi::polygon<float, 2>> poly_list;
	poly_list.push_back(poly);
	for (wykobi::polygon<float, 2> & hull : hull_vec) {
		bool hull_found = false;
		for (auto it = poly_list.begin(); it != poly_list.end(); ++it) {
			wykobi::polygon<float, 2> & p = (*it);
			if (isPolygonInsidePolygon(hull, p)) {
				auto temp = removePolygonHull(p, hull);
				poly_list.erase(it);
				poly_list.insert(poly_list.end(), temp.begin(), temp.end());
				hull_found = true;
				break;
			}
		}
		if (!hull_found) {
			std::vector<wykobi::polygon<float, 2>> overlapping_poly_1;
			std::vector<wykobi::polygon<float, 2>> overlapping_poly_2;
			auto it = poly_list.begin();
			while (it != poly_list.end()) {
				wykobi::polygon<float, 2> & p = (*it);
				if (isPolygonsOverlapping(p, hull)) {
					overlapping_poly_1.push_back(p);
					it = poly_list.erase(it);
				}
				else {
					++it;
				}
			}
			for (wykobi::polygon<float, 2> & p : overlapping_poly_1) {
				auto temp = clipPolygonDifference(p, hull);
				overlapping_poly_2.insert(overlapping_poly_2.end(), temp.begin(), temp.end());
			}
			poly_list.insert(poly_list.begin(), overlapping_poly_2.begin(), overlapping_poly_2.end());
		}
	}
	std::vector<wykobi::polygon<float, 2>> out_vec;
	out_vec.insert(out_vec.end(), poly_list.begin(), poly_list.end());
	return out_vec;
}
std::vector<wykobi::polygon<float, 2>> Math::Clipper::removePolygonHull(std::vector<wykobi::polygon<float, 2>> & poly_vec, std::vector<wykobi::polygon<float, 2>> & hull_vec) {
	std::vector<wykobi::polygon<float, 2>> out_poly_vec;
	for (wykobi::polygon<float, 2> & p : poly_vec) {
		auto temp = removePolygonHull(p, hull_vec);
		out_poly_vec.insert(out_poly_vec.end(), temp.begin(), temp.end());
	}
	return out_poly_vec;
}

wykobi::polygon<float, 2> Math::insertPointsOnEdgesOfPolygon(wykobi::polygon<float, 2> poly, std::vector<wykobi::point2d<float>> points) {
	int i = 0;
	while (i < poly.size()) {
		wykobi::segment<float, 2> poly_seg = wykobi::edge<float>(poly, i);
		int insert_index = i + 1;
		if (insert_index == poly.size()) insert_index = 0;
		bool inserted = false;
		for (auto point_it = points.begin(); point_it != points.end(); ++point_it) {
			wykobi::point2d<float> test_point = *point_it;
			if (wykobi::point_on_segment<float>(test_point, poly_seg) && !(test_point == poly_seg[0]) && !(test_point == poly_seg[1])) {
				poly = insertPointInPolygon(poly, test_point, insert_index);
				point_it = points.erase(point_it);
				inserted = true;
				break;
			}
		}
		if (!inserted) {
			i++;
		}
	}
	return poly;
}
std::vector<wykobi::polygon<float, 2>> Math::insertPointsOnEdgesOfPolygon(std::vector<wykobi::polygon<float, 2>> vec, std::vector<wykobi::point2d<float>> & points) {
	for (int i = 0; i < vec.size(); i++) {
		vec[i] = insertPointsOnEdgesOfPolygon(vec[i], points);
	}
	return vec;
}

wykobi::polygon<float, 2> Math::insertTriangleInPolygonSharedVerticies(wykobi::polygon<float, 2> poly, wykobi::triangle<float, 2> tri) {
	//check if 3 shared verticies
	for (int i = 0; i < poly.size(); i++) {
		std::vector<int> poly_tri(3);
		poly_tri[0] = i;
		poly_tri[1] = poly_tri[0] + 1;
		if (poly_tri[1] == poly.size()) poly_tri[1] = 0;
		poly_tri[2] = poly_tri[1] + 1;
		if (poly_tri[2] == poly.size()) poly_tri[2] = 0;
		std::vector<wykobi::point2d<float>> tri_tri(3);
		for (int j = 0; j < 3; j++) {
			tri_tri[j] = tri[j];
		}
		for (int j = 0; j < 3; j++) {
			int k = 0;
			while (k < tri_tri.size()) {
				if (poly[poly_tri[j]] == tri_tri[k]) {
					tri_tri.erase(tri_tri.begin() + k);
				}
				else {
					k++;
				}
			}
		}
		//if triangle shares 3 verticies.
		if (tri_tri.empty()) {	
			poly.erase(poly_tri[1]);
			return poly;
		}
	}
	//check if 2 shared verticies
	for (int i = 0; i < poly.size(); i++) {
		std::vector<int> poly_tri(2);
		poly_tri[0] = i;
		poly_tri[1] = poly_tri[0] + 1;
		if (poly_tri[1] == poly.size()) poly_tri[1] = 0;
		std::vector<wykobi::point2d<float>> tri_tri(3);
		for (int j = 0; j < 3; j++) {
			tri_tri[j] = tri[j];
		}
		for (int j = 0; j < 2; j++) {
			int k = 0;
			while (k < tri_tri.size()) {
				if (poly[poly_tri[j]] == tri_tri[k]) {
					tri_tri.erase(tri_tri.begin() + k);
				}
				else {
					k++;
				}
			}
		}
		//if triangle shares 2 verticies.
		if (tri_tri.size() == 1) {
			return insertPointInPolygon(poly, tri_tri.front(), poly_tri[1]);
		}
	}
	//Triangle shares 0 or 1 verticies and can not be inserted.
	return poly;
}

wykobi::polygon<float, 2> Math::insertPointInPolygon(wykobi::polygon<float, 2> poly, wykobi::point2d<float> & point, int index) {
	if (index < 0 || index > poly.size()) {
		return poly;
	}
	if (index == poly.size()) {
		poly.push_back(point);
		return poly;
	}
	std::vector<wykobi::point2d<float>> point_list;
	point_list.reserve(poly.size() + 1);
	for (int i = 0; i < poly.size(); i++) {
		if (i == index) {
			point_list.push_back(point);
		}
		point_list.push_back(poly[i]);
	}
	return wykobi::make_polygon<float>(point_list);
}

std::vector<wykobi::polygon<float, 2>> Math::mergeTrianglesSharedVertices(std::vector<wykobi::triangle<float, 2>> & triangles) {
#if 1
	std::list<wykobi::triangle<float, 2>> pending;
	std::list<wykobi::triangle<float, 2>> failed;
	std::vector<wykobi::polygon<float, 2>> finished_polygons;

	pending.insert(pending.end(), triangles.begin(), triangles.end());

	while (!pending.empty()) {
		wykobi::polygon<float, 2> current_polygon = wykobi::make_polygon<float>(pending.front());
		pending.pop_front();

		while (!pending.empty()) {
			
		}


	}

	return finished_polygons;

#elif 0
	std::list<wykobi::triangle<float, 2>> pending;
	std::list<wykobi::triangle<float, 2>> failed;
	std::list<wykobi::triangle<float, 2>> added;
	std::vector<wykobi::polygon<float, 2>> finished_polygons;
	for (wykobi::triangle<float, 2> & tri : triangles) {
		pending.push_back(tri);
	}
	wykobi::polygon<float, 2> current_polygon = wykobi::make_polygon<float>(pending.front());
	pending.pop_front();
	while (true) {
		wykobi::triangle<float, 2> current_test_triangle = pending.front();
		pending.pop_front();
		bool solution = false;
		for (int i = 0; i < current_polygon.size(); i++) {
			int a = i;
			int b = i + 1;
			if (b == current_polygon.size()) {
				b = 0;
			}
			wykobi::point2d<float> & p1 = current_polygon[a];
			wykobi::point2d<float> & p2 = current_polygon[b];
			for (int j = 0; j < 3; j++) {
				wykobi::point2d<float> add_point = current_test_triangle[j];
				std::vector<wykobi::point2d<float>> test_vert;
				test_vert.reserve(2);
				for (int k = 0; k < 3; k++) {
					if (k == j) {
						continue;
					}
					else {
						test_vert.push_back(current_test_triangle[k]);
					}
				}
				if ((p1 == test_vert[0] && p2 == test_vert[1]) || (p1 == test_vert[1] && p2 == test_vert[0])) {
					current_polygon = insertPointInPolygon(current_polygon, add_point, a + 1);
					solution = true;
					break;
				}
			}
			if (solution) {
				break;
			}
		}
		if (solution) {
			added.push_back(current_test_triangle);
		}
		else {
			failed.push_back(current_test_triangle);
		}
		if (pending.empty()) {
			if (failed.empty()) {
				break;
			}
			pending.swap(failed);
			failed.clear();
			finished_polygons.push_back(current_polygon);
			if (!pending.empty()) {
				current_polygon = wykobi::make_polygon<float>(pending.front());
				pending.pop_front();
				if (pending.empty()) {
					finished_polygons.push_back(current_polygon);
					break;
				}
			}
		}
	}
	return finished_polygons;
#endif
}

wykobi::polygon<float, 2> Math::roundWykobiPolygon(wykobi::polygon<float, 2> poly) {
	for (int i = 0; i < poly.size(); i++) {
		poly[i] = roundWykobiPoint(poly[i]);
	}
	return poly;
}

wykobi::triangle<float, 2> Math::roundWykobiTriangle(wykobi::triangle<float, 2> tri) {
	for (int i = 0; i < 3; i++) {
		tri[i] = roundWykobiPoint(tri[i]);
	}
	return tri;
}

wykobi::point2d<float> Math::roundWykobiPoint(wykobi::point2d<float> p) {
	p.x = std::round(p.x);
	p.y = std::round(p.y);
	return p;
}

std::vector<wykobi::polygon<float, 2>> Math::Clipper::mergePolygons(wykobi::polygon<float, 2> & poly1, wykobi::polygon<float, 2> & poly2) {
	std::vector<wykobi::polygon<float, 2>> polygons = { poly1, poly2 };
	return mergePolygons(polygons);
}

std::vector<wykobi::polygon<float, 2>> Math::Clipper::mergePolygons(std::vector<wykobi::polygon<float, 2>> & polygons) {
	ClipperLib::Clipper clipper;
	ClipperLib::Paths solution;

	for (auto & polygon : polygons) {
		clipper.AddPath(wykobiPolygonToClipperPath(polygon), ClipperLib::ptSubject, true);
	}

	clipper.Execute(ClipperLib::ctUnion, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

	std::vector<wykobi::polygon<float, 2>> out;
	out.reserve(solution.size());
	for (int i = 0; i < solution.size(); i++) {
		out.push_back(clipperPathToWykobiPolygon(solution[i]));
	}
	return out;
}

std::vector<wykobi::polygon<float, 2>> Math::Clipper::mergeTriangles(std::vector<wykobi::triangle<float, 2>> & triangles) {
	std::vector<wykobi::polygon<float, 2>> polygons;
	polygons.reserve(triangles.size());
	for (auto tri : triangles) {
		polygons.push_back(wykobi::make_polygon<float>(tri));
	}
	return mergePolygons(polygons);
}

bool Math::isPolygonsOverlapping(wykobi::polygon<float, 2> & poly1, wykobi::polygon<float, 2> & poly2) {
	if (isPolygonInsidePolygon(poly1, poly2) || isPolygonInsidePolygon(poly2, poly1)) {
		return true;
	}
	for (int i = 0; i < poly1.size(); i++) {
		wykobi::segment<float, 2> seg1 = wykobi::edge<float>(poly1, i);
		for (int j = 0; j < poly2.size(); j++) {
			wykobi::segment<float, 2> seg2 = wykobi::edge<float>(poly2, j);
			if (wykobi::intersect<float>(seg1, seg2)) {
				return true;
			}
		}
	}
	return false;
}

bool Math::isTrianglesOverlapping(wykobi::triangle<float, 2> & tri1, wykobi::triangle<float, 2> & tri2) {
	if (isTriangleInsideTriangle(tri1, tri2) || isTriangleInsideTriangle(tri2, tri1)) {
		return true;
	}
	for (int i = 0; i < 3; i++) {
		wykobi::segment<float, 2> seg1 = wykobi::edge<float>(tri1, i);
		for (int j = 0; j < 3; j++) {
			wykobi::segment<float, 2> seg2 = wykobi::edge<float>(tri2, j);
			if (wykobi::intersect<float>(seg1, seg2)) {
				return true;
			}
		}
	}
	return false;
}

bool Math::trianglesSharingEdge(wykobi::triangle<float, 2> & tri1, wykobi::triangle<float, 2> & tri2) {
	for (int i = 0; i < 3; i++) {
		wykobi::segment<float, 2> seg1 = wykobi::edge<float>(tri1, i);
		
		seg1 = wykobi::shorten_segment<float>(seg1, 2.f);
		
		for (int j = 0; j < 3; j++) {
			wykobi::segment<float, 2> seg2 = wykobi::edge<float>(tri2, j);

			seg2 = wykobi::shorten_segment<float>(seg2, 2.f);

			if (wykobi::intersect<float>(seg1, seg2) && wykobi::parallel<float>(seg1, seg2)) {
				return true;
			}
		}
	}
	return false;
}

bool Math::isPolygonInsidePolygon(wykobi::polygon<float, 2> & subject, wykobi::polygon<float, 2> & outer) {
	for (int i = 0; i < subject.size(); i++) {
		if (!wykobi::point_in_polygon<float>(subject[i], outer)) {
			return false;
		}
	}
	for (int i = 0; i < outer.size(); i++) {
		wykobi::segment<float, 2> outer_seg = wykobi::edge<float>(outer, i);
		for (int j = 0; j < subject.size(); j++) {
			wykobi::segment<float, 2> subject_seg = wykobi::edge<float>(subject, j);
			if (wykobi::intersect<float>(outer_seg, subject_seg)) {
				return false;
			}
		}
	}
	return true;
}

std::vector<wykobi::triangle<float, 2>> Math::Clipper::divideTriangleWithPointInside(wykobi::triangle<float, 2> & tri, wykobi::point2d<float> & point) {
	if (wykobi::point_in_triangle(point, tri)) {
		int corner = 0;
		for (int i = 1; i < 3; i++) {
			if (wykobi::distance<float>(tri[i], point) < wykobi::distance(tri[corner], point)) {
				corner = i;
			}
		}
		return divideTriangleWithPoint(tri, point, corner);
	}
	else {
		return std::vector<wykobi::triangle<float, 2>>{ tri };
	}
}

std::vector<wykobi::triangle<float, 2>> Math::Clipper::divideTriangleWithPoint(wykobi::triangle<float, 2> & tri, wykobi::point2d<float> & point, int corner) {
	wykobi::line<float, 2> line = wykobi::make_line<float>(tri[corner], point);
	wykobi::segment<float, 2> segment;
	int c = 0;
	for (int i = 0; i < 3; i++) {
		if (i != corner) {
			segment[c] = tri[i];
			c++;
		}
	}
	if (wykobi::intersect<float>(segment, line)) {
		wykobi::point2d<float> intersect_point = wykobi::intersection_point<float>(segment, line);
	
		std::vector<wykobi::triangle<float, 2>> out_triangles;
		out_triangles.reserve(2);

		for (int i = 0; i < 3; i++) {
			if (i != corner) {
				out_triangles.push_back(wykobi::make_triangle<float>(tri[corner], intersect_point, tri[i]));
			}
		}
		return out_triangles;
	}
	else {
		return std::vector<wykobi::triangle<float, 2>>{ tri };
	}

	
	
}

bool Math::isTriangleInsideTriangle(wykobi::triangle<float, 2> & subject, wykobi::triangle<float, 2> & outer) {
	for (int i = 0; i < 3; i++) {
		if (!wykobi::point_in_triangle<float>(subject[i], outer)) {
			return false;
		}
	}
	return true;
}

std::vector<wykobi::point2d<float>> Math::intersectLineRectanglePoints(wykobi::line<float, 2> & line, wykobi::rectangle<float> & rect) {
	std::vector<wykobi::segment<float, 2>> rect_segments;
	rect_segments.reserve(4);

	rect_segments.push_back(wykobi::make_segment<float>(wykobi::rectangle_corner<float>(rect, 0), wykobi::rectangle_corner<float>(rect, 1)));
	rect_segments.push_back(wykobi::make_segment<float>(wykobi::rectangle_corner<float>(rect, 1), wykobi::rectangle_corner<float>(rect, 2)));
	rect_segments.push_back(wykobi::make_segment<float>(wykobi::rectangle_corner<float>(rect, 2), wykobi::rectangle_corner<float>(rect, 3)));
	rect_segments.push_back(wykobi::make_segment<float>(wykobi::rectangle_corner<float>(rect, 3), wykobi::rectangle_corner<float>(rect, 0)));

	std::vector<wykobi::point2d<float>> out;
	out.reserve(2);
	for (wykobi::segment<float, 2> & seg : rect_segments) {
		if (wykobi::intersect(seg, line)) {
			out.push_back(wykobi::intersection_point<float>(seg, line));
		}
	}
	return out;
}


std::vector<wykobi::triangle<float, 2>> Math::triangulatePolygon(wykobi::polygon<float, 2> poly) {
#if 0
	std::vector<wykobi::triangle<float, 2>> triangle_list;
	wykobi::algorithm::polygon_triangulate<wykobi::point2d<float>>(poly, std::back_inserter(triangle_list));
	return triangle_list;

#elif 1


#endif // 0

}

std::vector<wykobi::triangle<float, 2>> Math::Clipper::divideTriangleInTwo(wykobi::triangle<float, 2> & tri, int point) {
	
	wykobi::line<float, 2> line = wykobi::triangle_bisector<float>(tri, point);

	std::vector<wykobi::triangle<float, 2>> out;

	for (int i = 0; i < 3; i++) {
		if (i != point) {
			out.push_back(wykobi::make_triangle<float>(tri[i], line[0], line[1]));
		}
	}
	
	return out;
}

std::vector<wykobi::polygon<float, 2>> Math::Clipper::clipPolygonIntersection(wykobi::polygon<float, 2> & subject, wykobi::polygon<float, 2> & clip) {
	ClipperLib::Clipper clipper;
	ClipperLib::Paths solution;
	clipper.AddPath(wykobiPolygonToClipperPath(subject), ClipperLib::ptSubject, true);
	clipper.AddPath(wykobiPolygonToClipperPath(clip), ClipperLib::ptClip, true);
	clipper.Execute(ClipperLib::ctIntersection, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
	std::vector<wykobi::polygon<float, 2>> out;
	out.reserve(solution.size());
	for (int i = 0; i < solution.size(); i++) {
		out.push_back(clipperPathToWykobiPolygon(solution[i]));
	}
	return out;
}
std::vector<wykobi::polygon<float, 2>> Math::Clipper::clipPolygonIntersection(std::vector<wykobi::polygon<float, 2>> & subject, std::vector<wykobi::polygon<float, 2>> & clip) {
	ClipperLib::Clipper clipper;
	ClipperLib::Paths solution;
	for (wykobi::polygon<float, 2> & poly : subject) {
		clipper.AddPath(wykobiPolygonToClipperPath(poly), ClipperLib::ptSubject, true);
	}
	for (wykobi::polygon<float, 2> & poly : clip) {
		clipper.AddPath(wykobiPolygonToClipperPath(poly), ClipperLib::ptClip, true);
	}
	clipper.Execute(ClipperLib::ctIntersection, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
	std::vector<wykobi::polygon<float, 2>> out;
	out.reserve(solution.size());
	for (int i = 0; i < solution.size(); i++) {
		out.push_back(clipperPathToWykobiPolygon(solution[i]));
	}
	return out;
}

/*
wykobi::polygon<float, 2> Math::Clipper::clipPolygonIntersectionSingle(wykobi::polygon<float, 2> & subject, wykobi::polygon<float, 2> & clip) {
	return clipPolygonIntersection(subject, clip)[0];
}
*/

std::vector<wykobi::polygon<float, 2>> Math::Clipper::clipPolygonDifference(wykobi::polygon<float, 2> & subject, wykobi::polygon<float, 2> & clip) {
	ClipperLib::Clipper clipper;
	ClipperLib::Paths solution;
	clipper.AddPath(wykobiPolygonToClipperPath(subject), ClipperLib::ptSubject, true);
	clipper.AddPath(wykobiPolygonToClipperPath(clip), ClipperLib::ptClip, true);
	clipper.Execute(ClipperLib::ctDifference, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
	std::vector<wykobi::polygon<float, 2>> out;
	out.reserve(solution.size());
	for (int i = 0; i < solution.size(); i++) {
		out.push_back(clipperPathToWykobiPolygon(solution[i]));
	}
	return out;
}
std::vector<wykobi::polygon<float, 2>> Math::Clipper::clipPolygonDifference(std::vector<wykobi::polygon<float, 2>> & subject, std::vector<wykobi::polygon<float, 2>> & clip) {
	ClipperLib::Clipper clipper;
	ClipperLib::Paths solution;
	for (wykobi::polygon<float, 2> & poly : subject) {
		clipper.AddPath(wykobiPolygonToClipperPath(poly), ClipperLib::ptSubject, true);
	}
	for (wykobi::polygon<float, 2> & poly : clip) {
		clipper.AddPath(wykobiPolygonToClipperPath(poly), ClipperLib::ptClip, true);
	}
	clipper.Execute(ClipperLib::ctDifference, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
	std::vector<wykobi::polygon<float, 2>> out;
	out.reserve(solution.size());
	for (int i = 0; i < solution.size(); i++) {
		out.push_back(clipperPathToWykobiPolygon(solution[i]));
	}
	return out;
}

ClipperLib::Path Math::Clipper::wykobiSegmentToClipperPath(wykobi::segment<float, 2> & segment) {
	ClipperLib::Path path;

	ClipperLib::IntPoint p1;
	ClipperLib::IntPoint p2;

	p1.X = (int)std::round(segment[0].x);
	p1.Y = (int)std::round(segment[0].y);

	p2.X = (int)std::round(segment[1].x);
	p2.Y = (int)std::round(segment[1].y);

	path.reserve(2);
	path.push_back(p1);
	path.push_back(p2);
	
	return path;
}

ClipperLib::Path Math::Clipper::wykobiPolygonToClipperPath(wykobi::polygon<float, 2> & poly) {
	ClipperLib::Path path;
	path.reserve(poly.size());
	for (int i = 0; i < poly.size(); i++) {
		ClipperLib::IntPoint p;
		p.X = (int)std::round(poly[i].x);
		p.Y = (int)std::round(poly[i].y);
		path.push_back(p);
	}
	return path;
}

wykobi::polygon<float, 2> Math::Clipper::clipperPathToWykobiPolygon(ClipperLib::Path & path) {
	wykobi::polygon<float, 2> poly;
	poly.reserve(path.size());
	for (int i = 0; i < path.size(); i++) {
		wykobi::point2d<float> p = wykobi::make_point<float>((float)path[i].X, (float)path[i].Y);
		poly.push_back(p);
	}
	return poly;
}

wykobi::rectangle<float> Math::createRectangleFromTriangle(wykobi::triangle<float, 2> & tri) {
	wykobi::point2d<float> low = tri[0];
	wykobi::point2d<float> high = tri[0];
	for (int i = 1; i < 3; i++) {
		if (low.x > tri[i].x) {
			low.x = tri[i].x;
		}
		if (low.y > tri[i].y) {
			low.y = tri[i].y;
		}
		if (high.x < tri[i].x) {
			high.x = tri[i].x;
		}
		if (high.y < tri[i].y) {
			high.y = tri[i].y;
		}
	}
	return wykobi::make_rectangle<float>(low, high);
}
wykobi::rectangle<float> Math::createRectangleFromTriangle(wykobi::triangle<float, 2> & tri, float increase) {
	wykobi::rectangle<float> rect = createRectangleFromTriangle(tri);
	rect[0].x -= increase;
	rect[0].y -= increase;
	rect[1].x += increase;
	rect[1].y += increase;
	return rect;
}

wykobi::rectangle<float> Math::createRectangleFromPolygon(wykobi::polygon<float, 2> & poly) {
	wykobi::point2d<float> low = poly[0];
	wykobi::point2d<float> high = poly[0];
	for (int i = 1; i < poly.size(); i++) {
		if (low.x > poly[i].x) {
			low.x = poly[i].x;
		}
		if (low.y > poly[i].y) {
			low.y = poly[i].y;
		}
		if (high.x < poly[i].x) {
			high.x = poly[i].x;
		}
		if (high.y < poly[i].y) {
			high.y = poly[i].y;
		}
	}
	return wykobi::make_rectangle<float>(low, high);
}
wykobi::rectangle<float> Math::createRectangleFromPolygon(wykobi::polygon<float, 2> & poly, float increase) {
	wykobi::rectangle<float> rect = createRectangleFromPolygon(poly);
	rect[0].x -= increase;
	rect[0].y -= increase;
	rect[1].x += increase;
	rect[1].y += increase;
	return rect;
}

wykobi::rectangle<float> Math::orderRectangle(wykobi::rectangle<float> rect) {
	if (rect[0].x > rect[1].x) {
		float temp = rect[0].x;
		rect[0].x = rect[1].x;
		rect[1].x = temp;
	}
	if (rect[0].y > rect[1].y) {
		float temp = rect[0].y;
		rect[0].y = rect[1].y;
		rect[1].y = temp;
	}
	return rect;
}

bool Math::isPolygonIntersectRectangle(wykobi::polygon<float, 2> poly, wykobi::rectangle<float> rect) {

	rect = orderRectangle(rect);

	std::vector<wykobi::triangle<float, 2>> triangle_list;
	wykobi::algorithm::polygon_triangulate<wykobi::point2d<float>>(poly, std::back_inserter(triangle_list));
	for (auto triangle : triangle_list) {
		if (wykobi::intersect<float>(triangle, rect)) {
			return true;
		}
	}
	return false;
}

float Math::normalizeFloat(float value, float min, float max) {
	return (value - min) / (max - min);
}
float Math::deNormalizeFloat(float value, float min, float max) {
	return (value * (max - min) + min);
}

float Math::distanceBetween2Points(wykobi::point2d<float> & p1, wykobi::point2d<float> & p2) {
	float x = p2.x - p1.x;
	float y = p2.y - p1.y;
	return std::hypotf(x, y);
}

wykobi::point2d<float> Math::averageCentreOfTriangle2d(wykobi::triangle<float, 2> & triangle) {

	float x = 0;
	float y = 0;

	for (int i = 0; i < triangle.size(); i++) {
		x = x + triangle[i].x;
		y = y + triangle[i].y;
	}

	x = x / (float)triangle.size();
	y = y / (float)triangle.size();

	return wykobi::make_point<float>(x, y);
}
wykobi::point2d<float> Math::avarageCentreOfRectangle2d(wykobi::rectangle<float> & rectangle) {
	float x = 0;
	float y = 0;
	for (int i = 0; i < rectangle.size(); i++) {
		x = x + rectangle[i].x;
		y = y + rectangle[i].y;
	}
	x = x / (float)rectangle.size();
	y = y / (float)rectangle.size();
	return wykobi::make_point<float>(x, y);
}

sf::Vector2f Math::wykobiPointToSfVector(wykobi::point2d<float> & p) {
	return sf::Vector2f(p.x, p.y);
}

sf::Vertex Math::wykobiPointToSfVertex(wykobi::point2d<float> & p) {
	sf::Vertex v;
	v.position.x = p.x;
	v.position.y = p.y;
	return v;
}

/*
std::vector<wykobi::segment<float, 2>> Math::delaunayTriangulatePoints(std::vector<wykobi::point2d<float>> & wykobi_points) {
	std::vector<Vector2<float>> points(wykobi_points.size());
	for (int i = 0; i < wykobi_points.size(); i++) {
		points[i].x = wykobi_points[i].x;
		points[i].y = wykobi_points[i].y;
	}

	Delaunay<float> triangulation;
	triangulation.triangulate(points);

	auto edges = triangulation.getEdges();
	
	std::vector<wykobi::segment<float, 2>> segments(edges.size());
	
	for (int i = 0; i < edges.size(); i++) {
		wykobi::point2d<float> p1 = wykobi::make_point<float>(edges[i].p1.x, edges[i].p1.y);
		wykobi::point2d<float> p2 = wykobi::make_point<float>(edges[i].p2.x, edges[i].p2.y);
		segments[i] = wykobi::make_segment<float>(p1, p2);
	}
	return segments;
}
*/

std::vector<std::pair<int, int>> Math::integerLine2d(int x1, int y1, int x2, int y2) {

	std::vector<std::pair<int, int>> line;

	int delta_x(x2 - x1);
	// if x1 == x2, then it does not matter what we set here
	signed char const ix((delta_x > 0) - (delta_x < 0));
	delta_x = std::abs(delta_x) << 1;

	int delta_y(y2 - y1);
	// if y1 == y2, then it does not matter what we set here
	signed char const iy((delta_y > 0) - (delta_y < 0));
	delta_y = std::abs(delta_y) << 1;

	line.push_back(std::pair<int, int>(x1, y1));

	if (delta_x >= delta_y)
	{
		// error may go below zero
		int error(delta_y - (delta_x >> 1));

		while (x1 != x2)
		{
			// reduce error, while taking into account the corner case of error == 0
			if ((error > 0) || (!error && (ix > 0)))
			{
				error -= delta_x;
				y1 += iy;
			}
			// else do nothing

			error += delta_y;
			x1 += ix;

			line.push_back(std::pair<int, int>(x1, y1));
		}
	}
	else
	{
		// error may go below zero
		int error(delta_x - (delta_y >> 1));

		while (y1 != y2)
		{
			// reduce error, while taking into account the corner case of error == 0
			if ((error > 0) || (!error && (iy > 0)))
			{
				error -= delta_y;
				x1 += ix;
			}
			// else do nothing

			error += delta_x;
			y1 += iy;

			line.push_back(std::pair<int, int>(x1, y1));
		}
	}
	return line;
}

wykobi::point2d<float> Math::translateWykobiPoint(wykobi::point2d<float> p, float x, float y) {
	return wykobi::make_point<float>(p.x + x, p.y + y);
}

std::vector<wykobi::point2d<float>> Math::translateWykobiPointVector(std::vector<wykobi::point2d<float>> vec, float x, float y) {
	for (auto it = vec.begin(); it != vec.end(); ++it) {
		(*it) = translateWykobiPoint((*it), x, y);
	}
	return vec;
}

wykobi::point2d<float> Math::rotateWykobiPoint(wykobi::point2d<float> p, wykobi::point2d<float> centre, float angle) {
	//p = translateWykobiPoint(p, -centre.x, -centre.y);
	//float x = p.x * std::cos(angle) - p.y * std::sin(angle);
	//float y = p.y * std::cos(angle) + p.x * std::sin(angle);
	//return translateWykobiPoint(wykobi::make_point<float>(x, y), centre.x, centre.y);
	////angle = angle * 180 / wykobi::PI;
	return wykobi::rotate<float>(angle, p, centre);
}

std::vector<wykobi::point2d<float>> Math::rotateWykobiPointVector(std::vector<wykobi::point2d<float>> vec, wykobi::point2d<float> centre, float angle) {
	for (auto it = vec.begin(); it != vec.end(); ++it) {
		(*it) = rotateWykobiPoint((*it), centre, angle);
	}
	return vec;
}

std::string Math::Debug::toString(wykobi::point2d<float> & point) {
	std::ostringstream str;
	str << "(" << point.x << ", " << point.y << ")";
	return str.str();
}
std::string Math::Debug::toString(wykobi::vector2d<float> & vector) {
	return toString(wykobi::make_point<float>(vector.x, vector.y));
}
std::string Math::Debug::toString(wykobi::segment<float, 2> & seg) {
	std::ostringstream str;
	str << toString(seg[0]) << "->" << toString(seg[1]);
	return str.str();
}
std::string Math::Debug::toString(wykobi::polygon<float, 2> & poly) {
	std::ostringstream str;
	for (int i = 0; i < poly.size(); i++) {
		if (i == poly.size() - 1) {
			str << toString(poly[i]);
		}
		else {
			str << toString(poly[i]) << "->";
		}
	}
	return str.str();
}
std::string Math::Debug::toString(wykobi::triangle<float, 2> & tri) {
	return toString(wykobi::make_polygon<float>(tri));
}
std::string Math::Debug::toString(wykobi::rectangle<float> & rect) {
	return toString(wykobi::make_segment<float>(rect[0], rect[1]));
}

std::string Math::Debug::toCode(wykobi::point2d<float> & point) {
	std::ostringstream str;
	str << "wykobi::make_point<float>";
	str << toString(point);
	return str.str();
}
std::string Math::Debug::toCode(wykobi::polygon<float, 2> & poly) {
	std::ostringstream str;
	str << "wykobi::make_polygon<float>({";
	for (int i = 0; i < poly.size(); i++) {
		if (i == poly.size() - 1) {
			str << toCode(poly[i]);
		}
		else {
			str << toCode(poly[i]) << ",";
		}
	}
	str << "})";
	return str.str();
}


//std::vector<wykobi::polygon<float, 2>> Math::Clipper::removePolygonHull(wykobi::polygon<float, 2> & poly, std::vector<wykobi::polygon<float, 2>> & hull_vec) {
//	/*
//	Psuedo code:
//		for each hull
//			check if inside poly
//				if not then discard
//		for each hull
//			check if inside other hull
//				if true then discard
//		for each hull
//			find two segments between that is not selfinterecting itself and is not selfintersecting poly and is not intersecting and hull and is not intersecting any already found segments
//			if no segments between poly and hull was found then put in failed queue
//		for each failed hull
//			find segments between itself and successful hull
//		make polygons from segments
//		return polygons
//	*/
//
//	std::vector<wykobi::polygon<float, 2>> out_vec;
//
//
//	return out_vec;
//}
//std::vector<wykobi::polygon<float, 2>> Math::Clipper::removePolygonHull(std::vector<wykobi::polygon<float, 2>> & poly_vec, std::vector<wykobi::polygon<float, 2>> & hull_vec) {
//	std::vector<wykobi::polygon<float, 2>> out_poly_vec;
//	std::vector<wykobi::polygon<float, 2>> temp_poly_vec_1;
//	for (wykobi::polygon<float, 2> & p : poly_vec) {
//		//gather hull for p
//		std::vector<wykobi::polygon<float, 2>> temp_hull_vec_1;
//		for (wykobi::polygon<float, 2> & h : hull_vec) {
//			if (isPolygonInsidePolygon(h, p)) {
//				temp_hull_vec_1.push_back(h);
//			}
//		}
//
//		for (wykobi::polygon<float, 2> & h : temp_hull_vec_1) {
//
//		}
//
//	}
//	return out_poly_vec;
//}

/*
	int corner = 0;
	for (int i = 1; i < 3; i++) {
		if (wykobi::distance<float>(tri[i], point) > wykobi::distance(tri[corner], point)) {
			corner = i;
		}
	}

	wykobi::segment<float, 2> intersect_edge;
	int c = 0;
	for (int i = 0; i < 3; i++) {
		if (i != corner) {
			intersect_edge[c] = tri[i];
			c++;
		}
	}

	wykobi::line<float, 2> line = wykobi::make_line<float>(tri[corner], point);

	if (wykobi::intersect(intersect_edge, line)) {
		wykobi::point2d<float> intersect_point = wykobi::intersection_point<float>(intersect_edge, line);

		std::vector<wykobi::triangle<float, 2>> out_triangles;
		out_triangles.reserve(2);

		for (int j = 0; j < 3; j++) {
			if (j != corner) {
				out_triangles.push_back(wykobi::make_triangle<float>(tri[corner], intersect_point, tri[j]));
			}
		}
		return out_triangles;

	}

	return std::vector<wykobi::triangle<float, 2>>{tri};
	*/
	
	/*
	for (int i = 0; i < 3; i++) {
		wykobi::point2d<float> corner = tri[i];
		wykobi::line<float, 2> line = wykobi::make_line<float>(corner, point);
		wykobi::segment<float, 2> seg;
		int c = 0;
		for (int j = 0; j < 3; j++) {
			if (j != i) {
				seg[c] = tri[j];
				c++;
			}
		}

		if (wykobi::intersect<float>(seg, line)) {

			wykobi::point2d<float> intersect = wykobi::intersection_point<float>(seg, line);

			std::vector<wykobi::triangle<float, 2>> out_triangles;

			for (int j = 0; j < 3; j++) {
				if (j != i) {
					out_triangles.push_back(wykobi::make_triangle<float>(corner, intersect, tri[j]));
				}
			}
			return out_triangles;
		}
	}
	*/

/*
std::vector<wykobi::triangle<float, 2>> Math::Clipper::divideTriangleUntilIntersect(wykobi::triangle<float, 2> & subject, wykobi::triangle<float, 2> & intersect) {
	std::vector<std::vector<wykobi::triangle<float, 2>>> triangle_list(3);
	std::vector<std::list<wykobi::triangle<float, 2>>> q(3);
	for (int i = 0; i < 3; i++) {
		q[i].push_back(subject);
	}
	int last;
	while (!q[0].empty() && !q[1].empty() && !q[2].empty()) {

		for (int i = 0; i < 3; i++) {
			wykobi::triangle<float, 2> tri = q[i].front();
			q[i].pop_front();
			if (isTriangleWithinTriangle(intersect, tri)) {
				auto vec = divideTriangleInTwo(tri, i);
				q[i].insert(q[i].end(), vec.begin(), vec.end());
			}
			else {
				triangle_list[i].push_back(tri);
				if (q[i].empty()) {
					last = i;
					break;
				}
			}
		}
	}
	return triangle_list[last];
	/*
	while (!q.empty()) {
		wykobi::triangle<float, 2> tri = q.front();
		q.pop_front();
		if (isTriangleWithinTriangle(intersect, tri)) {
			for (int i = 0; i < 3; i++) {
				auto vec = Math::Clipper::divideTriangleInTwo(tri, i);
				if (!isTriangleWithinTriangle(intersect, vec[0]) && !isTriangleWithinTriangle(intersect, vec[1])) {
					q.insert(q.end(), vec.begin(), vec.end());
					break;
				}
				if (i == 2) {
					q.insert(q.end(), vec.begin(), vec.end());
				}
			}
		}
		else {
			triangle_list.push_back(tri);
		}
	}
}
*/

/*
bool Math::trianglesOverlapping(wykobi::triangle<float, 2> & tri1, wykobi::triangle<float, 2> & tri2) {
	std::vector<wykobi::segment<float, 2>> tri1_seg;
	std::vector<wykobi::segment<float, 2>> tri2_seg;
	tri1_seg.reserve(3);
	tri2_seg.reserve(3);
	for (int i = 0; i < 3; i++) {
		tri1_seg.push_back(wykobi::edge<float>(tri1, i));
		tri2_seg.push_back(wykobi::edge<float>(tri2, i));
	}

	if (isTriangleInsideTriangle(tri1, tri2)) {
		return true;
	}
	if (isTriangleInsideTriangle(tri2, tri1)) {
		return true;
	}
	return false;
}
*/


