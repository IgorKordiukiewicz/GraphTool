#include "..\Include\GraphEditor.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

GraphEditor::GraphEditor(Graph& graph, sf::RenderWindow& window)
	: graph(graph), window(window)
{
	if (!font.loadFromFile("Resources/AGENCYR.ttf")) {
		std::cout << "Couldn't load font from file!\n";
	}

	graph.onGraphTypeChanged.connect("GraphEditor", this, &GraphEditor::onGraphTypeChanged);
}

void GraphEditor::processEvents(sf::Event& event)
{
	auto isMouseInsideGraphEditor = [this]() {
		const sf::FloatRect graphViewRect{
			static_cast<float>(window.getSize().x) * 0.25f, 0.f, 
			static_cast<float>(window.getSize().x) * 0.75f, static_cast<float>(window.getSize().y) };
		return graphViewRect.contains(sf::Vector2f(sf::Mouse::getPosition(window)));
	};
	
	if (event.type == sf::Event::MouseButtonPressed 
		&& event.mouseButton.button == sf::Mouse::Left
		&& isMouseInsideGraphEditor()) {
		const auto mousePosition = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
		// Check if mouse hovers over any of the nodes shapes
		bool mouseOverNodeShape = false;
		for (const auto& shape : nodesShapes) {
			if (shape.getShape().getGlobalBounds().contains(mousePosition)) {
				mouseOverNodeShape = true;

				// If user was already holding an edge, attach it to this node
				if (heldEdge.has_value()) {
					// Prevent having duplicate edge shapes
					if (auto [nodeA, nodeB] = std::pair{ heldEdge->getStartNodeId(), shape.getNodeId() }; !graph.doesEdgeExist(nodeA, nodeB)) {
						graph.addEdge(nodeA, nodeB);
						edgesShapes.push_back(GraphEdgeShape{ heldEdge->getStartPosition(), shape.getShape().getPosition(), nodeA, nodeB, graph.getType() });
						heldEdge.reset();
					}				
				}
				// Create new edge shape
				else {
					heldEdge = GraphEdgeShape{shape.getShape().getPosition(), mousePosition, shape.getNodeId(), -1, graph.getType()};
				}
			}
		}

		if (!mouseOverNodeShape) {
			// Double click to add new node
			if (doubleClickClock.getElapsedTime().asSeconds() <= doubleClickDelay) {
				const int newNodeId = graph.createNode();
				nodesShapes.push_back(GraphNodeShape{ newNodeId, mousePosition, font });

				// If user was holding an edge, attach it to the created node
				if (heldEdge.has_value()) {
					graph.addEdge(heldEdge->getStartNodeId(), newNodeId);
					edgesShapes.push_back(GraphEdgeShape{heldEdge->getStartPosition(), mousePosition, heldEdge->getStartNodeId(), newNodeId, graph.getType() });
					heldEdge.reset();
				}
			}
			doubleClickClock.restart();
		}	
	}
	// Right mouse button to release stop holding an edge
	if (event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Right && heldEdge.has_value()) {
		heldEdge.reset();
	}
}

void GraphEditor::update(float deltaTime)
{
	if (heldEdge.has_value()) {
		heldEdge->setEndPosition(sf::Vector2f(sf::Mouse::getPosition(window)));
	}
	
	for (auto& shape : nodesShapes) {
		shape.update(deltaTime);
	}
}

void GraphEditor::draw(sf::RenderWindow& window)
{
	for (auto& edgeShape : edgesShapes) {
		edgeShape.draw(window);
	}

	if (heldEdge.has_value()) {
		heldEdge->draw(window);
	}
	
	for (auto& nodeShape : nodesShapes) {
		nodeShape.draw(window);
	}
}

void GraphEditor::onGraphTypeChanged()
{
	if (graph.isDirected()) {
		for (auto& edgeShape : edgesShapes) {
			edgeShape.makeDirected();
		}
	}
	else {
		for (auto& edgeShape : edgesShapes) {
			edgeShape.makeUndirected();
		}
	}
}
