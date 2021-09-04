#include "../Include/GraphEdgeShape.hpp"

GraphEdgeShape::GraphEdgeShape(const sf::Vector2f& startPosition, const sf::Vector2f& endPosition, int startNodeId, int endNodeId, GraphType graphType)
	: startPosition(startPosition), endPosition(endPosition), startNodeId(startNodeId), endNodeId(endNodeId), graphType(graphType)
{
	lineVertices.setPrimitiveType(sf::PrimitiveType::Lines);
	headVertices.setPrimitiveType(sf::PrimitiveType::Triangles);
	updateVertices();
}

void GraphEdgeShape::draw(sf::RenderWindow& window)
{
	window.draw(lineVertices);
	if (graphType == GraphType::Directed) {
		window.draw(headVertices);
	}
}

void GraphEdgeShape::setEndPosition(const sf::Vector2f& newEndPosition)
{
	endPosition = newEndPosition;
	updateVertices();
}

void GraphEdgeShape::makeDirected()
{
	graphType = GraphType::Directed;
	updateVertices();
}

void GraphEdgeShape::makeUndirected()
{
	graphType = GraphType::Undirected;
	updateVertices();
}

void GraphEdgeShape::updateVertices()
{
	lineVertices.clear();
	lineVertices.append(startPosition);
	lineVertices.append(endPosition);

	// Calculate the head triangle vertices
	if (graphType == GraphType::Directed) {
		//const sf::Vector2f dirVec = endPosition - startPosition;
		const sf::Vector2f dirVec = startPosition - endPosition;
		const float dirVecLength = sqrt(dirVec.x * dirVec.x + dirVec.y * dirVec.y);

		// If endNodeId is -1, it means that the edge is held by the user, otherwise it is attached to a node, 
		// so the end position has to be recalculated so that the arrow head is not inside the node 
		if (endNodeId != -1) {
			endPosition = endPosition + dirVec / dirVecLength * 16.f; // 16 is the node shape radius (TODO: make it not hardcoded)
		}

		const sf::Vector2f dirVecNormalized = dirVec / dirVecLength;
		const sf::Vector2f headBaseCenter = endPosition + dirVecNormalized * headHeight;
		const sf::Vector2f dirVecOrth1 = { -dirVecNormalized.y, dirVecNormalized.x };
		const sf::Vector2f dirVecOrth2 = { dirVecNormalized.y, -dirVecNormalized.x };
		const float halfHeadWidth = headWidth / 2.f;
		const sf::Vector2f headPoint1 = headBaseCenter + dirVecOrth1 * halfHeadWidth;
		const sf::Vector2f headPoint2 = headBaseCenter + dirVecOrth2 * halfHeadWidth;

		headVertices.clear();
		headVertices.append(headPoint1);
		headVertices.append(headPoint2);
		headVertices.append(endPosition);
	}
}
