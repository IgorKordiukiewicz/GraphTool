#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <vector>
#include <optional>
#include "Animations.hpp"
#include "GraphEdgeShape.hpp"
#include <SFML/Graphics/Sprite.hpp>

enum class Panel
{
	EditPanel,
	AlgorithmsPanel
};

class GraphEditor
{
public:
	GraphEditor(Graph& graph, sf::RenderWindow& window);

	void processEvents(sf::Event& event);
	void update(float deltaTime);
	void draw(sf::RenderWindow& window) const;

	void setCurrentPanel(Panel newCurrentPanel);

	void activateTraversalOrderAnimation(const GraphAlgorithms::TraversalOrder& traversalOrder);
	void activateTraversalOrderAnimation(const GraphAlgorithms::TraversalOrder& traversalOrder, const GraphAlgorithms::NodesColorsIdxs& nodesColorsIdxs);
	void deactivateTraversalOrderAnimation();
	void startLoopingTraversalOrderAnimation();
	void stopLoopingTraversalOrderAnimation();

	void createNewNode(const sf::Vector2f& mousePosition);
	// Returns pointers to the created directed edge shape and undirected edge shape
	std::pair<GraphEdgeShape*, GraphEdgeShape*> createNewEdge(int a, int b);

	const std::vector<GraphNodeShape>& getNodesShapes() const { return nodesShapes; }

private:
	bool isMouseInsideGraphEditor() const;
	void stopEditingEdgeWeight();
	void editEdgeWeight(sf::Event& event);
	void attachHeldEdge(const GraphNodeShape& nodeShape);
	void createNewHeldEdge(const GraphNodeShape& nodeShape, const sf::Vector2f& mousePosition);
	void startEditingEdgeWeightIfRequired(const sf::Vector2f& mousePosition);
	// Returns true if a node was deleted and false otherwise
	bool deleteNodeIfRequired(const sf::Vector2f& mousePosition);
	void deleteEdgeIfRequired(const sf::Vector2f& mousePosition);
	void startHoldingNode(GraphNodeShape& nodeShape);
	void updateHeldEdge();
	void updateHeldNodePtrs();
	void onGraphDirectedEdgesDeleted(std::vector<std::pair<int, int>> deletedEdges);
	void onGraphUndirectedEdgesDeleted(std::vector<std::pair<int, int>> deletedEdges);
	void onGraphWeightedValueChanged();

private:
	std::vector<GraphNodeShape> nodesShapes;
	std::vector<GraphEdgeShape> directedEdgesShapes;
	std::vector<GraphEdgeShape> undirectedEdgesShapes;

	sf::Clock doubleClickClock;
	const float doubleClickDelay{ 0.2f };

	Panel currentPanel{ Panel::EditPanel };

	std::optional<GraphEdgeShape> heldEdge{ std::nullopt };
	struct HeldNodePtrs
	{
		GraphNodeShape* heldNodeShape{ nullptr };
		// Pointer to the edge shape, and bool indicating whether to change start or end position (true - startPosition, false - endPosition)
		std::vector<std::pair<GraphEdgeShape*, bool>> connectedEdgesShapes;
	};
	std::optional<HeldNodePtrs> heldNodePtrs{ std::nullopt };
	GraphEdgeShape* editedEdge{ nullptr };
	bool changedEditedEdgeWeight{ false };

	friend Animations::TraversalOrderAnimation;
	Animations::TraversalOrderAnimation traversalOrderAnimation;

	sf::Sprite background;

	Graph& graph;
	sf::RenderWindow& window;
};
