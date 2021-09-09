#include "../Include/Graph.hpp"
#include <iostream>

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
	directedEdges.insert(Edge{ a, b, weight });
	undirectedEdges.insert(Edge{ std::min(a, b), std::max(a, b), weight });

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
	
	if (doesDirectedEdgeExist(a, b)) {
		directedEdges.find(Edge{ a, b })->weight = newWeight;
	}
	if (doesUndirectedEdgeExist(a, b)) {
		undirectedEdges.find(Edge{ std::min(a, b), std::max(a, b) })->weight = newWeight;
	}
}

int Graph::createNode()
{
	Node node{ nextNodeId++ };
	nodes.insert(std::move(node));
	return nextNodeId - 1;
}

void Graph::deleteNode(int nodeId)
{
	if (!doesNodeExist(nodeId)) {
		return;
	}

	nodes.erase(nodeId);

	adjList.erase(nodeId);
	for (auto& [id, connectedIds] : adjList) {
		connectedIds.erase(nodeId);
	}

	auto updateEdges = [nodeId](std::set<Edge>& edges) {
		auto it = edges.begin();
		while (it != edges.end()) {
			if (it->a == nodeId || it->b == nodeId) {
				it = edges.erase(it);
			}
			else {
				++it;
			}
		}
	};
	updateEdges(directedEdges);
	updateEdges(undirectedEdges);	
}

void Graph::deleteEdge(int a, int b)
{
	if (!doesNodeExist(a) || !doesNodeExist(b)) {
		return;
	}

	// TODO
}

void Graph::makeDirected()
{
	if (isDirected()) {
		return;
	}

	type = GraphType::Directed;
	onGraphTypeChanged.emit();
}

void Graph::makeUndirected()
{
	if (!isDirected()) {
		return;
	}

	type = GraphType::Undirected;
	onGraphTypeChanged.emit();
}

bool Graph::doesNodeExist(int nodeId) const
{
	return nodes.find(nodeId) != nodes.end();
}

bool Graph::doesEdgeExist(int a, int b) const
{
	return isDirected() ? doesDirectedEdgeExist(a, b) : doesUndirectedEdgeExist(a, b);
}

bool Graph::doesDirectedEdgeExist(int a, int b) const
{
	return directedEdges.find({ a, b }) != directedEdges.end();
}

bool Graph::doesUndirectedEdgeExist(int a, int b) const
{
	return undirectedEdges.find({ std::min(a, b), std::max(a, b) }) != undirectedEdges.end();
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

void Graph::printEdges()
{
	if (isDirected()) {
		for (const auto& edge : directedEdges) {
			std::cout << edge.a << " " << edge.b << "\n";
		}
	}
	else {
		for (const auto& edge : undirectedEdges) {
			std::cout << edge.a << " " << edge.b << "\n";
		}
	}
}
