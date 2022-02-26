#include "../Include/Animations.hpp"
#include "../Include/GraphEditor.hpp"
#include "../Include/GraphEdgeShape.hpp"
#include <iostream>

namespace Animations
{
	void TraversalOrderAnimation::activate(const GraphAlgorithms::TraversalOrder& traversalOrder)
	{
		if (!parent) {
			return;
		}

		if (active) {
			deactivate();
		}

		active = true;
		this->traversalOrder = traversalOrder;

		nodesShapesInOrder.clear();
		for (const int a : traversalOrder.nodeOrder) {
			for (GraphNodeShape& nodeShape : parent->nodesShapes) {
				if (nodeShape.getNodeId() == a) {
					if (nodesColors.empty()) {
						nodesShapesInOrder.push_back({ &nodeShape, Constants::mainColor });
					}
					else {
						nodesShapesInOrder.push_back({ &nodeShape, nodesColors[a] });
					}
					break;
				}
			}
		}

		edgesShapesInOrder.clear();

		for (const auto [a, b] : traversalOrder.edgeOrder) {
			if (parent->graph.isDirected()) {
				for (GraphEdgeShape& edgeShape : parent->directedEdgesShapes) {
					if (edgeShape.getStartNodeId() == a && edgeShape.getEndNodeId() == b) {
						edgesShapesInOrder.push_back({ &edgeShape, false });
						break;
					}
				}
			}
			else {
				for (GraphEdgeShape& edgeShape : parent->undirectedEdgesShapes) {
					if (edgeShape.getStartNodeId() == a && edgeShape.getEndNodeId() == b) {
						edgesShapesInOrder.push_back({ &edgeShape, false });
						break;
					}
					else if (edgeShape.getStartNodeId() == b && edgeShape.getEndNodeId() == a) {
						edgesShapesInOrder.push_back({ &edgeShape, true });
						break;
					}
				}
			}
		}

		run();
	}

	void TraversalOrderAnimation::activate(const GraphAlgorithms::TraversalOrder& traversalOrder, const GraphAlgorithms::NodesColorsIdxs& nodesColorsIdxs)
	{
		if (!parent) {
			return;
		}

		const int totalColorsCount = [&nodesColorsIdxs]() {
			std::set<int> colorsIdxs;
			for (const auto [nodeId, colorIdx] : nodesColorsIdxs) {
				if (colorIdx.has_value()) {
					colorsIdxs.insert(*colorIdx);
				}
			}
			return colorsIdxs.size();
		}();
		const auto colors{ Colors::generateColors(totalColorsCount) };

		// Assign colors to nodes
		for (const auto [nodeId, colorIdx] : nodesColorsIdxs) {
			if (colorIdx.has_value()) {
				nodesColors.emplace(nodeId, colors[*colorIdx]);
			}
			else {
				nodesColors.emplace(nodeId, sf::Color{ 0, 0, 0 });
			}
		}

		activate(traversalOrder);
	}

	void TraversalOrderAnimation::run()
	{
		if (!active) {
			return;
		}

		deactivate();

		active = true;
		running = true;
		clock.restart();
		nodeShapeIdx = 0;
		edgeShapeIdx = 0;

		if (traversalOrder.instant) {
			for (auto [nodeShape, color] : nodesShapesInOrder) {
				nodeShape->makeColored(color);
			}

			running = false;
		}
		else {
			if (!nodesShapesInOrder.empty()) {
				nodesShapesInOrder[nodeShapeIdx].first->makeColored(nodesShapesInOrder[nodeShapeIdx].second);
				++nodeShapeIdx;
			}
			if (!edgesShapesInOrder.empty()) {
				edgesShapesInOrder[edgeShapeIdx].first->activateEdgeTraversalAnimation(edgesShapesInOrder[edgeShapeIdx].second);
				++edgeShapeIdx;
			}
		}
	}

	void TraversalOrderAnimation::deactivate()
	{
		if (!active) {
			return;
		}

		active = false;
		running = false;

		for (auto [nodeShape, color] : nodesShapesInOrder) {
			nodeShape->resetColor();
		}

		for (auto [edgeShape, reversedDirection] : edgesShapesInOrder) {
			edgeShape->deactivateEdgeTraversalAnimation();
		}

		nodesColors.clear();
	}

