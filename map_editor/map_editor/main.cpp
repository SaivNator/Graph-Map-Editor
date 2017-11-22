//Author: Sivert Andresen Cubedo

#include <iostream>

#include <SFML\Graphics.hpp>
#include <wykobi.hpp>
#include <clipper.hpp>

//local
#include "MapEditor.hpp"
#include "Math.hpp"
#include "Graph.hpp"

//Globals
sf::RenderWindow window;
std::string title = "map_editor";
sf::Font arial;

//Delta time
sf::Clock gameClock;		//GC
sf::Time deltaTime;			//DT
float frameRatePerSecond;	//FPS

//Views
sf::View screenView;

Global::TextureContainer textureContainer;
MapEditor mapEditor;

//Buttons
bool keyDown_W = false;
bool keyDown_A = false;
bool keyDown_S = false;
bool keyDown_D = false;

bool keyDown_Q = false;
bool keyDown_E = false;

bool keyDown_escape = false;

bool keyDown_space = false;

bool keyDown_f1 = false;
bool keyDown_f2 = false;
bool keyDown_f3 = false;
bool keyDown_f4 = false;
bool keyDown_f5 = false;
bool keyDown_f6 = false;
bool keyDown_f7 = false;
bool keyDown_f8 = false;
bool keyDown_f9 = false;
bool keyDown_f10 = false;
bool keyDown_f11 = false;
bool keyDown_f12 = false;

//Mouse
bool mouseDown_left = false;
bool mouseDown_middle = false;
bool mouseDown_right = false;

bool mouseBounce_left = false;
bool mouseBounce_Right = false;

int mousePos_x;
int mousePos_y;

//Speed vars
float moveSpeed = 1000.f;	//per second
float zoomSpeed = 1.f;		//per second

#if 0

int main() {


	window.create(sf::VideoMode(1000, 600), title, sf::Style::Default);
	window.setFramerateLimit(60);


	//load fonts
	if (!arial.loadFromFile("fonts/arial.ttf")) {
		std::cout << "Font load failed\n";
		return EXIT_FAILURE;
	}

	if (!textureContainer.init()) {
		std::cout << "Failed to load texture container\n";
		return EXIT_FAILURE;
	}

	mapEditor = MapEditor(&textureContainer);

	mapEditor.newMap(20, 10, 1024, 1024); 
	

	mapEditor.getEditorView() = window.getDefaultView();
	mapEditor.setShowChunkEdges(true);
	mapEditor.setShowChunkRelations(false);
	mapEditor.setShowTriangleEdges(true);
	mapEditor.setShowTriangleRelations(false);

	mapEditor.test();

	while (window.isOpen()) {
		deltaTime = gameClock.restart(); //get delta time
		frameRatePerSecond = std::roundf(1.f / deltaTime.asSeconds());

		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				switch (event.key.code)
				{
				case sf::Keyboard::W:
					keyDown_W = true;
					break;
				case sf::Keyboard::A:
					keyDown_A = true;
					break;
				case sf::Keyboard::S:
					keyDown_S = true;
					break;
				case sf::Keyboard::D:
					keyDown_D = true;
					break;
				case sf::Keyboard::Q:
					keyDown_Q = true;
					break;
				case sf::Keyboard::E:
					keyDown_E = true;
					break;
				default:
					break;
				}
				break;
			case sf::Event::KeyReleased:
				switch (event.key.code)
				{
				case sf::Keyboard::W:
					keyDown_W = false;
					break;
				case sf::Keyboard::A:
					keyDown_A = false;
					break;
				case sf::Keyboard::S:
					keyDown_S = false;
					break;
				case sf::Keyboard::D:
					keyDown_D = false;
					break;
				case sf::Keyboard::Q:
					keyDown_Q = false;
					break;
				case sf::Keyboard::E:
					keyDown_E = false;
					break;
				default:
					break;
				}
				break;
			case sf::Event::MouseButtonPressed:
				switch (event.mouseButton.button)
				{
				case sf::Mouse::Left:
					mouseDown_left = true;
					break;
				case sf::Mouse::Right:
					mouseDown_right = true;
					break;
				default:
					break;
				}
				break;
			case sf::Event::MouseButtonReleased:
				switch (event.mouseButton.button)
				{
				case sf::Mouse::Left:
					mouseDown_left = false;
					break;
				case sf::Mouse::Right:
					mouseDown_right = false;
					break;
				default:
					break;
				}
				break;
			case sf::Event::MouseMoved:
				mousePos_x = event.mouseMove.x;
				mousePos_y = event.mouseMove.y;

				break;
			default:
				break;
			}
		}

		mapEditor.updateMousePosition(mapEditor.screenPosToGamePos(window, mapEditor.getEditorView(), mousePos_x, mousePos_y));

		//update view pos
		if (keyDown_W) {
			mapEditor.getEditorView().move(0, -(deltaTime.asSeconds() * moveSpeed));
		}
		if (keyDown_S) {
			mapEditor.getEditorView().move(0, (deltaTime.asSeconds() * moveSpeed));
		}
		if (keyDown_A) {
			mapEditor.getEditorView().move(-(deltaTime.asSeconds() * moveSpeed), 0);
		}
		if (keyDown_D) {
			mapEditor.getEditorView().move((deltaTime.asSeconds() * moveSpeed), 0);
		}
		if (keyDown_Q) {
			float scalar = 1 + deltaTime.asSeconds() * zoomSpeed;
			mapEditor.getEditorView().zoom(1 + deltaTime.asSeconds() * zoomSpeed);
			moveSpeed = moveSpeed * scalar;
		}
		if (keyDown_E) {
			float scalar = 1 - deltaTime.asSeconds() * zoomSpeed;
			mapEditor.getEditorView().zoom(1 - deltaTime.asSeconds() * zoomSpeed);
			moveSpeed = moveSpeed * scalar;
		}



		if (mouseDown_left && !mouseBounce_left) {
			mouseBounce_left = true;
			mapEditor.mouseLeftClick();
		}

		if (!mouseDown_left && mouseBounce_left) {
			mouseBounce_left = false;
		}


		window.clear();

		window.setView(mapEditor.getEditorView());
		mapEditor.updateVertexArrays();
		window.draw(mapEditor);

		window.setView(window.getDefaultView());

		window.display();

	}



	return EXIT_SUCCESS;
}

