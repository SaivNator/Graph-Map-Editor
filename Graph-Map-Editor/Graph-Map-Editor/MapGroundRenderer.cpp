//Author: Sivert Andresen Cubedp

#include "MapGroundRenderer.hpp"

MapGroundRenderer::MapGroundRenderer(std::shared_ptr<sf::Texture> tex_ptr, std::vector<wykobi::rectangle<float>> & tex_offset) :
	m_texture(tex_ptr),
	m_tex_offset(tex_offset)
{
}

void MapGroundRenderer::clear() {
	m_vertex_array.clear();
}

void MapGroundRenderer::appendTriangle(MapTriangle & triangle) {
	for (std::size_t i = 0; i < triangle.size(); ++i) {
		sf::Vertex v;
		v.position = { triangle[i].x, triangle[i].y };
		wykobi::point2d<float> tex_pos = triangle[i] - triangle.getChunk().getOffset();
		tex_pos = tex_pos + m_tex_offset[triangle.getType()][0];
		v.texCoords = { tex_pos.x, tex_pos.y };
		m_vertex_array.append(v);
	}
}

void MapGroundRenderer::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	if (m_texture) {
		states.texture = m_texture.get();
	}
	target.draw(m_vertex_array, states);
}

std::shared_ptr<sf::Texture> MapGroundRenderer::getTexture() {
	return m_texture;
}

//end