#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Text.hpp>
#include "Graph.hpp"
#include <array>
#include "Utilities.hpp"

class GraphEdgeShape
{
public:
	GraphEdgeShape(const sf::Vector2f& startPosition, const sf::Vector2f& endPosition, int startNodeId, int endNodeId, Directed graphType, Weighted weighted);

	void update(float deltaTime);
	void draw(sf::RenderWindow& window) const;

	void setStartPosition(const sf::Vector2f& newStartPosition);
	void setEndPosition(const sf::Vector2f& newEndPosition);
	void setWeight(int newWeight);

	void activateTextOpacityAnimation();
	void deactivateTextOpacityAnimation();
	void activateEdgeTraversalAnimation(bool reversedDirection);
	void deactivateEdgeTraversalAnimation();

	void makeDirected();
	void makeUndirected();
	void makeWeighted();
	void makeUnweighted();
	void makeOrthogonalOffsetEnabled();
	void makeOrthogonalOffsetDisabled();

	bool contains(const sf::Vector2f& point) const;

	const sf::Vector2f& getStartPosition() const { return startPosition; }
	const sf::Vector2f& getEndPosition() const { return endPosition; }
	int getStartNodeId() const { return startNodeId; }
	int getEndNodeId() const { return endNodeId; }

	const sf::Text& getWeightText() const { return weightText; }

private:
	void updateVertices();

private:
	Directed directed;
	Weighted weighted;

	sf::Vector2f startPosition;
	sf::Vector2f endPosition;
	sf::Vector2f startPositionFixed;
	sf::Vector2f endPositionFixed;
	int startNodeId;
	int endNodeId;

	sf::VertexArray lineVertices;
	sf::VertexArray headVertices;
	float headHeight = 10.f;
	float headWidth = 11.5f;
	float orthogonalOffset = 5.f;
	bool isOrthogonalOffsetEnabled{ false };
	struct LineBounds 
	{
		sf::Vector2f a, b, c, d;
		float width{ 0.f };
		float height{ 4.f };
	} lineBounds;

	sf::Text weightText;
	float weightTextOrthOffset = 10.f;

	class TextOpacityAnimation
	{
		bool active{ false };
		float speed{ 400.f };
		float value{ 255.f };
		bool isValueIncreased{ false };

	public:
		GraphEdgeShape* parent{ nullptr };

		void activate();
		void deactivate();
		void update(float deltaTime);
	} textOpacityAnimation;

	class EdgeTraversalAnimation
	{
		bool active{ false };
		bool running{ false };
		// By default animation direction will be from startPosition to endPosition
		bool reversedDirection{ false };
		sf::Clock clock;

	public:
		GraphEdgeShape* parent{ nullptr };
		sf::VertexArray coloredLineVertices;
		sf::VertexArray coloredHeadVertices;

		void activate(bool reversedDirection);
		void deactivate();
		void update(float deltaTime);
		bool isActive() const { return active; }
	} edgeTraversalAnimation;
};