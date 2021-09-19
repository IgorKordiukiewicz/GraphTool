#pragma once

#include <map>
#include <set>
#include <utility>
#include <algorithm>
#include "Event.hpp"

class Node
{
public:
	Node(int id) 
		: id(id) {}
	
	const int id{ 0 };
};

inline bool operator<(const Node& lhs, const Node& rhs) { return lhs.id < rhs.id; }
inline bool operator>(const Node& lhs, const Node& rhs) { return rhs < lhs; }
inline bool operator<=(const Node& lhs, const Node& rhs) { return !(lhs < rhs); }
inline bool operator>=(const Node& lhs, const Node& rhs) { return !(rhs < lhs); }
inline bool operator==(const Node& lhs, const Node& rhs) { return lhs.id == rhs.id; }
inline bool operator!=(const Node& lhs, const Node& rhs) { return !(lhs == rhs); }

struct Edge
{
	int a{ 0 };
	int b{ 0 };
	// mutable so the weight of the edge inside a set can be changed (weight is not involved in comparisons)
	mutable int weight{ 0 };

	Edge(int a, int b, int weight = 0)
		: a(a), b(b), weight(weight) {}

	Edge() {}

	Edge(const Edge&) = default;
	Edge& operator=(const Edge&) = default;
};

inline bool operator<(const Edge& lhs, const Edge& rhs) { return lhs.a < rhs.a || (!(rhs.a < lhs.a) && lhs.b < rhs.b); }
inline bool operator>(const Edge& lhs, const Edge& rhs) { return rhs < lhs; }
inline bool operator<=(const Edge& lhs, const Edge& rhs) { return !(lhs < rhs); }
inline bool operator>=(const Edge& lhs, const Edge& rhs) { return !(rhs < lhs); }
inline bool operator==(const Edge& lhs, const Edge& rhs) { return (lhs.a == rhs.a && lhs.b == rhs.b); }
inline bool operator!=(const Edge& lhs, const Edge& rhs) { return !(lhs == rhs); }

enum class Directed
{
	Yes,
	No
};

enum class Weighted
{
	Yes, 
	No
};

class Graph
{
public:
	Graph();

	using AdjacencyList = std::map<int, std::set<int>>;

	Event<std::vector<std::pair<int, int>>> onDirectedEdgesDeleted;
	Event<std::vector<std::pair<int, int>>> onUndirectedEdgesDeleted;
	Event<> onWeightedValueChanged;
	Event<> onLoadedFromFile;

	void addEdge(int a, int b);

	void setEdgeWeight(int a, int b, int newWeight);
	void setEdgeWeight(int a, int b, int newWeight, Directed directed);
	int getEdgeWeight(int a, int b) const;

	// Creates a new node and returns id of the node
	int createNode();
	void setNextNodeId(int newNextNodeId);

	void deleteNode(int nodeId);
	void deleteEdge(int a, int b);

	void reset();

	void makeDirected();
	void makeUndirected();
	bool isDirected() const { return directed == Directed::Yes; }
	void makeWeighted();
	void makeUnweighted();
	bool isWeighted() const { return weighted == Weighted::Yes; }

	bool doesNodeExist(int nodeId) const;
	bool doesEdgeExist(int a, int b) const;

	const AdjacencyList& getAdjacencyList() const { return isDirected() ? directedAdjList : undirectedAdjList; }
	const std::set<Node>& getNodes() const { return nodes; }
	const std::set<Edge>& getEdges() const { return isDirected() ? directedEdges : undirectedEdges; }
	const std::set<Edge>& getDirectedEdges() const { return directedEdges; }
	const std::set<Edge>& getUndirectedEdges() const { return undirectedEdges; }
	std::vector<std::string> getNodesIdsStrings() const;
	
	bool doesDirectedEdgeExist(int a, int b) const;
	bool doesUndirectedEdgeExist(int a, int b) const;

public: // TEMP
	void print() const;
	void printEdges() const;

private:
	Directed directed{ Directed::No };
	Weighted weighted{ Weighted::No };

	AdjacencyList directedAdjList;
	AdjacencyList undirectedAdjList;

	std::set<Node> nodes;
	int nextNodeId{ 1 };

	// Directed & undirected edges are stored separately so user can easily switch between directed & undirected graph
	std::set<Edge> directedEdges;
	// Undirected edges are stored with .a being the smaller node id and .b being the bigger node id
	std::set<Edge> undirectedEdges;
};
