#include "../3rdParty/doctest.h"
#include "../Include/GraphAlgorithms.hpp"

TEST_CASE("Graph Algorithms") {
	Graph graph;
	graph.setNextNodeId(1);
	for (int i{ 0 }; i < 8; ++i) {
		graph.createNode();
	}
	graph.addEdge(1, 2);
	graph.addEdge(2, 3);
	graph.addEdge(3, 4);
	graph.addEdge(2, 5);
	graph.addEdge(5, 6);
	graph.addEdge(7, 8);

	auto checkNodesColorsIdxs = [](const GraphAlgorithms::NodesColorsIdxs& nodesColorsIdxs, int node, int expectedValue) {
		if (auto it{ nodesColorsIdxs.find(node) }; it != nodesColorsIdxs.end()) {
			CHECK(it->second.has_value());
			CHECK(it->second.value() == expectedValue);
		}
	};

	SUBCASE("DFS") {
		const GraphAlgorithms::TraversalOrder traversalOrder{ GraphAlgorithms::dfs(graph, 1) };

		const std::vector<std::pair<int, int>>& edgeOrder{ traversalOrder.edgeOrder };
		CHECK(edgeOrder.size() == 5);
		CHECK(edgeOrder[0].first == 1);
		CHECK(edgeOrder[0].second == 2);
		CHECK(edgeOrder[1].first == 2);
		CHECK(edgeOrder[1].second == 3);
		CHECK(edgeOrder[2].first == 3);
		CHECK(edgeOrder[2].second == 4);
		CHECK(edgeOrder[3].first == 2);
		CHECK(edgeOrder[3].second == 5);
		CHECK(edgeOrder[4].first == 5);
		CHECK(edgeOrder[4].second == 6);

		const std::vector<int>& nodeOrder{ traversalOrder.nodeOrder };
		CHECK(nodeOrder.size() == 6);
		CHECK(nodeOrder[0] == 1);
		CHECK(nodeOrder[1] == 2);
		CHECK(nodeOrder[2] == 3);
		CHECK(nodeOrder[3] == 4);
		CHECK(nodeOrder[4] == 5);
		CHECK(nodeOrder[5] == 6);
	}

	SUBCASE("BFS") {
		const GraphAlgorithms::TraversalOrder traversalOrder{ GraphAlgorithms::bfs(graph, 1) };

		const std::vector<std::pair<int, int>>& edgeOrder{ traversalOrder.edgeOrder };
		CHECK(edgeOrder.size() == 5);
		CHECK(edgeOrder[0].first == 1);
		CHECK(edgeOrder[0].second == 2);
		CHECK(edgeOrder[1].first == 2);
		CHECK(edgeOrder[1].second == 3);
		CHECK(edgeOrder[2].first == 2);
		CHECK(edgeOrder[2].second == 5);
		CHECK(edgeOrder[3].first == 3);
		CHECK(edgeOrder[3].second == 4);
		CHECK(edgeOrder[4].first == 5);
		CHECK(edgeOrder[4].second == 6);

		const std::vector<int>& nodeOrder{ traversalOrder.nodeOrder };
		CHECK(nodeOrder.size() == 6);
		CHECK(nodeOrder[0] == 1);
		CHECK(nodeOrder[1] == 2);
		CHECK(nodeOrder[2] == 3);
		CHECK(nodeOrder[3] == 5);
		CHECK(nodeOrder[4] == 4);
		CHECK(nodeOrder[5] == 6);
	}

	SUBCASE("Dijkstra") {
		const GraphAlgorithms::TraversalOrder traversalOrder{ GraphAlgorithms::dijkstra(graph, 1, 6) };

		const std::vector<std::pair<int, int>>& edgeOrder{ traversalOrder.edgeOrder };
		CHECK(edgeOrder.size() == 4);
		CHECK(edgeOrder[0].first == 1);
		CHECK(edgeOrder[0].second == 1);
		CHECK(edgeOrder[1].first == 1);
		CHECK(edgeOrder[1].second == 2);
		CHECK(edgeOrder[2].first == 2);
		CHECK(edgeOrder[2].second == 5);
		CHECK(edgeOrder[3].first == 5);
		CHECK(edgeOrder[3].second == 6);

		const std::vector<int>& nodeOrder{ traversalOrder.nodeOrder };
		CHECK(nodeOrder.size() == 4);
		CHECK(nodeOrder[0] == 1);
		CHECK(nodeOrder[1] == 2);
		CHECK(nodeOrder[2] == 5);
		CHECK(nodeOrder[3] == 6);
	}

	SUBCASE("Coloring") {
		const auto result { GraphAlgorithms::coloring(graph) };
		const GraphAlgorithms::TraversalOrder traversalOrder{ result.first };
		const GraphAlgorithms::NodesColorsIdxs nodesColorsIdxs{ result.second };

		const std::vector<int>& nodeOrder{ traversalOrder.nodeOrder };
		CHECK(nodeOrder.size() == 8);
		CHECK(nodeOrder[0] == 1);
		CHECK(nodeOrder[1] == 2);
		CHECK(nodeOrder[2] == 3);
		CHECK(nodeOrder[3] == 4);
		CHECK(nodeOrder[4] == 5);
		CHECK(nodeOrder[5] == 6);
		CHECK(nodeOrder[6] == 7);
		CHECK(nodeOrder[7] == 8);

		checkNodesColorsIdxs(nodesColorsIdxs, 1, 0);
		checkNodesColorsIdxs(nodesColorsIdxs, 2, 1);
		checkNodesColorsIdxs(nodesColorsIdxs, 3, 0);
		checkNodesColorsIdxs(nodesColorsIdxs, 4, 1);
		checkNodesColorsIdxs(nodesColorsIdxs, 5, 0);
		checkNodesColorsIdxs(nodesColorsIdxs, 6, 1);
		checkNodesColorsIdxs(nodesColorsIdxs, 7, 0);
		checkNodesColorsIdxs(nodesColorsIdxs, 8, 1);
	}

	SUBCASE("Islands") {
		const auto result{ GraphAlgorithms::findIslands(graph) };
		const GraphAlgorithms::TraversalOrder traversalOrder{ result.first };
		const GraphAlgorithms::NodesColorsIdxs nodesColorsIdxs{ result.second };

		const std::vector<int>& nodeOrder{ traversalOrder.nodeOrder };
		CHECK(nodeOrder.size() == 8);
		CHECK(nodeOrder[0] == 1);
		CHECK(nodeOrder[1] == 2);
		CHECK(nodeOrder[2] == 3);
		CHECK(nodeOrder[3] == 4);
		CHECK(nodeOrder[4] == 5);
		CHECK(nodeOrder[5] == 6);
		CHECK(nodeOrder[6] == 7);
		CHECK(nodeOrder[7] == 8);

		checkNodesColorsIdxs(nodesColorsIdxs, 1, 0);
		checkNodesColorsIdxs(nodesColorsIdxs, 2, 0);
		checkNodesColorsIdxs(nodesColorsIdxs, 3, 0);
		checkNodesColorsIdxs(nodesColorsIdxs, 4, 0);
		checkNodesColorsIdxs(nodesColorsIdxs, 5, 0);
		checkNodesColorsIdxs(nodesColorsIdxs, 6, 0);
		checkNodesColorsIdxs(nodesColorsIdxs, 7, 1);
		checkNodesColorsIdxs(nodesColorsIdxs, 8, 1);
	}

	SUBCASE("Kruskal") {
		const GraphAlgorithms::TraversalOrder traversalOrder{ GraphAlgorithms::kruskalMST(graph) };

		const std::vector<std::pair<int, int>>& edgeOrder{ traversalOrder.edgeOrder };
		CHECK(edgeOrder.size() == 6);
		CHECK(edgeOrder[0].first == 1);
		CHECK(edgeOrder[0].second == 2);
		CHECK(edgeOrder[1].first == 2);
		CHECK(edgeOrder[1].second == 3);
		CHECK(edgeOrder[2].first == 2);
		CHECK(edgeOrder[2].second == 5);
		CHECK(edgeOrder[3].first == 3);
		CHECK(edgeOrder[3].second == 4);
		CHECK(edgeOrder[4].first == 5);
		CHECK(edgeOrder[4].second == 6);
		CHECK(edgeOrder[5].first == 7);
		CHECK(edgeOrder[5].second == 8);

		const std::vector<int>& nodeOrder{ traversalOrder.nodeOrder };
		CHECK(nodeOrder.size() == 8);
		CHECK(nodeOrder[0] == 1);
		CHECK(nodeOrder[1] == 2);
		CHECK(nodeOrder[2] == 3);
		CHECK(nodeOrder[3] == 5);
		CHECK(nodeOrder[4] == 4);
		CHECK(nodeOrder[5] == 6);
		CHECK(nodeOrder[6] == 7);
		CHECK(nodeOrder[7] == 8);
	}

	SUBCASE("Degrees") {
		const auto result{ GraphAlgorithms::nodesDegrees(graph) };
		const GraphAlgorithms::NodesColorsIdxs nodesColorsIdxs{ result.second };

		checkNodesColorsIdxs(nodesColorsIdxs, 1, 0);
		checkNodesColorsIdxs(nodesColorsIdxs, 2, 1);
		checkNodesColorsIdxs(nodesColorsIdxs, 3, 2);
		checkNodesColorsIdxs(nodesColorsIdxs, 4, 0);
		checkNodesColorsIdxs(nodesColorsIdxs, 5, 2);
		checkNodesColorsIdxs(nodesColorsIdxs, 6, 0);
		checkNodesColorsIdxs(nodesColorsIdxs, 7, 0);
		checkNodesColorsIdxs(nodesColorsIdxs, 8, 0);
	}
}