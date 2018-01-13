//Author: Sivert Andresen Cubedp

#include "MapGroundTypeRenderer.hpp"

MapGroundTypeRenderer::MapGroundTypeRenderer()
{
}

void MapGroundTypeRenderer::clear() {
	m_vertex_array.clear();
}

void MapGroundTypeRenderer::appendTriangle(MapTriangle & triangle) {
	for (std::size_t i = 0; i < triangle.size(); ++i) {
		sf::Vertex v;
		v.position = { triangle[i].x, triangle[i].y };
		wykobi::vector2d<float> tex_pos = triangle[i] - triangle.getChunk().getOffset();
		v.texCoords = { tex_pos.x, tex_pos.y };
		m_vertex_array.append(v);
	}
}

void MapGroundTypeRenderer::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	if (m_texture) {
		states.texture = m_texture.get();
	}
	target.draw(m_vertex_array, states);
}

void MapGroundTypeRenderer::setColor(sf::Color color) {
	m_color = color;
}

void MapGroundTypeRenderer::setTexture(std::shared_ptr<sf::Texture> texture) {
	m_texture = texture;
}

sf::Color MapGroundTypeRenderer::getColor() {
	return m_color;
}

std::shared_ptr<sf::Texture> MapGroundTypeRenderer::getTexture() {
	return m_texture;
}

//end