#include "..\Include\GraphEditor.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "../Include/Utilities.hpp"
#include "../Include/ResourceManager.hpp"
#include <algorithm>

GraphEditor::GraphEditor(Graph& graph, sf::RenderWindow& window)
	: graph(graph), window(window)
{
	background.setTexture(ResourceManager::instance().getEditorBackgroundTexture());
	background.setPosition({ window.getSize().x * Constants::imGuiWindowWidth, 0.f });

	graph.onDirectedEdgesDeleted.connect("GraphEditor", this, &GraphEditor::onGraphDirectedEdgesDeleted);
	graph.onUndirectedEdgesDeleted.connect("GraphEditor", this, &GraphEditor::onGraphUndirectedEdgesDeleted);
	graph.onWeightedValueChanged.connect("GraphEditor", this, &GraphEditor::onGraphWeightedValueChanged);

	traversalOrderAnimation.parent = this;
}

void GraphEditor::processEvents(sf::Event& event)
{
	if (currentPanel != Panel::EditPanel) {
		return;
	}

	const sf::Vector2f mousePosition{ window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y }) };

	// Press any mouse button or enter to stop editing edge weight
	if (editedEdge && (event.type == sf::Event::MouseButtonPressed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter))) {
		stopEditingEdgeWeight();
		startEditingEdgeWeightIfRequired(mousePosition);
	}
	else if (editedEdge && event.type == sf::Event::KeyPressed) {
		editEdgeWeight(event);
	}
	else if (event.type == sf::Event::MouseButtonPressed
		&& event.mouseButton.button == sf::Mouse::Left
		&& isMouseInsideGraphEditor()
		&& !heldNodePtrs.has_value()) {
		// Check if mouse is on top of any of the nodes shapes
		bool isMouseOverNodeShape{ false };
		for (const GraphNodeShape& nodeShape : nodesShapes) {
			if (nodeShape.getShape().getGlobalBounds().contains(mousePosition)) {
				isMouseOverNodeShape = true;

				// If user was already holding an edge, attach it to this node
				if (heldEdge.has_value() && heldEdge->getStartNodeId() != nodeShape.getNodeId()) {
					attachHeldEdge(nodeShape);
				}
				else {
					createNewHeldEdge(nodeShape, mousePosition);
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
				startEditingEdgeWeightIfRequired(mousePosition);
			}
		}
	}
	// Delete node/edge
	else if (event.type == sf::Event::MouseButtonPressed
		&& event.key.code == sf::Mouse::Right
		&& isMouseInsideGraphEditor()
		&& !heldNodePtrs.has_value()
		&& !heldEdge.has_value()) {
		const bool wasAnyNodeDeleted{ deleteNodeIfRequired(mousePosition) };
		// If any node was deleted, then mouse was on top of a node, so it can't be on top of any edge
		if (!wasAnyNodeDeleted) {
			deleteEdgeIfRequired(mousePosition);
		}
	}
	// Stop holding an edge
	else if (heldEdge.has_value() && ((event.type == sf::Event::MouseButtonPressed && event.key.code == sf::Mouse::Right)
		|| (event.type == sf::Event::MouseButtonPressed && !isMouseInsideGraphEditor()))) {
		heldEdge.reset();
	}
	// Move a node
	else if (event.type == sf::Event::MouseButtonPressed
		&& event.key.code == sf::Mouse::Middle
		&& isMouseInsideGraphEditor()
		&& !heldNodePtrs.has_value()
		&& !heldEdge.has_value()) {
		for (GraphNodeShape& nodeShape : nodesShapes) {
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

	// Update edges shapes
	if (graph.isDirected()) {
		for (GraphEdgeShape& edgeShape : directedEdgesShapes) {
			edgeShape.update(deltaTime);
		}
	}
	else {
		for (GraphEdgeShape& edgeShape : undirectedEdgesShapes) {
			edgeShape.update(deltaTime);
		}
	}

	traversalOrderAnimation.update();
}

void GraphEditor::draw(sf::RenderWindow& window) const
{
	window.draw(background);

	// Draw edges
	if (graph.isDirected()) {
		for (const GraphEdgeShape& edgeShape : directedEdgesShapes) {
			edgeShape.draw(window);
		}
	}
	else {
		for (const GraphEdgeShape& edgeShape : undirectedEdgesShapes) {
			edgeShape.draw(window);
		}
	}

	if (heldEdge.has_value()) {
		heldEdge->draw(window);
	}

	// Draw nodes
	for (const GraphNodeShape& nodeShape : nodesShapes) {
		nodeShape.draw(window);
	}
}

void GraphEditor::setCurrentPanel(Panel newCurrentPanel)
{
	currentPanel = newCurrentPanel;

	if (currentPanel != Panel::AlgorithmsPanel && traversalOrderAnimation.isActive()) {
		traversalOrderAnimation.deactivate();
	}
}

void GraphEditor::activateTraversalOrderAnimation(const GraphAlgorithms::TraversalOrder& traversalOrder)
{
	traversalOrderAnimation.activate(traversalOrder);
}

void GraphEditor::activateTraversalOrderAnimation(const GraphAlgorithms::TraversalOrder& traversalOrder, const GraphAlgorithms::NodesColorsIdxs& nodesColorsIdxs)
{
	traversalOrderAnimation.activate(traversalOrder, nodesColorsIdxs);
}

void GraphEditor::deactivateTraversalOrderAnimation()
{
	traversalOrderAnimation.deactivate();
}

void GraphEditor::startLoopingTraversalOrderAnimation()
{
	traversalOrderAnimation.startLooping();
}

void GraphEditor::stopLoopingTraversalOrderAnimation()
{
	traversalOrderAnimation.stopLooping();
}

bool GraphEditor::isMouseInsideGraphEditor() const
{
	const sf::FloatRect graphViewRect{
			static_cast<float>(window.getSize().x) * Constants::imGuiWindowWidth, 0.f,
			static_cast<float>(window.getSize().x) * (1.0f - Constants::imGuiWindowWidth), static_cast<float>(window.getSize().y) };
	return graphViewRect.contains(sf::Vector2f(sf::Mouse::getPosition(window)));
}

void GraphEditor::stopEditingEdgeWeight()
{
	editedEdge->deactivateTextOpacityAnimation();
	editedEdge = nullptr;
}

void GraphEditor::editEdgeWeight(sf::Event& event)
{
	// Check if pressed key is a digit
	const auto digit = [&event]() -> std::optional<int> {
		return (event.key.code >= 26 && event.key.code <= 35) ? event.key.code - 26 : std::optional<int>{};
	}();

	const int currentWeight{ graph.getEdgeWeight(editedEdge->getStartNodeId(), editedEdge->getEndNodeId()) };
	if (digit.has_value()) {
		const int newWeight = [this, currentWeight, &digit]() {
			if (changedEditedEdgeWeight) {
				if (currentWeight < 10) { // Weight can be at most a 2 digit number
					return currentWeight * 10 + *digit;
				}
				else {
					return currentWeight;
				}
			}
			else {
				return *digit;
			}
		}();
		graph.setEdgeWeight(editedEdge->getStartNodeId(), editedEdge->getEndNodeId(), newWeight);
		editedEdge->setWeight(newWeight);
		changedEditedEdgeWeight = true;
	}
	else if (event.key.code == sf::Keyboard::BackSpace) {
		const int newWeight{ currentWeight / 10 };
		graph.setEdgeWeight(editedEdge->getStartNodeId(), editedEdge->getEndNodeId(), newWeight);
		editedEdge->setWeight(newWeight);
		changedEditedEdgeWeight = true;
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
			for (GraphEdgeShape& edgeShape : directedEdgesShapes) {
				if (edgeShape.getStartNodeId() == nodeB && edgeShape.getEndNodeId() == nodeA) {
					edgeShape.makeOrthogonalOffsetEnabled();
					break;
				}
			}
		}
	}
}

void GraphEditor::createNewHeldEdge(const GraphNodeShape& nodeShape, const sf::Vector2f& mousePosition)
{
	heldEdge = GraphEdgeShape{ nodeShape.getShape().getPosition(), mousePosition, nodeShape.getNodeId(), -1,
						(graph.isDirected() ? Directed::Yes : Directed::No), (graph.isWeighted() ? Weighted::Yes : Weighted::No) };
}

void GraphEditor::createNewNode(const sf::Vector2f& mousePosition)
{
	const int newNodeId{ graph.createNode() };
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

std::pair<GraphEdgeShape*, GraphEdgeShape*> GraphEditor::createNewEdge(int a, int b)
{
	GraphNodeShape& nodeShapeA{ *std::find_if(nodesShapes.begin(), nodesShapes.end(), [a](const auto& nodeShape) { return nodeShape.getNodeId() == a; }) };
	GraphNodeShape& nodeShapeB{ *std::find_if(nodesShapes.begin(), nodesShapes.end(), [b](const auto& nodeShape) { return nodeShape.getNodeId() == b; }) };
	GraphEdgeShape* directedEdgeShapePtr{ nullptr };
	GraphEdgeShape* undirectedEdgeShapePtr{ nullptr };

	if (!graph.doesDirectedEdgeExist(a, b)) {
		directedEdgesShapes.push_back(GraphEdgeShape{ nodeShapeA.getShape().getPosition(), nodeShapeB.getShape().getPosition(), a, b, Directed::Yes,
			(graph.isWeighted() ? Weighted::Yes : Weighted::No) });
		directedEdgeShapePtr = &directedEdgesShapes.back();

		if (!graph.doesUndirectedEdgeExist(a, b)) {
			undirectedEdgesShapes.push_back(GraphEdgeShape{ nodeShapeA.getShape().getPosition(), nodeShapeB.getShape().getPosition(), a, b, Directed::No,
				(graph.isWeighted() ? Weighted::Yes : Weighted::No) });
			undirectedEdgeShapePtr = &undirectedEdgesShapes.back();
		}

		graph.addEdge(a, b);

		// If edges is directed, and there are edges A -> B and B -> A,
		// set these edges shapes to use orthogonal offset to prevent them from overlapping each other
		if (graph.doesDirectedEdgeExist(b, a)) {
			directedEdgesShapes.back().makeOrthogonalOffsetEnabled();
			for (GraphEdgeShape& edgeShape : directedEdgesShapes) {
				if (edgeShape.getStartNodeId() == b && edgeShape.getEndNodeId() == a) {
					edgeShape.makeOrthogonalOffsetEnabled();
					break;
				}
			}
		}
	}

	return { directedEdgeShapePtr, undirectedEdgeShapePtr };
}

void GraphEditor::startEditingEdgeWeightIfRequired(const sf::Vector2f& mousePosition)
{
	auto startEditingEdgeWeight = [this, &mousePosition](auto& edgesShapes) {
		for (GraphEdgeShape& edgeShape : edgesShapes) {
			if (edgeShape.getWeightText().getGlobalBounds().contains(mousePosition)) {
				editedEdge = &edgeShape;
				editedEdge->activateTextOpacityAnimation();
				changedEditedEdgeWeight = false;

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

bool GraphEditor::deleteNodeIfRequired(const sf::Vector2f& mousePosition)
{
	for (auto i{ nodesShapes.size() }; i--;) {
		const GraphNodeShape& nodeShape = nodesShapes[i];
		if (nodeShape.getShape().getGlobalBounds().contains(mousePosition)) {
			graph.deleteNode(nodeShape.getNodeId());
			// Delete the node shapes
			nodesShapes.erase(nodesShapes.begin() + i);

			return true;
		}
	}

	return false;
}

void GraphEditor::deleteEdgeIfRequired(const sf::Vector2f& mousePosition)
{
	if (graph.isDirected()) {
		for (const GraphEdgeShape& edgeShape : directedEdgesShapes) {
			if (edgeShape.contains(mousePosition)) {
				graph.deleteEdge(edgeShape.getStartNodeId(), edgeShape.getEndNodeId());
				return;
			}
		}
	}
	else {
		for (const GraphEdgeShape& edgeShape : undirectedEdgesShapes) {
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
		for (GraphEdgeShape& edgeShape : edgesShapes) {
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
		const sf::Vector2f mousePosition{ window.mapPixelToCoords(sf::Mouse::getPosition(window)) };
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

void GraphEditor::onGraphDirectedEdgesDeleted(std::vector<std::pair<int, int>> deletedEdges)
{
	for (const std::pair<int, int>& deletedEdge : deletedEdges) {
		directedEdgesShapes.erase(std::remove_if(directedEdgesShapes.begin(), directedEdgesShapes.end(), [&deletedEdge](const GraphEdgeShape& shape) {
			return shape.getStartNodeId() == deletedEdge.first && shape.getEndNodeId() == deletedEdge.second;
			}), directedEdgesShapes.end());
	}
}

void GraphEditor::onGraphUndirectedEdgesDeleted(std::vector<std::pair<int, int>> deletedEdges)
{
	for (const std::pair<int, int>& deletedEdge : deletedEdges) {
		undirectedEdgesShapes.erase(std::remove_if(undirectedEdgesShapes.begin(), undirectedEdgesShapes.end(), [&deletedEdge](const GraphEdgeShape& shape) {
			return (shape.getStartNodeId() == deletedEdge.first && shape.getEndNodeId() == deletedEdge.second)
				|| (shape.getStartNodeId() == deletedEdge.second && shape.getEndNodeId() == deletedEdge.first);
			}), undirectedEdgesShapes.end());
	}
}

void GraphEditor::onGraphWeightedValueChanged()
{
	auto updateEdgesShapes = [](auto& edgesShapes, bool makeWeighted) {
		for (GraphEdgeShape& edgeShape : edgesShapes) {
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
