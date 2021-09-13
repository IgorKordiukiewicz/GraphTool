#pragma once

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include "Graph.hpp"

class GraphNodeShape
{
public:
	GraphNodeShape(int nodeId, const sf::Vector2f& position);

	void draw(sf::RenderWindow& window) const;

	void setPosition(const sf::Vector2f& newPosition);

	void makeOutlineColored();
	void resetOutlineColor();

	const sf::CircleShape& getShape() const { return shape; }
	int getNodeId() const { return nodeId; }

private:
	sf::CircleShape shape;
	sf::Color color{ 220, 60, 60 };
	float radius{ 16.f };
	sf::Text text;

	int nodeId;
};
