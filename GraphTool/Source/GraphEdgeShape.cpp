#include "../Include/GraphEdgeShape.hpp"

GraphEdgeShape::GraphEdgeShape(const sf::Vector2f& startPosition, const sf::Vector2f& endPosition, int startNodeId, int endNodeId)
	: startPosition(startPosition), endPosition(endPosition), startNodeId(startNodeId), endNodeId(endNodeId)
{
	lineVertices.setPrimitiveType(sf::PrimitiveType::Lines);
	updateVertices();
}

void GraphEdgeShape::draw(sf::RenderWindow& window)
{
	window.draw(lineVertices);
}

void GraphEdgeShape::setEndPosition(const sf::Vector2f& newEndPosition)
{
	endPosition = newEndPosition;
	updateVertices();
}

void GraphEdgeShape::updateVertices()
{
	lineVertices.clear();
	lineVertices.append(startPosition);
	lineVertices.append(endPosition);
}
