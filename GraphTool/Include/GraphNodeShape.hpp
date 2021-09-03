#pragma once

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include "Graph.hpp"

class GraphNodeShape
{
public:
	GraphNodeShape(int nodeId, const sf::Vector2f& position, const sf::Font& font);

	void processEvents(sf::Event& event, sf::RenderWindow& window);
	void update(float deltaTime);
	void draw(sf::RenderWindow& window);

	const sf::CircleShape& getShape() const { return shape; }
	int getNodeId() const { return nodeId; }

private:
	sf::CircleShape shape;
	const float radius{ 16.f };
	sf::Text text;

	const int nodeId;
};
