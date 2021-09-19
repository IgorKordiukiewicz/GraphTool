#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Color.hpp>
#include "GraphEditor.hpp"
#include "EditPanel.hpp"
#include "AlgorithmsPanel.hpp"
#include "Graph.hpp"
#include <memory>

class Application
{
public:
	Application();

	void run();

private:
	void processEvents();
	void initializeImGuiStyle();

private:
	sf::RenderWindow window;
	const sf::Color backgroundColor{ 30, 30, 35 };

	Graph graph;
	Panel currentPanel{ Panel::EditPanel };
	std::unique_ptr<GraphEditor> graphEditor;
	std::unique_ptr<EditPanel> editPanel;
	std::unique_ptr<AlgorithmsPanel> algorithmsPanel;

	struct ImFont* font{ nullptr };
};