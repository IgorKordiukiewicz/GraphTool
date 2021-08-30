#include "..\Include\GraphEditor.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

void GraphEditor::processEvents(sf::Event& event, sf::RenderWindow& window)
{
	auto isMouseInsideGraphEditor = [&window]() {
		const sf::FloatRect graphViewRect{
			static_cast<float>(window.getSize().x) * 0.25f, 0.f, 
			static_cast<float>(window.getSize().x) * 0.75f, static_cast<float>(window.getSize().y) };
		return graphViewRect.contains(sf::Vector2f(sf::Mouse::getPosition(window)));
	};
	
	if (event.type == sf::Event::MouseButtonPressed 
		&& event.mouseButton.button == sf::Mouse::Left
		&& isMouseInsideGraphEditor()) {
		// Double click to add new node
		if (doubleClickClock.getElapsedTime().asSeconds() <= doubleClickDelay) {
			sf::CircleShape shape;
			shape.setRadius(16.f);
			shape.setPosition(window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y }));
			nodesShapes.push_back(std::move(shape));
		}
		doubleClickClock.restart();
	}
}

void GraphEditor::update(float deltaTime)
{
}

void GraphEditor::draw(sf::RenderWindow& window)
{
	for (auto& shape : nodesShapes) {
		window.draw(shape);
	}
}
