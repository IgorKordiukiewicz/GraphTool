#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Text.hpp>
#include "Graph.hpp"
#include <array>

class GraphEdgeShape
{
public:
	GraphEdgeShape(const sf::Vector2f& startPosition, const sf::Vector2f& endPosition, int startNodeId, int endNodeId, Directed graphType, Weighted weighted);

	void draw(sf::RenderWindow& window) const;

	void setStartPosition(const sf::Vector2f& newStartPosition);
	void setEndPosition(const sf::Vector2f& newEndPosition);

	void makeDirected();
	void makeUndirected();
	void makeWeighted();
	void makeUnweighted();
	void makeOrthogonalOffsetEnabled();
	void makeOrthogonalOffsetDisabled();

	bool contains(const sf::Vector2f& point) const;

	const sf::Vector2f& getStartPosition() const { return startPosition; }
	const sf::Vector2f& getEndPosition() const { return endPosition; }
	int getStartNodeId() const { return startNodeId; }
	int getEndNodeId() const { return endNodeId; }

private:
	void updateVertices();

private:
	Directed directed;
	Weighted weighted;

	sf::Vector2f startPosition;
	sf::Vector2f endPosition;
	int startNodeId;
	int endNodeId;

	sf::VertexArray lineVertices;
	sf::VertexArray headVertices;
	float headHeight = 10.f;
	float headWidth = 11.5f;
	float orthogonalOffset = 5.f;
	bool isOrthogonalOffsetEnabled{ false };
	struct Rectangle 
	{
		sf::Vector2f a, b, c, d;
		float width{ 0.f };
		float height{ 4.f };
	} lineBounds;

	sf::Text weightText;
	float weightTextOrthOffset = 10.f;
};