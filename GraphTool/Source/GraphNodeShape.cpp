#include "../Include/GraphNodeShape.hpp"
#include "../Include/ResourceManager.hpp"

GraphNodeShape::GraphNodeShape(int nodeId, const sf::Vector2f& position)
	: nodeId(nodeId)
{	
	// Initialize shape
	shape.setRadius(radius);
	shape.setOrigin({ radius, radius });
	shape.setPosition(position);
	shape.setFillColor(color);
	shape.setOutlineColor(sf::Color::White);
	shape.setOutlineThickness(2.f);

	// Initialize text
	text.setFont(ResourceManager::instance().getSFMLFont());
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

void GraphNodeShape::setPosition(const sf::Vector2f& newPosition)
{
	shape.setPosition(newPosition);
	text.setPosition(newPosition);
}

void GraphNodeShape::makeOutlineColored()
{
	shape.setOutlineColor(color);
}

void GraphNodeShape::resetOutlineColor()
{
	shape.setOutlineColor(sf::Color::White);
}
