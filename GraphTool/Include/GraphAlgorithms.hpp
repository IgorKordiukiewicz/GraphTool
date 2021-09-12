#pragma once

#include "Graph.hpp"
#include <vector>

namespace GraphAlgorithms
{
	using TraversalOrder = std::vector<int>;
	using VisitedNodes = std::map<int, bool>;
	
	TraversalOrder dfs(const Graph& graph, int startNode);
	TraversalOrder bfs(const Graph& graph, int startNode);
}
