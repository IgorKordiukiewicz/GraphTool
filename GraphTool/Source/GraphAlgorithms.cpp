#include "../Include/GraphAlgorithms.hpp"
#include <queue>

namespace GraphAlgorithms
{
	namespace Impl
	{
		void dfsImpl(const Graph& graph, int currentNode, TraversalOrder& traversalOrder, VisitedNodes& visitedNodes)
		{
			visitedNodes[currentNode] = true;
			traversalOrder.push_back(currentNode);

			const auto& connectedNodes = graph.getAdjacencyList().find(currentNode)->second;
			for (const auto node : connectedNodes) {
				if (!visitedNodes[node]) {
					dfsImpl(graph, node, traversalOrder, visitedNodes);
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
				traversalOrder.push_back(currentNode);
				queue.pop();

				const auto& connectedNodes = graph.getAdjacencyList().find(currentNode)->second;
				for (const auto node : connectedNodes) {
					if (!visitedNodes[node]) {
						visitedNodes[node] = true;
						queue.push(node);
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
