#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <vector>
#include "Graph.hpp"
#include "GraphNodeShape.hpp"
#include "GraphEdgeShape.hpp"
#include <optional>

class GraphEditor
{
public:
	GraphEditor(Graph& graph, sf::RenderWindow& window);

	void processEvents(sf::Event& event);
	void update(float deltaTime);
	void draw(sf::RenderWindow& window);

	void onDirectedEdgesDeleted(std::vector<std::pair<int, int>> deletedEdges);
	void onUndirectedEdgesDeleted(std::vector<std::pair<int, int>> deletedEdges);

private:
	std::vector<GraphNodeShape> nodesShapes;
	std::vector<GraphEdgeShape> directedEdgesShapes;
	std::vector<GraphEdgeShape> undirectedEdgesShapes;

	sf::Clock doubleClickClock;
	const float doubleClickDelay{ 0.2f };

	std::optional<GraphEdgeShape> heldEdge{ std::nullopt };

	sf::Font font;

	Graph& graph;
	sf::RenderWindow& window;
};
