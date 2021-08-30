#pragma once
#include <vector>
#include <map>
#include <set>
#include <utility>
#include <iostream>

class Node
{
public:
	Node(int id) 
		: id(id) {}
	
	const int id{ 0 };
private:
	std::vector<Node*> connectedNodes;
};

inline bool operator<(const Node& lhs, const Node& rhs) { return lhs.id < rhs.id; }
inline bool operator>(const Node& lhs, const Node& rhs) { return rhs < lhs; }
inline bool operator<=(const Node& lhs, const Node& rhs) { return !(lhs < rhs); }
inline bool operator>=(const Node& lhs, const Node& rhs) { return !(rhs < lhs); }
inline bool operator==(const Node& lhs, const Node& rhs) { return lhs.id == rhs.id; }
inline bool operator!=(const Node& lhs, const Node& rhs) { return !(lhs == rhs); }

enum class Directed
{
	Yes,
	No
};

struct Edge
{
	int a{ 0 };
	int b{ 0 };
	int weight{ 0 };

	Edge(int a, int b, int weight)
		: a(a), b(b), weight(weight) {}
};

template<Directed directed>
class Graph
{
public:
	inline void addEdge(int a, int b, int weight = 0)
	{
		// If nodes with id's a or b dont exist, add them to the nodes set
		nodes.insert(Node{ a });
		nodes.insert(Node{ b });

		// Create edge
		Edge edge{ a, b, weight };
		if constexpr (directed == Directed::Yes) {
			edges.emplace(std::make_pair(a, b), edge);
		}
		else {
			edges.emplace(std::make_pair(std::min(a, b), std::max(a, b)), edge);
		}

		// Update adjacency list
		adjList[a].insert(b);
		if constexpr (directed == Directed::No) {
			adjList[b].insert(a);
		}
	}

	inline void setEdgeWeight(int a, int b, int newWeight)
	{
		if constexpr (directed == Directed::Yes) {
			edges[{a, b}].weight = newWeight;
		}
		else {
			edges[{std::min(a, b), std::max(a, b)}].weight = newWeight;
		}
	}

	inline void addNode(int id)
	{
		nodes.insert(Node{ id });
	}

	inline void print()
	{
		for (const auto& [key, values] : adjList) {
			std::cout << key << ": ";
			for (const auto& node : values) {
				//std::cout << node.connectedToId << "(" << node.weight << ") ";
				std::cout << node << " ";
			}
			std::cout << '\n';
		}
	}

private:
	std::map<int, std::set<int>> adjList;
	std::set<Node> nodes;
	std::map<std::pair<int, int>, Edge> edges;
};
