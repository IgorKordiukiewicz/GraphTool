#include "../Include/GraphEdgeShape.hpp"
#include <iostream>
#include "../Include/ResourceManager.hpp"

GraphEdgeShape::GraphEdgeShape(const sf::Vector2f& startPosition, const sf::Vector2f& endPosition, int startNodeId, int endNodeId, Directed graphType, Weighted weighted)
	: startPosition(startPosition), endPosition(endPosition), startNodeId(startNodeId), endNodeId(endNodeId), directed(graphType), weighted(weighted)
{
	// Initialize vertices
	lineVertices.setPrimitiveType(sf::PrimitiveType::Lines);
	headVertices.setPrimitiveType(sf::PrimitiveType::Triangles);
	updateVertices();

	// Initialize weight text
	weightText.setFont(ResourceManager::instance().getSFMLFont());
	weightText.setCharacterSize(20);
	weightText.setString("0");
	const auto textRect = weightText.getLocalBounds();
	weightText.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
}

void GraphEdgeShape::draw(sf::RenderWindow& window) const
{
	window.draw(lineVertices);
	if (directed == Directed::Yes) {
		window.draw(headVertices);
	}

	if (weighted == Weighted::Yes) {
		window.draw(weightText);
	}
}

void GraphEdgeShape::setStartPosition(const sf::Vector2f& newStartPosition)
{
	startPosition = newStartPosition;
	updateVertices();
}

void GraphEdgeShape::setEndPosition(const sf::Vector2f& newEndPosition)
{
	endPosition = newEndPosition;
	updateVertices();
}

void GraphEdgeShape::setWeight(int newWeight)
{
	weightText.setString(std::to_string(newWeight));
}

void GraphEdgeShape::updateTextOpacityAnimation(float deltaTime)
{
	if (textOpacityAnimation.isActive) {
		if (textOpacityAnimation.isValueIncreased) {
			textOpacityAnimation.value += textOpacityAnimation.speed * deltaTime;
			if (textOpacityAnimation.value >= 255.f) {
				textOpacityAnimation.value = 255.f;
				textOpacityAnimation.isValueIncreased = false;
			}
		}
		else {
			textOpacityAnimation.value -= textOpacityAnimation.speed * deltaTime;
			if (textOpacityAnimation.value <= 0.f) {
				textOpacityAnimation.value = 0.f;
				textOpacityAnimation.isValueIncreased = true;
			}
		}

		weightText.setFillColor({ 255, 255, 255, static_cast<sf::Uint8>(textOpacityAnimation.value) });
	}
}

void GraphEdgeShape::startTextOpacityAnimation()
{
	textOpacityAnimation.isActive = true;
	// Reset text opacity animation properties
	textOpacityAnimation.value = 255.f;
	textOpacityAnimation.isValueIncreased = false;
}

void GraphEdgeShape::stopTextOpacityAnimation()
{
	textOpacityAnimation.isActive = false;
	// Reset the color to full opacity
	weightText.setFillColor({ 255, 255, 255, 255 });
}

void GraphEdgeShape::makeDirected()
{
	directed = Directed::Yes;
	updateVertices();
}

void GraphEdgeShape::makeUndirected()
{
	directed = Directed::No;
	isOrthogonalOffsetEnabled = false;
	updateVertices();
}

void GraphEdgeShape::makeWeighted()
{
	weighted = Weighted::Yes;
}

void GraphEdgeShape::makeUnweighted()
{
	weighted = Weighted::No;
}

void GraphEdgeShape::makeOrthogonalOffsetEnabled()
{
	isOrthogonalOffsetEnabled = true;
	updateVertices();
}

void GraphEdgeShape::makeOrthogonalOffsetDisabled()
{
	isOrthogonalOffsetEnabled = false;
	updateVertices();
}

bool GraphEdgeShape::contains(const sf::Vector2f& point) const
{
	auto isPointInsideLineBounds = [this, &point]() {
		auto calculateTriangleArea = [](const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& c) {
			return abs((b.x * a.y - a.x * b.y) + (c.x * b.y - b.x * c.y) + (a.x * c.y - c.x * a.y)) / 2.f;
		};
		const float area1 = calculateTriangleArea(lineBounds.a, point, lineBounds.b);
		const float area2 = calculateTriangleArea(lineBounds.a, point, lineBounds.d); 
		const float area3 = calculateTriangleArea(lineBounds.b, point, lineBounds.c);
		const float area4 = calculateTriangleArea(lineBounds.c, point, lineBounds.d);
		const float boundsArea = lineBounds.width * lineBounds.height;
		return (area1 + area2 + area3 + area4) <= boundsArea;
	};
	
	if (directed == Directed::Yes) {
		return isPointInsideLineBounds() || headVertices.getBounds().contains(point);
	}
	else {
		return isPointInsideLineBounds();
	}
}

void GraphEdgeShape::updateVertices()
{
	const sf::Vector2f dirVec = startPosition - endPosition;
	const float dirVecLength = sqrt(dirVec.x * dirVec.x + dirVec.y * dirVec.y);
	const sf::Vector2f dirVecNormalized = dirVec / dirVecLength;

	const sf::Vector2f orthDirVecNormalized{ dirVecNormalized.y * -1.f, dirVecNormalized.x };
	const sf::Vector2f offsetVec = orthDirVecNormalized * (isOrthogonalOffsetEnabled ? orthogonalOffset : 0.f);
	const sf::Vector2f startPositionFixed = startPosition + offsetVec;

	// If endNodeId is equal to -1, it means that the edge is held by the user, otherwise it is attached to a node, 
	// so the end position has to be recalculated so that the arrow head is not inside the node 
	const sf::Vector2f endPositionFixed = [this, &dirVec, &dirVecLength, &offsetVec]() {
		sf::Vector2f result = endPosition;
		result += offsetVec;
		if (endNodeId != -1) {
			return result + dirVec / dirVecLength * 16.f; // 16 is the node shape radius (TODO: make it not hardcoded)
		}
		else {
			return result;
		}
	}();
	
	lineVertices.clear();
	lineVertices.append(startPositionFixed);
	lineVertices.append(endPositionFixed);

	// Calculate the head triangle vertices
	if (directed == Directed::Yes) {
		const sf::Vector2f headBaseCenter = endPositionFixed + dirVecNormalized * headHeight;
		const sf::Vector2f dirVecOrth1 = { -dirVecNormalized.y, dirVecNormalized.x };
		const sf::Vector2f dirVecOrth2 = dirVecOrth1 * -1.f;
		const float halfHeadWidth = headWidth / 2.f;
		const sf::Vector2f headPoint1 = headBaseCenter + dirVecOrth1 * halfHeadWidth;
		const sf::Vector2f headPoint2 = headBaseCenter + dirVecOrth2 * halfHeadWidth;

		headVertices.clear();
		headVertices.append(headPoint1);
		headVertices.append(headPoint2);
		headVertices.append(endPositionFixed);
	}

	// Calculate line bounds
	const sf::Vector2f orthVector1 = orthDirVecNormalized * (lineBounds.height / 2.f);
	const sf::Vector2f orthVector2 = orthDirVecNormalized * -1.f * (lineBounds.height / 2.f);
	lineBounds.a = startPositionFixed + orthVector1;
	lineBounds.b = endPositionFixed + orthVector1;
	lineBounds.c = startPositionFixed + orthVector2;
	lineBounds.d = endPositionFixed + orthVector2;
	lineBounds.width = dirVecLength;

	// Update text position
	weightText.setPosition(startPositionFixed + (orthDirVecNormalized * weightTextOrthOffset) - (dirVec / 2.f));
}
