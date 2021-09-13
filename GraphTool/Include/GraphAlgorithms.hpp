#pragma once

#include "Graph.hpp"
#include <vector>

namespace GraphAlgorithms
{
	struct TraversalOrder
	{
		std::vector<std::pair<int, int>> edgeOrder;
		std::vector<int> nodeOrder;
	};
	
	TraversalOrder dfs(const Graph& graph, int startNode);
	TraversalOrder bfs(const Graph& graph, int startNode);
	TraversalOrder dijkstra(const Graph& graph, int startNode, int endNode);
}
