#include "../Include/GraphNodeShape.hpp"

GraphNodeShape::GraphNodeShape(int nodeId, const sf::Vector2f& position, const sf::Font& font)
	: nodeId(nodeId)
{
	shape.setRadius(radius);
	shape.setOrigin({ radius, radius });
	shape.setPosition(position);
	shape.setFillColor({ 200, 130, 120 });

	text.setFont(font);
	text.setCharacterSize(24);
	text.setString(std::to_string(nodeId));
	const auto textRect = text.getLocalBounds();
	text.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
	text.setPosition(position);
}

void GraphNodeShape::draw(sf::RenderWindow& window) const
{
	window.draw(shape);
	window.draw(text);
}
