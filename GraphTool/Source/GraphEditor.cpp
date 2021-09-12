#include "..\Include\GraphEditor.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

GraphEditor::GraphEditor(Graph& graph, sf::RenderWindow& window)
	: graph(graph), window(window)
{
	if (!font.loadFromFile("Resources/consola.ttf")) {
		std::cout << "Couldn't load font from file!\n";
	}

	graph.onDirectedEdgesDeleted.connect("GraphEditor", this, &GraphEditor::onDirectedEdgesDeleted);
	graph.onUndirectedEdgesDeleted.connect("GraphEditor", this, &GraphEditor::onUndirectedEdgesDeleted);
	graph.onWeightedValueChanged.connect("GraphEditor", this, &GraphEditor::onGraphWeightedValueChanged);
}

void GraphEditor::processEvents(sf::Event& event)
{
	const auto mousePosition = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });

	// Press any mouse button or enter to stop editing edge weight
	if (editedEdge && (event.type == sf::Event::MouseButtonPressed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter))) {
		stopEditingEdgeWeight();
	}
	else if (editedEdge && event.type == sf::Event::KeyPressed) {
		editEdgeWeight(event);
	}
	else if (event.type == sf::Event::MouseButtonPressed 
		&& event.mouseButton.button == sf::Mouse::Left 
		&& isMouseInsideGraphEditor()
		&& !heldNodePtrs.has_value()) {
		// Check if mouse is on top of any of the nodes shapes
		bool isMouseOverNodeShape = false;
		for (const auto& nodeShape : nodesShapes) {
			if (nodeShape.getShape().getGlobalBounds().contains(mousePosition)) {
				isMouseOverNodeShape = true;

				// If user was already holding an edge, attach it to this node
				if (heldEdge.has_value()) {
					attachHeldEdge(nodeShape);
				}
				else {
					createNewEdge(nodeShape, mousePosition);
				}

				break;
			}
		}

		if (!isMouseOverNodeShape) {
			// Double click to add new node
			if (doubleClickClock.getElapsedTime().asSeconds() <= doubleClickDelay) {
				createNewNode(mousePosition);
			}
			doubleClickClock.restart();

			if (graph.isWeighted()) {
				startEditingEdgeWeightIfNeeded(mousePosition);
			}
			
		}	
	}
	// Delete node/edge
	else if (event.type == sf::Event::MouseButtonPressed 
		&& event.key.code == sf::Mouse::Right 
		&& isMouseInsideGraphEditor()
		&& !heldNodePtrs.has_value()
		&& !heldEdge.has_value()) {
		bool wasAnyNodeDeleted = deleteNodeIfNeeded(mousePosition);

		// If any node was deleted, then mouse was on top of a node, so it can't be on top of any edge
		if (!wasAnyNodeDeleted) {
			deleteEdgeIfNeeded(mousePosition);
		}
	}
	// Stop holding an edge
	else if (event.type == sf::Event::MouseButtonPressed 
		&& event.key.code == sf::Mouse::Right 
		&& heldEdge.has_value()) {
		heldEdge.reset();
	}
	// Move a node
	else if (event.type == sf::Event::MouseButtonPressed 
		&& event.key.code == sf::Mouse::Middle 
		&& isMouseInsideGraphEditor() 
		&& !heldNodePtrs.has_value() 
		&& !heldEdge.has_value()) {
		for (auto& nodeShape : nodesShapes) {
			if (nodeShape.getShape().getGlobalBounds().contains(mousePosition)) {
				startHoldingNode(nodeShape);
			}
		}
	}
	// Stop moving the held node
	else if (event.type == sf::Event::MouseButtonReleased 
		&& event.key.code == sf::Mouse::Middle 
		&& heldNodePtrs.has_value()) {
		heldNodePtrs.reset();
	}
}

