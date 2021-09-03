#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Color.hpp>
#include "GraphEditor.hpp"
#include "EditPanel.hpp"
#include "AlgorithmsPanel.hpp"
#include "Graph.hpp"

class Application
{
public:
	Application();

	void run();

private:
	void processEvents();

private:
	sf::RenderWindow window;
	const sf::Color backgroundColor{ 30, 30, 30 };

	GraphEditor graphEditor;
	EditPanel editPanel;
	AlgorithmsPanel algorithmsPanel;
	Graph graph;
};