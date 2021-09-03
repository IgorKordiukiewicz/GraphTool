#pragma once

#include <vector>
#include <map>
#include <set>
#include <utility>
#include <iostream>
#include <algorithm>
#include <memory>

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

enum class GraphType
{
	Directed,
	Undirected
};

struct Edge
{
	int a{ 0 };
	int b{ 0 };
	int weight{ 0 };

	Edge(int a, int b, int weight)
		: a(a), b(b), weight(weight) {}

	Edge() {}
};

class Graph
{
public:
	Graph(GraphType type);

	void addEdge(int a, int b, int weight = 0);

	void setEdgeWeight(int a, int b, int newWeight);

	// Creates a new node and returns id of the node
	int createNode();

	void makeDirected();
	void makeUndirected();
	bool isDirected() const { return type == GraphType::Directed; }

	bool doesNodeExist(int nodeId) const;
	bool doesEdgeExist(int a, int b) const;

	void print();

private:
	GraphType type;
	std::map<int, std::set<int>> adjList;
	std::set<Node> nodes;
	std::map<std::pair<int, int>, Edge> edges;
	int nextNodeId{ 1 };
};
