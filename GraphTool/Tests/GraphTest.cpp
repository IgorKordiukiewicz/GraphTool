#include "../3rdParty/doctest.h"
#include "../Include/Graph.hpp"

TEST_CASE("Node") {
	Node node1{ 1 };
	Node node2{ 2 };
	Node node3{ 1 };
	CHECK(node1 < node2);
	CHECK_FALSE(node1 > node2);
	CHECK(node1 == node3);
}

TEST_CASE("Edge") {
	Edge edge1{ 1, 2 };
	Edge edge2{ 1, 3 };
	Edge edge3{ 2, 1 };
	Edge edge4{ 1, 2, 5 };
	CHECK(edge1 < edge2);
	CHECK(edge2 < edge3);
	CHECK_FALSE(edge1 > edge3);
	CHECK(edge1 == edge4);
}

TEST_CASE("Graph") {
	Graph graph;
	const int nodeA{ graph.createNode() };
	const int nodeB{ graph.createNode() };
	graph.addEdge(nodeA, nodeB);

	SUBCASE("Add/delete edge") {
		graph.addEdge(nodeA, nodeB);
		CHECK(graph.doesEdgeExist(nodeA, nodeB));
		graph.deleteEdge(nodeA, nodeB);
		CHECK_FALSE(graph.doesEdgeExist(nodeA, nodeB));
	}

	SUBCASE("Add/delete node") {
		const int nodeId{ graph.createNode() };
		CHECK(graph.doesNodeExist(nodeId));
		graph.deleteNode(nodeId);
		CHECK_FALSE(graph.doesNodeExist(nodeId));
	}

	SUBCASE("Deleting node should delete connected to it edges") {
		graph.deleteNode(nodeA);
		CHECK_FALSE(graph.doesNodeExist(nodeA));
		CHECK_FALSE(graph.doesEdgeExist(nodeA, nodeB));
	}

	SUBCASE("Directed/undirected edges") {
		CHECK(graph.doesEdgeExist(nodeA, nodeB));
		CHECK(graph.doesEdgeExist(nodeB, nodeA));
		graph.makeDirected();
		CHECK(graph.doesEdgeExist(nodeA, nodeB));
		CHECK_FALSE(graph.doesEdgeExist(nodeB, nodeA));
		graph.addEdge(nodeB, nodeA);
		CHECK(graph.doesEdgeExist(nodeB, nodeA));
	}

	SUBCASE("Edges/nodes container size") {
		CHECK(graph.getNodes().size() == 2);
		CHECK(graph.getEdges().size() == 1);
		graph.addEdge(nodeB, nodeA);
		CHECK(graph.getEdges().size() == 1);
		graph.makeDirected();
		CHECK(graph.getEdges().size() == 2);
	}

	SUBCASE("Modify next node id") {
		graph.setNextNodeId(5);
		const int nodeId{ graph.createNode() };
		CHECK(nodeId == 5);
	}

	SUBCASE("Edge weight") {
		graph.addEdge(nodeA, nodeB);
		graph.setEdgeWeight(nodeA, nodeB, 5);
		CHECK(graph.getEdgeWeight(nodeA, nodeB) == 1); // not weighted
		graph.makeWeighted();
		CHECK(graph.getEdgeWeight(nodeA, nodeB) == 5);
	}

	SUBCASE("Reset") {
		graph.reset();
		CHECK_FALSE(graph.isDirected());
		CHECK_FALSE(graph.isWeighted());
		CHECK(graph.getNodes().empty());
		CHECK(graph.getDirectedEdges().empty());
		CHECK(graph.getUndirectedEdges().empty());
		CHECK(graph.getAdjacencyList().empty());
		graph.makeDirected();
		CHECK(graph.getAdjacencyList().empty());
	}
}