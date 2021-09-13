#pragma once

#include "Graph.hpp"
#include <vector>
#include <optional>

namespace GraphAlgorithms
{
	struct TraversalOrder
	{
		std::vector<std::pair<int, int>> edgeOrder;
		std::vector<int> nodeOrder;
	};

	using NodesColorsIdxs = std::map<int, std::optional<int>>;
	
	TraversalOrder dfs(const Graph& graph, int startNode);
	TraversalOrder bfs(const Graph& graph, int startNode);
	TraversalOrder dijkstra(const Graph& graph, int startNode, int endNode);
	std::pair<TraversalOrder, NodesColorsIdxs> coloring(const Graph& graph);
}
