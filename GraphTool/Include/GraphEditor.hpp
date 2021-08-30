#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>
#include <SFML/Graphics/CircleShape.hpp>

class GraphEditor
{
public:
	GraphEditor() {}

	void processEvents(sf::Event& event, sf::RenderWindow& window);
	void update(float deltaTime);
	void draw(sf::RenderWindow& window);

private:
	std::vector<sf::CircleShape> nodesShapes;

	sf::Clock doubleClickClock;
	const float doubleClickDelay{ 0.2f };
};
