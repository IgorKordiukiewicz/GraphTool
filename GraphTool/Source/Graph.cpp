#include "../Include/Graph.hpp"

Graph::Graph(GraphType type)
	: type(type)
{

}

void Graph::addEdge(int a, int b, int weight)
{
	// If nodes with id's a or b dont exist, return
	if (!doesNodeExist(a) || !doesNodeExist(b)) {
		return;
	}

	// Create edge
	Edge edge{ a, b, weight };
	if (isDirected()) {
		edges.emplace(std::make_pair(a, b), edge);
	}
	else {
		edges.emplace(std::make_pair(std::min(a, b), std::max(a, b)), edge);
	}

	// Update adjacency list
	adjList[a].insert(b);
	if (!isDirected()) {
		adjList[b].insert(a);
	}
}

void Graph::setEdgeWeight(int a, int b, int newWeight)
{
	if (!doesNodeExist(a) || !doesNodeExist(b)) {
		return;
	}
	
	if (isDirected()) {
		edges[{a, b}].weight = newWeight;
	}
	else {
		edges[{std::min(a, b), std::max(a, b)}].weight = newWeight;
	}
}

int Graph::createNode()
{
	Node node{ nextNodeId++ };
	nodes.insert(std::move(node));
	return nextNodeId - 1;
}

void Graph::makeDirected()
{
	if (isDirected()) {
		return;
	}

	type = GraphType::Directed;
}

void Graph::makeUndirected()
{
	if (!isDirected()) {
		return;
	}

	type = GraphType::Undirected;
}

bool Graph::doesNodeExist(int nodeId) const
{
	return nodes.find(nodeId) != nodes.end();
}

bool Graph::doesEdgeExist(int a, int b) const
{
	if (type == GraphType::Directed) {
		return edges.find({ a, b }) != edges.end();
	}
	else {
		return edges.find({ std::min(a, b), std::max(a, b) }) != edges.end();
	}
}

void Graph::print()
{
	for (const auto& [key, values] : adjList) {
		std::cout << key << ": ";
		for (const auto& node : values) {
			std::cout << node << " ";
		}
		std::cout << '\n';
	}
}