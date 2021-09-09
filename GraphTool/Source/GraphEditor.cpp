#include "..\Include\GraphEditor.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

GraphEditor::GraphEditor(Graph& graph, sf::RenderWindow& window)
	: graph(graph), window(window)
{
	if (!font.loadFromFile("Resources/AGENCYR.ttf")) {
		std::cout << "Couldn't load font from file!\n";
	}

	graph.onDirectedEdgesDeleted.connect("GraphEditor", this, &GraphEditor::onDirectedEdgesDeleted);
	graph.onUndirectedEdgesDeleted.connect("GraphEditor", this, &GraphEditor::onUndirectedEdgesDeleted);
}

void GraphEditor::processEvents(sf::Event& event)
{
	auto isMouseInsideGraphEditor = [this]() {
		const sf::FloatRect graphViewRect{
			static_cast<float>(window.getSize().x) * 0.25f, 0.f, // 0.25f, because the imgui window's length is 1/4 of the app window length, TODO: make it not hardcoded
			static_cast<float>(window.getSize().x) * 0.75f, static_cast<float>(window.getSize().y) };
		return graphViewRect.contains(sf::Vector2f(sf::Mouse::getPosition(window)));
	};
	const auto mousePosition = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
	
	if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && isMouseInsideGraphEditor()) {
		// Check if mouse hovers over any of the nodes shapes
		bool isMouseOverNodeShape = false;
		for (const auto& nodeShape : nodesShapes) {
			if (nodeShape.getShape().getGlobalBounds().contains(mousePosition)) {
				isMouseOverNodeShape = true;

				// If user was already holding an edge, attach it to this node
				if (heldEdge.has_value()) {
					// Prevent having duplicate edge shapes
					if (auto [nodeA, nodeB] = std::pair{ heldEdge->getStartNodeId(), nodeShape.getNodeId() }; !graph.doesEdgeExist(nodeA, nodeB)) {
						directedEdgesShapes.push_back(GraphEdgeShape{ heldEdge->getStartPosition(), nodeShape.getShape().getPosition(), nodeA, nodeB, GraphType::Directed });
						if (!graph.doesEdgeExist(nodeB, nodeA)) {
							undirectedEdgesShapes.push_back(GraphEdgeShape{ heldEdge->getStartPosition(), nodeShape.getShape().getPosition(), nodeA, nodeB, GraphType::Undirected });
						}
						graph.addEdge(nodeA, nodeB);
						heldEdge.reset();

						// If edges is directed, and there are edges A -> B and B -> A, 
						// set these edges shapes to use orthogonal offset to prevent them from overlapping each other
						if (graph.isDirected() && graph.doesEdgeExist(nodeB, nodeA)) {
							directedEdgesShapes.back().makeOrthogonalOffsetEnabled();
							for (auto& edgeShape : directedEdgesShapes) {
								if (edgeShape.getStartNodeId() == nodeB && edgeShape.getEndNodeId() == nodeA) {
									edgeShape.makeOrthogonalOffsetEnabled();
									break;
								}
							}
						}
					}				
				}
				// Create new edge shape
				else {
					heldEdge = GraphEdgeShape{ nodeShape.getShape().getPosition(), mousePosition, nodeShape.getNodeId(), -1, graph.getType()};
				}

				break;
			}
		}

		if (!isMouseOverNodeShape) {
			// Double click to add new node
			if (doubleClickClock.getElapsedTime().asSeconds() <= doubleClickDelay) {
				const int newNodeId = graph.createNode();
				nodesShapes.push_back(GraphNodeShape{ newNodeId, mousePosition, font });

				// If user was holding an edge, attach it to the created node
				if (heldEdge.has_value()) {
					graph.addEdge(heldEdge->getStartNodeId(), newNodeId);
					directedEdgesShapes.push_back(GraphEdgeShape{heldEdge->getStartPosition(), mousePosition, heldEdge->getStartNodeId(), newNodeId, GraphType::Directed });
					undirectedEdgesShapes.push_back(GraphEdgeShape{heldEdge->getStartPosition(), mousePosition, heldEdge->getStartNodeId(), newNodeId, GraphType::Undirected });
					heldEdge.reset();
				}
			}
			doubleClickClock.restart();
		}	
	}
	if (event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Right && isMouseInsideGraphEditor()) {
		// Delete node
		bool isMouseOverNodeShape = false;
		for (auto i = nodesShapes.size(); i--;) {
			const auto& nodeShape = nodesShapes[i];
			if (nodeShape.getShape().getGlobalBounds().contains(mousePosition)) {
				isMouseOverNodeShape = true;
				
				graph.deleteNode(nodeShape.getNodeId());
				// Delete the node shapes
				nodesShapes.erase(nodesShapes.begin() + i);

				break;
			}
		}
		// If mouse is hovering over a node, it can't be hovering over an edge
		if (!isMouseOverNodeShape) {
			// Delete edge
			if (graph.isDirected()) {
				for (const auto& edgeShape : directedEdgesShapes) {
					if (edgeShape.contains(mousePosition)) {
						graph.deleteEdge(edgeShape.getStartNodeId(), edgeShape.getEndNodeId());

						break;
					}
				}
			}
			else {
				for (const auto& edgeShape : undirectedEdgesShapes) {
					if (edgeShape.contains(mousePosition)) {
						graph.deleteEdge(edgeShape.getStartNodeId(), edgeShape.getEndNodeId());

						break;
					}
				}
			}
		}
	}
	// Right mouse button to stop holding an edge
	if (event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Right && heldEdge.has_value()) {
		heldEdge.reset();
	}

	// TEMPORARY
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
		graph.print();
	}
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) {
		graph.printEdges();
	}
}

void GraphEditor::update(float deltaTime)
{
	if (heldEdge.has_value()) {
		heldEdge->setEndPosition(sf::Vector2f(sf::Mouse::getPosition(window)));
	}
}

void GraphEditor::draw(sf::RenderWindow& window)
{
	// Draw edges
	if (graph.isDirected()) {
		for (const auto& edgeShape : directedEdgesShapes) {
			edgeShape.draw(window);
		}
	}
	else {
		for (const auto& edgeShape : undirectedEdgesShapes) {
			edgeShape.draw(window);
		}
	}

	if (heldEdge.has_value()) {
		heldEdge->draw(window);
	}
	
	// Draw nodes
	for (const auto& nodeShape : nodesShapes) {
		nodeShape.draw(window);
	}
}

void GraphEditor::onDirectedEdgesDeleted(std::vector<std::pair<int, int>> deletedEdges)
{
	for (const auto& deletedEdge : deletedEdges) {
		directedEdgesShapes.erase(std::remove_if(directedEdgesShapes.begin(), directedEdgesShapes.end(), [&deletedEdge](const GraphEdgeShape& shape) {
			return shape.getStartNodeId() == deletedEdge.first && shape.getEndNodeId() == deletedEdge.second;
		}), directedEdgesShapes.end());
	}
}

void GraphEditor::onUndirectedEdgesDeleted(std::vector<std::pair<int, int>> deletedEdges)
{
	for (const auto& deletedEdge : deletedEdges) {
		undirectedEdgesShapes.erase(std::remove_if(undirectedEdgesShapes.begin(), undirectedEdgesShapes.end(), [&deletedEdge](const GraphEdgeShape& shape) {
			return (shape.getStartNodeId() == deletedEdge.first && shape.getEndNodeId() == deletedEdge.second)
				|| (shape.getStartNodeId() == deletedEdge.second && shape.getEndNodeId() == deletedEdge.first);
			}), undirectedEdgesShapes.end());
	}
}