#elif 0

int main() {

	//(507, 550)->(479, 603)->(497, 621)->(499, 608)->(507, 550)->(544, 547)->(744, 747)->(544, 747)->(625, 834)->(618, 851)->(644, 887)->(646, 889)->(446, 889)->(446, 851)->(418, 851)->(418, 723)->(561, 723)->(361, 523)->(361, 650)->(355, 650)->(359, 548)->(359, 348)
	
	
	return EXIT_SUCCESS;
}




#elif 1

int main() {

	window.create(sf::VideoMode(1000, 600), title, sf::Style::Default);
	if (!arial.loadFromFile("fonts/arial.ttf")) {
		std::cout << "Font load failed\n";
		return EXIT_FAILURE;
	}

	std::vector<wykobi::point2d<float>> graph_points = {
		wykobi::make_point<float>(10, 10),
		wykobi::make_point<float>(400, 10),
		wykobi::make_point<float>(400, 100),
		wykobi::make_point<float>(10, 400),
	};
	std::vector<std::pair<int, int>> graph_edges = {
		std::pair<int, int>(0, 1),
		std::pair<int, int>(1, 2),
		std::pair<int, int>(2, 3),
		std::pair<int, int>(3, 0),
		std::pair<int, int>(0, 2)
	};
	Graph::EdgeGraph<wykobi::point2d<float>> graph = Graph::makeEdgeGraphFromNodes<wykobi::point2d<float>>(graph_points, graph_edges);

	//graph.deleteNode(3);
	
	//wykobi::polygon<float, 2> poly = wykobi::make_polygon<float>({
	//	wykobi::make_point<float>(10, 10),
	//	wykobi::make_point<float>(400, 10),
	//	wykobi::make_point<float>(400, 100),
	//	wykobi::make_point<float>(300, 100),
	//	wykobi::make_point<float>(300, 200),
	//	wykobi::make_point<float>(400, 100),
	//	wykobi::make_point<float>(400, 400),
	//	wykobi::make_point<float>(300, 400),
	//	wykobi::make_point<float>(300, 350),
	//	wykobi::make_point<float>(250, 350),
	//	wykobi::make_point<float>(300, 400),
	//	wykobi::make_point<float>(10, 400),
	//});
	//wykobi::polygon<float, 2> poly = wykobi::make_polygon<float>({ wykobi::make_point<float>(754, 251),wykobi::make_point<float>(809, 263),wykobi::make_point<float>(830, 272),wykobi::make_point<float>(741, 272),wykobi::make_point<float>(810, 341),wykobi::make_point<float>(610, 341),wykobi::make_point<float>(610, 323),wykobi::make_point<float>(610, 311),wykobi::make_point<float>(745, 311),wykobi::make_point<float>(545, 111),wykobi::make_point<float>(545, 311),wykobi::make_point<float>(598, 311),wykobi::make_point<float>(610, 323),wykobi::make_point<float>(581, 316),wykobi::make_point<float>(389, 389),wykobi::make_point<float>(441, 292),wykobi::make_point<float>(537, 303),wykobi::make_point<float>(537, 103),wykobi::make_point<float>(554, 115),wykobi::make_point<float>(554, 51) });
	//wykobi::polygon<float, 2> poly;
	//{
	//	wykobi::polygon<float, 2> s1 = wykobi::make_polygon<float>(wykobi::make_circle<float>(100, 100, 50), 10);
	//	wykobi::polygon<float, 2> s2 = wykobi::make_polygon<float>(wykobi::make_circle<float>(175, 175, 75), 5);
	//	poly = Math::Clipper::mergePolygons(s1, s2)[0];
	//}
	//auto vec = Math::Clipper::findFirstLegalPolygonCut(poly, 7);
	//if (vec.empty()) {
	//	std::cout << "NO CUT FOUND\n";
	//}
	//wykobi::segment<float, 2> seg = wykobi::make_segment<float>(poly[vec[0]], poly[vec[1]]);
	//std::cout << "TEST: " << Math::isSegmentFromPolygonInsidePolygon(poly, 8, 9) << "\n";

	std::vector<wykobi::polygon<float, 2>> polygons;
	std::vector<wykobi::segment<float, 2>> segments;
	//polygons.push_back(poly);
	//segments.push_back(seg);
	//polygons = Math::Clipper::removeSubPolygon(poly);
	segments = Graph::getWykobiSegmentsFromEdgeGraph<float>(graph);
	//Math::Graph::getWykobiPolygonsFromEdgeGraph<float>(graph);
	//polygons =Graph::getWykobiPolygonsFromEdgeGraph(graph);

	sf::View fuck_view = window.getDefaultView();
	//fuck_view.zoom(-1.f);

	std::cout << "Polygons: " << polygons.size() << "\n";
	for (int i = 0; i < polygons.size(); i++) {
		std::cout << i << ": " << Math::Debug::toString(polygons[i]) << "\n";
	}

	std::cout << "Segments:\n";
	for (auto seg : segments) {
		std::cout << Math::Debug::toString(seg) << "\n";
	}

	wykobi::point2d<float> middle_of_screen = wykobi::make_point<float>(fuck_view.getCenter().x, fuck_view.getCenter().y);
	float test_angle = 90.f;
	wykobi::point2d<float> mouse_point;
	float state = 0.f;
	float old_state = 0.f;

	sf::VertexArray lineVertexArray = sf::VertexArray(sf::Lines);
	std::vector<sf::Text> numbers;

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			//case sf::Event::MouseMoved:
			//	mouse_point = wykobi::make_point<float>(event.mouseMove.x, event.mouseMove.y);
			//	state = Math::getRelativeAngle<float>(test_angle, Math::angleTowardsPoint(middle_of_screen, mouse_point));
			//	if (state != old_state) {
			//		old_state = state;
			//		std::cout << state << "\n";
			//	}
			//	break;
			default:
				break;
			}
		}

		int count = 0;

		lineVertexArray.clear();
		numbers.clear();

		for (auto poly : polygons) {
			for (int i = 0; i < poly.size(); i++) {
				wykobi::segment<float, 2> seg = wykobi::edge<float>(poly, i);
				sf::Vertex vertex1 = Math::wykobiPointToSfVertex(seg[0]);
				sf::Vertex vertex2 = Math::wykobiPointToSfVertex(seg[1]);
				lineVertexArray.append(vertex1);
				lineVertexArray.append(vertex2);
			}
			for (int i = 0; i < poly.size(); i++) {
				sf::Text t;
				std::ostringstream s;
				t.setFont(arial);
				t.setCharacterSize(20);
				s << i;
				t.setPosition(Math::wykobiPointToSfVector(poly[i]));
				t.setString(s.str());
				numbers.push_back(t);
			}
		}
		for (auto seg : segments) {
			for (int i = 0; i < seg.size(); i++) {
				sf::Vertex vertex = Math::wykobiPointToSfVertex(seg[i]);
				vertex.color = sf::Color::Green;
				lineVertexArray.append(vertex);
			}
		}
	
		window.clear();
		window.setView(fuck_view);

		window.draw(lineVertexArray);
		for (sf::Text & t : numbers) {
			window.draw(t);
		}

		window.display();

	}




	return EXIT_SUCCESS;
}

