//Author: Sivert Andresen Cubedp

#include "MapGroundType.hpp"

MapGroundType::MapGroundType(std::shared_ptr<sf::Texture> texture) :
	m_texture(texture)
{
}

void MapGroundType::clear() {
	m_vertex_array.clear();
}

void MapGroundType::appendTriangle(MapTriangle & triangle) {
	for (std::size_t i = 0; i < triangle.size(); ++i) {
		sf::Vertex v;
		v.position = { triangle[i].getPos().x, triangle[i].getPos().y };
		wykobi::vector2d<float> tex_pos = triangle[i].getPos() - triangle.getChunk().getOffset();
		v.texCoords = { tex_pos.x, tex_pos.y };
		m_vertex_array.append(v);
	}
}

std::shared_ptr<sf::Texture> MapGroundType::getTexture() {
	return m_texture;
}

void MapGroundType::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	states.texture = m_texture.get();
	target.draw(m_vertex_array, states);
}

//end