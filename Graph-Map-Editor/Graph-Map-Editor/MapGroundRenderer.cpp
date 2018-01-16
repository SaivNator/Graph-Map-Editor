//Author: Sivert Andresen Cubedp

#include "MapGroundRenderer.hpp"

MapGroundRenderer::MapGroundRenderer(std::shared_ptr<sf::Texture> tex_ptr, std::vector<wykobi::rectangle<float>> & tex_offset) :
	m_texture(tex_ptr),
	m_tex_offset(tex_offset)
{
}

void MapGroundRenderer::setShowTriangleTexture(bool v) {
	m_show_triangle_texture = v;
}

void MapGroundRenderer::setShowTriangleLines(bool v) {
	m_show_triangle_lines = v;
}

void MapGroundRenderer::setShowTriangleRelations(bool v) {
	m_show_triangle_relations = v;
}

void MapGroundRenderer::clear() {
	m_vertex_array_tri.clear();
	m_vertex_array_line.clear();
}

void MapGroundRenderer::appendTriangle(MapTriangle & triangle) {
	if (m_show_triangle_texture) {
		for (std::size_t i = 0; i < triangle.size(); ++i) {
			sf::Vertex v;
			v.position = { triangle[i].x, triangle[i].y };
			wykobi::point2d<float> tex_pos = triangle[i] - triangle.getChunk().getOffset();
			tex_pos = tex_pos + m_tex_offset[triangle.getType()][0];
			v.texCoords = { tex_pos.x, tex_pos.y };
			m_vertex_array_tri.append(v);
		}
	}
	if (m_show_triangle_lines) {
		for (std::size_t i = 0; i < triangle.size(); ++i) {
			auto a = triangle[i];
			auto b = (i < triangle.size() - 1) ? triangle[i + 1] : triangle[0];
			sf::Vertex v0;
			sf::Vertex v1;
			v0.position = { a.x, a.y };
			v1.position = { b.x, b.y };
			m_vertex_array_line.append(v0);
			m_vertex_array_line.append(v1);
		}
	}
	if (m_show_triangle_relations) {
		wykobi::point2d<float> origin = triangle.getCentroid();
		sf::Vertex v0;
		v0.position = { origin.x, origin.y };
		for (MapTriangle* rel : triangle.getRelations()) {
			wykobi::point2d<float> c = rel->getCentroid();
			sf::Vertex v1;
			v1.position = { c.x, c.y };
			m_vertex_array_line.append(v0);
			m_vertex_array_line.append(v1);
		}
	}
}

void MapGroundRenderer::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	if (m_texture) {
		states.texture = m_texture.get();
	}
	target.draw(m_vertex_array_tri, states);
	target.draw(m_vertex_array_line);
}

std::shared_ptr<sf::Texture> MapGroundRenderer::getTexture() {
	return m_texture;
}

//end