#endif

//polygons.push_back(poly);
//polygons.push_back(hull);
//for (auto p : Math::triangulatePolygon(poly)) {
//polygons.push_back(wykobi::make_polygon<float>(p));
//}
//std::vector<wykobi::polygon<float, 2>> polygons = Math::Clipper::removePolygonHull(poly, hull);


/*
wykobi::point2d<float> p1 = wykobi::make_point<float>(100, 100);
wykobi::point2d<float> p2 = wykobi::make_point<float>(200, 100);
wykobi::point2d<float> p3 = wykobi::make_point<float>(200, 200);

wykobi::point2d<float> pshit = wykobi::make_point<float>(100, 101);
wykobi::point2d<float> pcuck = wykobi::make_point<float>(200, 201);

wykobi::point2d<float> p4 = wykobi::make_point<float>(100, 200);

wykobi::triangle<float, 2> t1 = wykobi::make_triangle<float>(p1, p2, p3);
wykobi::triangle<float, 2> t2 = wykobi::make_triangle<float>(pshit, p4, pcuck);

wykobi::segment<float, 2> seg1 = wykobi::edge<float>(t1, 0);
wykobi::segment<float, 2> seg2 = wykobi::edge<float>(t1, 0);

seg2 = wykobi::translate<float>(wykobi::make_vector<float>(100, 0), seg2);


if (wykobi::parallel<float>(seg1, seg2) && wykobi::intersect<float>(seg1, seg2)) {
std::cout << "true\n";
}
else {
std::cout << "false\n";
}


//std::cout << wykobi::intersect<float>(seg1, seg2) << "\n";
*/

