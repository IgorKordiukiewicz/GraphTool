#include "../Include/GraphAlgorithms.hpp"
#include <queue>

namespace GraphAlgorithms
{
	namespace Impl
	{
		void dfsImpl(const Graph& graph, int currentNode, TraversalOrder& traversalOrder, VisitedNodes& visitedNodes, int parentNode = 0)
		{
			visitedNodes[currentNode] = true;
			if (parentNode != 0) {
				traversalOrder.edgeOrder.push_back({ parentNode, currentNode });
			}
			traversalOrder.nodeOrder.push_back(currentNode);

			if (const auto it = graph.getAdjacencyList().find(currentNode); it != graph.getAdjacencyList().end()) {
				for (const auto node : it->second) {
					if (!visitedNodes[node]) {
						dfsImpl(graph, node, traversalOrder, visitedNodes, currentNode);
					}
				}
			}
		}

		void bfsImpl(const Graph& graph, int currentNode, TraversalOrder& traversalOrder, VisitedNodes& visitedNodes)
		{
			visitedNodes[currentNode] = true;
			std::queue<int> queue;
			queue.push(currentNode);

			while (!queue.empty()) {
				currentNode = queue.front();
				traversalOrder.nodeOrder.push_back(currentNode);
				queue.pop();

				if (const auto it = graph.getAdjacencyList().find(currentNode); it != graph.getAdjacencyList().end()) {
					for (const auto node : it->second) {
						if (!visitedNodes[node]) {
							traversalOrder.edgeOrder.push_back({ currentNode, node });
							visitedNodes[node] = true;
							queue.push(node);
						}
					}
				}
				
			}
		}
	}
	
	TraversalOrder dfs(const Graph& graph, int startNode)
	{
		TraversalOrder traversalOrder;
		VisitedNodes visitedNodes;
		for (const auto& node : graph.getNodes()) {
			visitedNodes.emplace(node.id, false);
		}

		Impl::dfsImpl(graph, startNode, traversalOrder, visitedNodes);

		return traversalOrder;
	}

	TraversalOrder bfs(const Graph& graph, int startNode)
	{
		TraversalOrder traversalOrder;
		VisitedNodes visitedNodes;
		for (const auto& node : graph.getNodes()) {
			visitedNodes.emplace(node.id, false);
		}

		Impl::bfsImpl(graph, startNode, traversalOrder, visitedNodes);

		return traversalOrder;
	}
}
