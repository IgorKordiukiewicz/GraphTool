#pragma once

#include "Graph.hpp"
#include <vector>

namespace GraphAlgorithms
{
	//using TraversalOrder = std::vector<std::pair<int, int>>;
	struct TraversalOrder
	{
		std::vector<std::pair<int, int>> edgeOrder;
		std::vector<int> nodeOrder;
	};
	using VisitedNodes = std::map<int, bool>;
	
	TraversalOrder dfs(const Graph& graph, int startNode);
	TraversalOrder bfs(const Graph& graph, int startNode);
}
