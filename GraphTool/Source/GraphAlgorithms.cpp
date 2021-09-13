#include "../Include/GraphAlgorithms.hpp"
#include <queue>
#include <optional>
#include <iostream>

namespace GraphAlgorithms
{
	using VisitedNodes = std::map<int, bool>;
	
	namespace Helpers
	{
		VisitedNodes createEmptyVisitedNodesContainer(const Graph& graph)
		{
			VisitedNodes visitedNodes;
			for (const auto& node : graph.getNodes()) {
				visitedNodes.emplace(node.id, false);
			}
			return visitedNodes;
		}
	}
	
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

		void dijkstraImpl(const Graph& graph, int startNode, int endNode, TraversalOrder& traversalOrder)
		{
			std::map<int, std::optional<int>> distances;
			std::map<int, std::vector<int>> paths;
			// pair - distance, node
			std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> queue;
			queue.push({ 0, startNode });
			distances.emplace(startNode, 0);
			
			while (!queue.empty()) {
				const int currentNode = queue.top().second;
				const int currentDistance = queue.top().first;
				queue.pop();

				if (const auto it = graph.getAdjacencyList().find(currentNode); it != graph.getAdjacencyList().end()) {
					for (const auto node : it->second) {
						const int distance = graph.getEdgeWeight(currentNode, node);
						if ((distances[node].has_value() && currentDistance + distance < distances[node]) || !distances[node].has_value()) {
							distances[node] = currentDistance + distance;
							queue.push({ *distances[node], node });

							paths[node] = paths[currentNode];
							paths[node].push_back(currentNode);
						}
					}
				}
			}

			if (distances[endNode].has_value()) {
				int lastNode = startNode;
				for (const auto node : paths[endNode]) {
					traversalOrder.nodeOrder.push_back(node);
					traversalOrder.edgeOrder.push_back({ lastNode, node });
					lastNode = node;
				}
				traversalOrder.nodeOrder.push_back(endNode);
				traversalOrder.edgeOrder.push_back({ lastNode, endNode });
			}
		}
	}
	
	TraversalOrder dfs(const Graph& graph, int startNode)
	{
		TraversalOrder traversalOrder;
		VisitedNodes visitedNodes = Helpers::createEmptyVisitedNodesContainer(graph);
		
		Impl::dfsImpl(graph, startNode, traversalOrder, visitedNodes);

		return traversalOrder;
	}

	TraversalOrder bfs(const Graph& graph, int startNode)
	{
		TraversalOrder traversalOrder;
		VisitedNodes visitedNodes = Helpers::createEmptyVisitedNodesContainer(graph);

		Impl::bfsImpl(graph, startNode, traversalOrder, visitedNodes);

		return traversalOrder;
	}

	TraversalOrder dijkstra(const Graph& graph, int startNode, int endNode)
	{
		TraversalOrder traversalOrder;

		Impl::dijkstraImpl(graph, startNode, endNode, traversalOrder);

		return traversalOrder;
	}
}
