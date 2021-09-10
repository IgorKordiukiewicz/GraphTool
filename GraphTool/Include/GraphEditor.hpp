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

	void onGraphWeightedValueChanged();

private:
	bool isMouseInsideGraphEditor() const;

private:
	std::vector<GraphNodeShape> nodesShapes;
	std::vector<GraphEdgeShape> directedEdgesShapes;
	std::vector<GraphEdgeShape> undirectedEdgesShapes;

	sf::Clock doubleClickClock;
	const float doubleClickDelay{ 0.2f };

	std::optional<GraphEdgeShape> heldEdge{ std::nullopt };
	struct HeldNodePtrs
	{
		GraphNodeShape* heldNodeShape{ nullptr };
		// Pointer to the edge shape, and bool indicating whether to change start or end position (true - startPosition, false - endPosition)
		std::vector<std::pair<GraphEdgeShape*, bool>> connectedEdgesShapes;
	};
	std::optional<HeldNodePtrs> heldNodePtrs{ std::nullopt };

	sf::Font font;

	Graph& graph;
	sf::RenderWindow& window;
};
