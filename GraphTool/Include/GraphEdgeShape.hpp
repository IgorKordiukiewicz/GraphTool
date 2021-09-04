#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include "Graph.hpp"

class GraphEdgeShape
{
public:
	GraphEdgeShape(const sf::Vector2f& startPosition, const sf::Vector2f& endPosition, int startNodeId, int endNodeId, GraphType graphType);

	void draw(sf::RenderWindow& window);

	void setEndPosition(const sf::Vector2f& newEndPosition);
	void makeDirected();
	void makeUndirected();

	const sf::Vector2f& getStartPosition() const { return startPosition; }
	const sf::Vector2f& getEndPosition() const { return endPosition; }
	int getStartNodeId() const { return startNodeId; }
	int getEndNodeId() const { return endNodeId; }

private:
	void updateVertices();

private:
	GraphType graphType;

	sf::Vector2f startPosition;
	sf::Vector2f endPosition;
	int startNodeId;
	int endNodeId;

	sf::VertexArray lineVertices;
	sf::VertexArray headVertices;
	float headHeight = 10.f;
	float headWidth = 11.5f;
};