#include "../Include/GraphNodeShape.hpp"
#include "../Include/ResourceManager.hpp"
#include "../Include/Utilities.hpp"

GraphNodeShape::GraphNodeShape(int nodeId, const sf::Vector2f& position)
	: nodeId(nodeId)
{	
	// Initialize shape
	shape.setRadius(Constants::nodeRadius);
	shape.setOrigin({ Constants::nodeRadius, Constants::nodeRadius });
	shape.setPosition(position);
	shape.setFillColor({ 255, 255, 255 });
	shape.setOutlineThickness(Constants::nodeOutlineThickness);

	// Initialize text
	text.setFont(ResourceManager::instance().getSFMLFont());
	text.setCharacterSize(24);
	text.setString(std::to_string(nodeId));
	const auto textRect = text.getLocalBounds();
	text.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
	text.setPosition(position);
	text.setFillColor({ 50, 50, 50 });
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

void GraphNodeShape::makeColored(const sf::Color& color)
{
	shape.setFillColor(color);
	text.setFillColor(sf::Color::White);
}

void GraphNodeShape::resetColor()
{
	shape.setFillColor({ 255, 255, 255 });
	text.setFillColor({ 50,50,50 });
}