/**
wykobi::polygon<float, 2> subject;
subject.push_back(wykobi::make_point<float>(100, 100));
subject.push_back(wykobi::make_point<float>(400, 100));
subject.push_back(wykobi::make_point<float>(400, 200));
subject.push_back(wykobi::make_point<float>(100, 400));
*/

/*
wykobi::polygon<float, 2> clip;
clip.push_back(wykobi::make_point<float>(125, 100));
clip.push_back(wykobi::make_point<float>(150, 100));
clip.push_back(wykobi::make_point<float>(125, 150));
*/

//std::vector<wykobi::polygon<float, 2>> polygons = Math::Clipper::clipPolygonDifference(subject, clip);

/*

//subject
wykobi::triangle<float, 2> triangle1 = wykobi::make_triangle<float>(100, 100, 300, 100, 300, 300);

wykobi::triangle<float, 2> triangle2 = wykobi::make_triangle<float>(100, 100, 300, 300, 100, 300);

wykobi::triangle<float, 2> triangle3 = wykobi::make_triangle<float>(300, 100, 500, 100, 300, 300);

//wykobi::triangle<float, 2> triangle4 = wykobi::make_triangle<float>(200, 400, 250, 400, 500, 600);

wykobi::triangle<float, 2> triangle5 = wykobi::make_triangle<float>(100, 300, 300, 300, 300, 500);

wykobi::triangle<float, 2> triangle6 = wykobi::make_triangle<float>(500, 100, 300, 300, 300, 500);

wykobi::triangle<float, 2> triangle7 = wykobi::make_triangle<float>(200, 400, 250, 400, 500, 600);

wykobi::triangle<float, 2> triangle8 = wykobi::make_triangle<float>(200, 400, 250, 400, 500, 600);


std::vector<wykobi::point2d<float>> pp;
pp.push_back(wykobi::make_point<float>(100, 100));
pp.push_back(wykobi::make_point<float>(400, 100));
pp.push_back(wykobi::make_point<float>(700, 100));
pp.push_back(wykobi::make_point<float>(400, 400));
pp.push_back(wykobi::make_point<float>(100, 400));
pp.push_back(wykobi::make_point<float>(150, 300));

wykobi::polygon<float, 2> poly = wykobi::make_polygon(pp);

wykobi::polygon<float, 2> hull = wykobi::make_polygon<float>(wykobi::make_triangle<float>(110, 110, 200, 110, 200, 200));

//std::cout << Math::isSegmentFromPolygonSelfIntersecting(poly, 0, wykobi::make_point<float>(110, 150)) << "\n";

//wykobi::polygon<float, 2> poly1 = wykobi::make_polygon<float>(pp);

std::vector<wykobi::triangle<float, 2>> triangles;

triangles.push_back(triangle1);
triangles.push_back(triangle2);
triangles.push_back(triangle3);
triangles.push_back(triangle5);
triangles.push_back(triangle6);

*/

