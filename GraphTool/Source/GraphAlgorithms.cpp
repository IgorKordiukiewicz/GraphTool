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

		void coloringImpl(const Graph& graph, TraversalOrder& traversalOrder, NodesColorsIdxs& nodesColorsIdxs)
		{
			std::map<int, bool> availableColorsIdxs;

			for (const auto& node : graph.getNodes()) {
				nodesColorsIdxs[node.id] = 0;
				traversalOrder.nodeOrder.push_back(node.id);
				break;
			}

			for (int i = 0; i < graph.getNodes().size(); ++i) {
				availableColorsIdxs.emplace(i, false);
			}

			bool firstLoop{ true };
			for (const auto& node : graph.getNodes()) {
				if (firstLoop) {
					firstLoop = false;
					continue;
				}

				if (const auto it = graph.getAdjacencyList().find(node.id); it != graph.getAdjacencyList().end()) {
					for (const auto nodeId : it->second) {
						if (nodesColorsIdxs[nodeId].has_value()) {
							availableColorsIdxs[*nodesColorsIdxs[nodeId]] = true;
						}
					}

					int availableColorIdx = 0;
					for (availableColorIdx; availableColorIdx < graph.getNodes().size(); ++availableColorIdx) {
						if (!availableColorsIdxs[availableColorIdx]) {
							break;
						}
					}

					nodesColorsIdxs[node.id] = availableColorIdx;
					traversalOrder.nodeOrder.push_back(node.id);

					for (const auto nodeId : it->second) {
						if (nodesColorsIdxs[nodeId].has_value()) {
							availableColorsIdxs[*nodesColorsIdxs[nodeId]] = false;
						}
					}
				}
			}
		}

		void kruskalMSTImpl(const Graph& graph, TraversalOrder& traversalOrder, VisitedNodes& visitedNodes)
		{
			std::vector<int> parent;
			for (int i = 0; i < graph.getNodes().size() + graph.getEdges().size(); ++i) {
				parent.push_back(i);
			}

			const auto edgesSortedByWeight = [&graph]() {
				std::vector<Edge> edges;
				for (const auto& edge : graph.getEdges()) {
					edges.push_back(edge);
				}
				std::sort(edges.begin(), edges.end(), [](const auto& edge1, const auto& edge2) {
					if (edge1.weight < edge2.weight) {
						return true;
					}
					else if (edge1.weight > edge2.weight) {
						return false;
					}
					else {
						return edge1 < edge2;
					}
					});
				return edges;
			}();

			auto getRoot = [&parent](int nodeId) {
				while (parent[nodeId] != nodeId) {
					parent[nodeId] = parent[parent[nodeId]];
					nodeId = parent[nodeId];
				}
				return nodeId;
			};

			auto join = [&parent, &getRoot](int a, int b) {
				const int rootA = getRoot(a);
				const int rootB = getRoot(b);
				parent[rootA] = rootB;
			};

			for (int i = 0; i < edgesSortedByWeight.size(); ++i) {
				const int a = edgesSortedByWeight[i].a;
				const int b = edgesSortedByWeight[i].b;
				if (getRoot(a) != getRoot(b)) {
					join(a, b);

					if (!visitedNodes[a]) {
						traversalOrder.nodeOrder.push_back(a);
						visitedNodes[a] = true;
					}
					if (!visitedNodes[b]) {
						traversalOrder.nodeOrder.push_back(b);
						visitedNodes[b] = true;
					}

					traversalOrder.edgeOrder.push_back({ a, b });
				}
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

	std::pair<TraversalOrder, NodesColorsIdxs> coloring(const Graph& graph)
	{
		TraversalOrder traversalOrder;
		NodesColorsIdxs nodesColorsIdxs;
		
		Impl::coloringImpl(graph, traversalOrder, nodesColorsIdxs);

		return { traversalOrder, nodesColorsIdxs };
	}

	std::pair<TraversalOrder, NodesColorsIdxs> findIslands(const Graph& graph)
	{
		TraversalOrder traversalOrder;
		NodesColorsIdxs nodesColorsIdxs;
		VisitedNodes visitedNodes = Helpers::createEmptyVisitedNodesContainer(graph);

		int colorIdx{ 0 };
		for (const auto [nodeId, visited] : visitedNodes) {
			if (!visited) {
				TraversalOrder tempTraversalOrder;
				Impl::dfsImpl(graph, nodeId, tempTraversalOrder, visitedNodes);

				for (const auto nodeId : tempTraversalOrder.nodeOrder) {
					nodesColorsIdxs[nodeId] = colorIdx;
				}
				++colorIdx;
				traversalOrder += std::move(tempTraversalOrder);
			}
		}

		traversalOrder.edgeOrder.clear();
		return { traversalOrder, nodesColorsIdxs };
	}

	TraversalOrder kruskalMST(const Graph& graph)
	{
		TraversalOrder traversalOrder;
		VisitedNodes visitedNodes = Helpers::createEmptyVisitedNodesContainer(graph);

		Impl::kruskalMSTImpl(graph, traversalOrder, visitedNodes);

		return traversalOrder;
	}

	TraversalOrder& TraversalOrder::operator+=(const TraversalOrder& other)
	{
		std::copy(other.edgeOrder.begin(), other.edgeOrder.end(), std::back_inserter(edgeOrder));
		std::copy(other.nodeOrder.begin(), other.nodeOrder.end(), std::back_inserter(nodeOrder));
		return *this;
	}

	TraversalOrder& TraversalOrder::operator+=(TraversalOrder&& other)
	{
		std::move(other.edgeOrder.begin(), other.edgeOrder.end(), std::back_inserter(edgeOrder));
		std::move(other.nodeOrder.begin(), other.nodeOrder.end(), std::back_inserter(nodeOrder));
		return *this;
	}
}
