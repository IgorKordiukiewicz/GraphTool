#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <vector>
#include "GraphAlgorithms.hpp"
#include "GraphNodeShape.hpp"
#include "GraphEdgeShape.hpp"
#include <optional>

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
	void draw(sf::RenderWindow& window);

	void setCurrentPanel(Panel newCurrentPanel);

	void onDirectedEdgesDeleted(std::vector<std::pair<int, int>> deletedEdges);
	void onUndirectedEdgesDeleted(std::vector<std::pair<int, int>> deletedEdges);

	void onGraphWeightedValueChanged();

	void activateTraversalOrderAnimation(const GraphAlgorithms::TraversalOrder& traversalOrder);
	void activateTraversalOrderAnimation(const GraphAlgorithms::TraversalOrder& traversalOrder, const GraphAlgorithms::NodesColorsIdxs& nodesColorsIdxs);
	void deactivateTraversalOrderAnimation();
	void startLoopingTraversalOrderAnimation();
	void stopLoopingTraversalOrderAnimation();

private:
	bool isMouseInsideGraphEditor() const;
	void stopEditingEdgeWeight();
	void editEdgeWeight(sf::Event& event);
	void attachHeldEdge(const GraphNodeShape& nodeShape);
	void createNewEdge(const GraphNodeShape& nodeShape, const sf::Vector2f& mousePosition);
	void createNewNode(const sf::Vector2f& mousePosition);
	void startEditingEdgeWeightIfRequired(const sf::Vector2f& mousePosition);
	// Returns true if a node was deleted and false otherwise
	bool deleteNodeIfRequired(const sf::Vector2f& mousePosition);
	void deleteEdgeIfRequired(const sf::Vector2f& mousePosition);
	void startHoldingNode(GraphNodeShape& nodeShape);
	void updateHeldEdge();
	void updateHeldNodePtrs();
	
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

	class TraversalOrderAnimation
	{	
		bool active{ false };
		bool running{ false };
		GraphAlgorithms::TraversalOrder traversalOrder;
		std::map<int, sf::Color> nodesColors;
		std::vector<std::pair<GraphNodeShape*, sf::Color>> nodesShapesInOrder;
		// bool - true if direction of the animation should be reversed
		std::vector<std::pair<GraphEdgeShape*, bool>> edgesShapesInOrder;

		int index{ 0 };
		sf::Clock clock;
		bool loop{ false };
		float loopDelay{  };

	public:
		GraphEditor* parent{ nullptr };

		void activate(const GraphAlgorithms::TraversalOrder& traversalOrder);
		void activate(const GraphAlgorithms::TraversalOrder& traversalOrder, const GraphAlgorithms::NodesColorsIdxs& nodesColorsIdxs);
		void run();
		void deactivate();
		void update();
		void startLooping();
		void stopLooping();
		bool isActive() const { return active; }
	} traversalOrderAnimation;

	sf::Font font;

	Graph& graph;
	sf::RenderWindow& window;
};