	void TraversalOrderAnimation::update()
	{
		if (active && running) {
			if (clock.getElapsedTime().asSeconds() >= Settings::instance().getTraversalAnimationTime()) {
				clock.restart();

				if (!edgesShapesInOrder.empty()) {
					if (edgeShapeIdx < edgesShapesInOrder.size()) {
						int nodesColoredThisIteration{ 0 };
						for (auto [nodeShape, color] : nodesShapesInOrder) {
							// Color this node if its id is equal to the last shape end id
							if (edgesShapesInOrder[edgeShapeIdx - 1].second) { // Check if direction is reversed
								if (nodeShape->getNodeId() == edgesShapesInOrder[edgeShapeIdx - 1].first->getStartNodeId()) {
									nodeShape->makeColored(color);
									++nodesColoredThisIteration;
								}
							}
							else {
								if (nodeShape->getNodeId() == edgesShapesInOrder[edgeShapeIdx - 1].first->getEndNodeId()) {
									nodeShape->makeColored(color);
									++nodesColoredThisIteration;
								}
							}
							// Color this node if its id is equal to the current shape start id
							if (edgesShapesInOrder[edgeShapeIdx].second) { // Check if direction is reversed
								if (nodeShape->getNodeId() == edgesShapesInOrder[edgeShapeIdx].first->getEndNodeId()) {
									nodeShape->makeColored(color);
									++nodesColoredThisIteration;
								}
							}
							else {
								if (nodeShape->getNodeId() == edgesShapesInOrder[edgeShapeIdx].first->getStartNodeId()) {
									nodeShape->makeColored(color);
									++nodesColoredThisIteration;
								}
							}

							// Only 2 nodes can be colored per iteration
							if (nodesColoredThisIteration >= 2) {
								break;
							}
						}

						// Activate edge traversal animation
						edgesShapesInOrder[edgeShapeIdx].first->activateEdgeTraversalAnimation(edgesShapesInOrder[edgeShapeIdx].second);
						++edgeShapeIdx;
					}
					// Color last edge's end id node
					else if (edgeShapeIdx == edgesShapesInOrder.size()) {
						for (auto [nodeShape, color] : nodesShapesInOrder) {
							if (edgesShapesInOrder[edgeShapeIdx - 1].second) {
								if (nodeShape->getNodeId() == edgesShapesInOrder[edgeShapeIdx - 1].first->getStartNodeId()) {
									nodeShape->makeColored(color);
									break;
								}
							}
							else {
								if (nodeShape->getNodeId() == edgesShapesInOrder[edgeShapeIdx - 1].first->getEndNodeId()) {
									nodeShape->makeColored(color);
									break;
								}
							}
						}

						running = false;
					}
				}
				else {
					// Color shape
					if (nodeShapeIdx < nodesShapesInOrder.size()) {
						nodesShapesInOrder[nodeShapeIdx].first->makeColored(nodesShapesInOrder[nodeShapeIdx].second);
						++nodeShapeIdx;
					}

					// Stop animation
					if (nodeShapeIdx == nodesShapesInOrder.size()) {
						running = false;
					}
				}
			}
		}
		// Looping
		else if (active && !running && loop && clock.getElapsedTime().asSeconds() >= Settings::instance().getTraversalAnimationTime() && !traversalOrder.instant) {
			run();
		}
	}

	void TraversalOrderAnimation::startLooping()
	{
		loop = true;
		if (active && !running && !traversalOrder.instant) {
			run();
		}
	}

	void TraversalOrderAnimation::stopLooping()
	{
		loop = false;
	}

	void EdgeTraversalAnimation::activate(bool reversedDirection)
	{
		active = true;
		running = true;
		clock.restart();
		this->reversedDirection = reversedDirection;
	}

	void EdgeTraversalAnimation::deactivate()
	{
		active = false;
		running = false;
		coloredLineVertices.clear();
		coloredHeadVertices.clear();
	}

	void EdgeTraversalAnimation::update(float deltaTime)
	{
		if (active && running) {
			const sf::Vector2f dirVec = [this]() {
				if (reversedDirection) {
					return parent->startPositionFixed - parent->endPositionFixed;
				}
				else {
					return parent->endPositionFixed - parent->startPositionFixed;
				}
			}();
			const float dirVecLength{ sqrt(dirVec.x * dirVec.x + dirVec.y * dirVec.y) };
			const float lengthFraction = [this, &dirVecLength]() {
				float result{ (clock.getElapsedTime().asSeconds() / Settings::instance().getTraversalAnimationTime()) * dirVecLength };
				result = std::clamp(result, 0.f, dirVecLength);
				return result;
			}();
			const sf::Vector2f dirVecNormalized{ dirVec / dirVecLength };

			// Update colored line vertices
			coloredLineVertices.clear();
			if (reversedDirection) {
				coloredLineVertices.append({ parent->endPositionFixed, Constants::mainColor });
				coloredLineVertices.append({ parent->endPositionFixed + dirVecNormalized * lengthFraction, Constants::mainColor });
			}
			else {
				coloredLineVertices.append({ parent->startPositionFixed, Constants::mainColor });
				coloredLineVertices.append({ parent->startPositionFixed + dirVecNormalized * lengthFraction, Constants::mainColor });
			}

			// Update colored head vertices
			if (parent->directed == Directed::Yes && (clock.getElapsedTime().asSeconds() / Settings::instance().getTraversalAnimationTime()) > 0.95f) {
				coloredHeadVertices = parent->headVertices;
				coloredHeadVertices[0].color = Constants::mainColor;
				coloredHeadVertices[1].color = Constants::mainColor;
				coloredHeadVertices[2].color = Constants::mainColor;
			}

			if (clock.getElapsedTime().asSeconds() > Settings::instance().getTraversalAnimationTime()) {
				running = false;
			}
		}
	}

	void TextOpacityAnimation::activate()
	{
		active = true;
		// Reset animation properties
		value = 255.f;
		isValueIncreased = false;
	}

	void TextOpacityAnimation::deactivate()
	{
		active = false;
		// Reset the color to full opacity
		parent->weightText.setFillColor(sf::Color::White);
	}

	void TextOpacityAnimation::update(float deltaTime)
	{
		if (active) {
			if (isValueIncreased) {
				value += speed * deltaTime;
				if (value >= 255.f) {
					value = 255.f;
					isValueIncreased = false;
				}
			}
			else {
				value -= speed * deltaTime;
				if (value <= 0.f) {
					value = 0.f;
					isValueIncreased = true;
				}
			}

			parent->weightText.setFillColor({ 255, 255, 255, static_cast<sf::Uint8>(value) });
		}
	}
}