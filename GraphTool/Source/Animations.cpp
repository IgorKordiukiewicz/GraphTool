#include "../Include/Animations.hpp"
#include "../Include/GraphEditor.hpp"
#include "../Include/GraphEdgeShape.hpp"

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
		for (const auto a : traversalOrder.nodeOrder) {
			for (auto& nodeShape : parent->nodesShapes) {
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
				for (auto& edgeShape : parent->directedEdgesShapes) {
					if (edgeShape.getStartNodeId() == a && edgeShape.getEndNodeId() == b) {
						edgesShapesInOrder.push_back({ &edgeShape,false });
						break;
					}
				}
			}
			else {
				for (auto& edgeShape : parent->undirectedEdgesShapes) {
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
		const auto colors = Colors::generateColors(totalColorsCount);

		// Generate nodes colors
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
		index = 0;

		if (!nodesShapesInOrder.empty()) {
			nodesShapesInOrder[index].first->makeColored(nodesShapesInOrder[index].second);
		}
		if (!edgesShapesInOrder.empty()) {
			edgesShapesInOrder[index].first->activateEdgeTraversalAnimation(edgesShapesInOrder[index].second);
		}
		++index;
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
			if (clock.getElapsedTime().asSeconds() >= Constants::traversalAnimationTime) {
				if (index < nodesShapesInOrder.size()) {
					nodesShapesInOrder[index].first->makeColored(nodesShapesInOrder[index].second);
				}
				if (index < edgesShapesInOrder.size()) {
					edgesShapesInOrder[index].first->activateEdgeTraversalAnimation(edgesShapesInOrder[index].second);
				}
				++index;

				clock.restart();

				// Stop animation
				if (index == nodesShapesInOrder.size()) {
					running = false;
				}
			}
		}
		else if (active && !running && loop && clock.getElapsedTime().asSeconds() >= Constants::traversalAnimationTime) {
			run();
		}
	}

	void TraversalOrderAnimation::startLooping()
	{
		loop = true;
		if (active && !running) {
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
			const float dirVecLength = sqrt(dirVec.x * dirVec.x + dirVec.y * dirVec.y);
			const float lengthFraction = [this, &dirVecLength]() {
				float result = (clock.getElapsedTime().asSeconds() / Constants::traversalAnimationTime) * dirVecLength;
				result = std::clamp(result, 0.f, dirVecLength);
				return result;
			}();
			const sf::Vector2f dirVecNormalized = dirVec / dirVecLength;

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
			if (parent->directed == Directed::Yes && (clock.getElapsedTime().asSeconds() / Constants::traversalAnimationTime) > 0.95f) {
				coloredHeadVertices = parent->headVertices;
				coloredHeadVertices[0].color = Constants::mainColor;
				coloredHeadVertices[1].color = Constants::mainColor;
				coloredHeadVertices[2].color = Constants::mainColor;
			}

			if (clock.getElapsedTime().asSeconds() > Constants::traversalAnimationTime) {
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