/*
for (auto & tri : triangles) {
std::vector<wykobi::point2d<float>> vec;
for (int i = 0; i < tri.size(); i++) {
vec.push_back(tri[i]);
}
vec = Math::rotateWykobiPointVector(vec, wykobi::make_point<float>(500, 300), 45.f);
for (int i = 0; i < tri.size(); i++) {
tri[i] = vec[i];
}
}
*/

//wykobi::polygon<float, 2> shit = wykobi::make_polygon<float>({ 
//	wykobi::make_point<float>(100, 100),
//	wykobi::make_point<float>(200, 100),
//	wykobi::make_point<float>(150, 150),
//	wykobi::make_point<float>(200, 200),
//	wykobi::make_point<float>(100, 200)
//});


//polygons.pop_back();

//polygons.push_back(poly);

//polygons.push_back(Math::insertPointsOnEdgesOfPolygon(shit, { wykobi::make_point<float>(150, 100) , wykobi::make_point<float>(140, 200)}));

//for (auto poly : Math::Clipper::mergeTriangles(triangles)) {
//for (auto poly : Math::Clipper::mergeTrianglesSharedVertices(triangles)) {
//for (auto poly : Math::Clipper::mergeTriangles(triangles)) {
/*
for (auto poly : Math::Clipper::mergeTriangles(triangles)) {
poly = Math::insertPointsOnEdgesOfPolygon(poly, Math::makeWykobiPointVectorFromShape<wykobi::triangle<float, 2>>(triangles));
for (int i = 0; i < poly.size(); i++) {
std::cout << poly[i].x << " " << poly[i].y << "->";
}
std::cout << "\n\n";
for (auto tri : Math::triangulatePolygon(poly)) {
polygons.push_back(wykobi::make_polygon<float>(tri));
}
}
*/
//}
//wykobi::polygon<float, 2> poly1 = Math::Clipper::mergeTriangles(triangles)[0];
//wykobi::polygon<float, 2> poly1 = Math::Clipper::mergeTrianglesSharedVertices(triangles)[0];
//triangles = Math::triangulatePolygon(poly1);
//std::vector<wykobi::polygon<float, 2>> polygons = Math::Clipper::mergeTriangles(triangles);
//for (auto tri : triangles) {
//	polygons.push_back(wykobi::make_polygon<float>(tri));
//}


//std::cout << polygons.size() << "\n";

//Polygon: (369, 367)->(373, 310)->(573, 310)->(1024, 0)->(1024, 1024)->(0, 1024)
//Hull : (719.332, 604.126)->(919.332, 804.126)->(719.332, 804.126)
//Seg1: (369, 367)->(719.332, 604.126)
//Seg2 : (0, 0)->(0, 0)