void GraphEditor::update(float deltaTime)
{
	if (heldEdge.has_value()) {
		updateHeldEdge();
	}

	if (heldNodePtrs.has_value()) {
		updateHeldNodePtrs();
	}

	updateTextOpacityAnimationIfNeeded(deltaTime);
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

void GraphEditor::onGraphWeightedValueChanged()
{
	auto updateEdgesShapes = [](auto& edgesShapes, bool makeWeighted) {
		for (auto& edgeShape : edgesShapes) {
			if (makeWeighted) {
				edgeShape.makeWeighted();
			}
			else {
				edgeShape.makeUnweighted();
			}
		}
	};
	
	if (graph.isWeighted()) {
		updateEdgesShapes(directedEdgesShapes, true);
		updateEdgesShapes(undirectedEdgesShapes, true);
	}
	else {
		updateEdgesShapes(directedEdgesShapes, false);
		updateEdgesShapes(undirectedEdgesShapes, false);
	}
}

bool GraphEditor::isMouseInsideGraphEditor() const
{
	const sf::FloatRect graphViewRect{
			static_cast<float>(window.getSize().x) * 0.25f, 0.f, // 0.25f, because the imgui window's length is 1/4 of the app window length, TODO: make it not hardcoded
			static_cast<float>(window.getSize().x) * 0.75f, static_cast<float>(window.getSize().y) };
	return graphViewRect.contains(sf::Vector2f(sf::Mouse::getPosition(window)));
}

void GraphEditor::stopEditingEdgeWeight()
{
	editedEdge->stopTextOpacityAnimation();
	editedEdge = nullptr;
}

void GraphEditor::editEdgeWeight(sf::Event& event)
{
	// If key pressed is a number, update the edited edge's weight
	const auto newEdgeWeight = [&event]()->std::optional<int> {
		return (event.key.code >= 26 && event.key.code <= 35) ? event.key.code - 26 : std::optional<int>{};
	}();
	if (newEdgeWeight.has_value()) {
		graph.setEdgeWeight(editedEdge->getStartNodeId(), editedEdge->getEndNodeId(), *newEdgeWeight);
		editedEdge->setWeight(*newEdgeWeight);
	}
}

void GraphEditor::attachHeldEdge(const GraphNodeShape& nodeShape)
{
	// Prevent having duplicate edge shapes
	if (auto [nodeA, nodeB] = std::pair{ heldEdge->getStartNodeId(), nodeShape.getNodeId() }; !graph.doesEdgeExist(nodeA, nodeB)) {
		directedEdgesShapes.push_back(GraphEdgeShape{ heldEdge->getStartPosition(), nodeShape.getShape().getPosition(), nodeA, nodeB, Directed::Yes,
			(graph.isWeighted() ? Weighted::Yes : Weighted::No) });
		if (!graph.doesEdgeExist(nodeB, nodeA)) {
			undirectedEdgesShapes.push_back(GraphEdgeShape{ heldEdge->getStartPosition(), nodeShape.getShape().getPosition(), nodeA, nodeB, Directed::No,
				(graph.isWeighted() ? Weighted::Yes : Weighted::No) });
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

void GraphEditor::createNewEdge(const GraphNodeShape& nodeShape, const sf::Vector2f& mousePosition)
{
	heldEdge = GraphEdgeShape{ nodeShape.getShape().getPosition(), mousePosition, nodeShape.getNodeId(), -1,
						(graph.isDirected() ? Directed::Yes : Directed::No), (graph.isWeighted() ? Weighted::Yes : Weighted::No) };
}

void GraphEditor::createNewNode(const sf::Vector2f& mousePosition)
{
	const int newNodeId = graph.createNode();
	nodesShapes.push_back(GraphNodeShape{ newNodeId, mousePosition });

	// If user was holding an edge, attach it to the created node
	if (heldEdge.has_value()) {
		graph.addEdge(heldEdge->getStartNodeId(), newNodeId);
		directedEdgesShapes.push_back(GraphEdgeShape{ heldEdge->getStartPosition(), mousePosition, heldEdge->getStartNodeId(), newNodeId, Directed::Yes,
			(graph.isWeighted() ? Weighted::Yes : Weighted::No) });
		undirectedEdgesShapes.push_back(GraphEdgeShape{ heldEdge->getStartPosition(), mousePosition, heldEdge->getStartNodeId(), newNodeId, Directed::No,
			(graph.isWeighted() ? Weighted::Yes : Weighted::No) });
		heldEdge.reset();
	}
}

void GraphEditor::startEditingEdgeWeightIfNeeded(const sf::Vector2f& mousePosition)
{
	auto startEditingEdgeWeight = [this, &mousePosition](auto& edgesShapes) {
		for (auto& edgeShape : edgesShapes) {
			if (edgeShape.getWeightText().getGlobalBounds().contains(mousePosition)) {
				editedEdge = &edgeShape;
				editedEdge->startTextOpacityAnimation();
				break;
			}
		}
	};

	if (graph.isDirected()) {
		startEditingEdgeWeight(directedEdgesShapes);
	}
	else {
		startEditingEdgeWeight(undirectedEdgesShapes);
	}
}

bool GraphEditor::deleteNodeIfNeeded(const sf::Vector2f& mousePosition)
{
	for (auto i = nodesShapes.size(); i--;) {
		const auto& nodeShape = nodesShapes[i];
		if (nodeShape.getShape().getGlobalBounds().contains(mousePosition)) {
			graph.deleteNode(nodeShape.getNodeId());
			// Delete the node shapes
			nodesShapes.erase(nodesShapes.begin() + i);

			return true;
		}
	}

	return false;
}

void GraphEditor::deleteEdgeIfNeeded(const sf::Vector2f& mousePosition)
{
	if (graph.isDirected()) {
		for (const auto& edgeShape : directedEdgesShapes) {
			if (edgeShape.contains(mousePosition)) {
				graph.deleteEdge(edgeShape.getStartNodeId(), edgeShape.getEndNodeId());
				return;
			}
		}
	}
	else {
		for (const auto& edgeShape : undirectedEdgesShapes) {
			if (edgeShape.contains(mousePosition)) {
				graph.deleteEdge(edgeShape.getStartNodeId(), edgeShape.getEndNodeId());
				return;
			}
		}
	}
}

void GraphEditor::startHoldingNode(GraphNodeShape& nodeShape)
{
	heldNodePtrs.emplace(HeldNodePtrs{});
	heldNodePtrs->heldNodeShape = &nodeShape;

	// Find edges connected to this node
	auto findEdgesConnectedToHeldNode = [&nodeShape](auto& edgesShapes, auto& heldNodePtrs) {
		for (auto& edgeShape : edgesShapes) {
			if (edgeShape.getStartNodeId() == nodeShape.getNodeId()) {
				heldNodePtrs->connectedEdgesShapes.push_back({ &edgeShape, true });
			}
			else if (edgeShape.getEndNodeId() == nodeShape.getNodeId()) {
				heldNodePtrs->connectedEdgesShapes.push_back({ &edgeShape, false });
			}
		}
	};
	findEdgesConnectedToHeldNode(directedEdgesShapes, heldNodePtrs);
	findEdgesConnectedToHeldNode(undirectedEdgesShapes, heldNodePtrs);
}

void GraphEditor::updateHeldEdge()
{
	heldEdge->setEndPosition(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
}

void GraphEditor::updateHeldNodePtrs()
{
	if (isMouseInsideGraphEditor()) {
		// Update held node's shape position
		const auto mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
		heldNodePtrs->heldNodeShape->setPosition(mousePosition);
		// Update connected edges positions
		for (auto [edgeShape, isStartPositionAttached] : heldNodePtrs->connectedEdgesShapes) {
			if (isStartPositionAttached) {
				edgeShape->setStartPosition(mousePosition);
			}
			else {
				edgeShape->setEndPosition(mousePosition);
			}
		}
	}
	// If mouse goes out of graph editor, stop holding the node
	else {
		heldNodePtrs.reset();
	}
}

void GraphEditor::updateTextOpacityAnimationIfNeeded(float deltaTime)
{
	auto updateEdgeTextOpacityAnimation = [deltaTime](auto& edgesShapes) {
		for (auto& edgeShape : edgesShapes) {
			if (edgeShape.isTextOpacityAnimationActive()) {
				edgeShape.updateTextOpacityAnimation(deltaTime);
			}
		}
	};
	if (graph.isDirected()) {
		updateEdgeTextOpacityAnimation(directedEdgesShapes);
	}
	else {
		updateEdgeTextOpacityAnimation(undirectedEdgesShapes);
	}
}
