#pragma once

#include <map>
#include <vector>
#include "GraphAlgorithms.hpp"
#include "GraphNodeShape.hpp"

class GraphEditor;
class GraphEdgeShape;

namespace Animations
{
	class Settings
	{
	public:
		Settings(const Settings&) = delete;
		Settings& operator=(const Settings&) = delete;
		Settings(Settings&&) = delete;
		Settings& operator=(Settings&&) = delete;
		
		static Settings& instance()
		{
			static Settings instance;
			return instance;
		}

		void setTraversalAnimationTime(float newTime) { traversalAnimationTime = newTime; }
		float getTraversalAnimationTime() const { return traversalAnimationTime; }
		
	private:
		float traversalAnimationTime{ 1.f };

		Settings() = default;
		~Settings() = default;
	};
	
	class TraversalOrderAnimation
	{
		bool active{ false };
		bool running{ false };
		GraphAlgorithms::TraversalOrder traversalOrder;
		std::map<int, sf::Color> nodesColors;
		std::vector<std::pair<GraphNodeShape*, sf::Color>> nodesShapesInOrder;
		// bool - true if direction of the animation should be reversed
		std::vector<std::pair<GraphEdgeShape*, bool>> edgesShapesInOrder;

		int nodeShapeIdx{ 0 };
		int edgeShapeIdx{ 0 };
		sf::Clock clock;
		bool loop{ false };

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
	};

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
	};

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
	};
}
