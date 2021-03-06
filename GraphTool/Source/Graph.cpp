#include "../Include/Graph.hpp"
#include <iostream>
#include <string>

Graph::Graph()
{

}

void Graph::addEdge(int a, int b)
{
	if (!doesNodeExist(a) || !doesNodeExist(b)) {
		return;
	}

	// Create edge
	directedEdges.insert(Edge{ a, b, 0 });
	undirectedEdges.insert(Edge{ std::min(a, b), std::max(a, b), 0 });

	// Update adjacency lists
	directedAdjList[a].insert(b);
	undirectedAdjList[a].insert(b);
	undirectedAdjList[b].insert(a);
}

void Graph::setEdgeWeight(int a, int b, int newWeight)
{
	setEdgeWeight(a, b, newWeight, directed);
}

void Graph::setEdgeWeight(int a, int b, int newWeight, Directed directed)
{
	if (!doesNodeExist(a) || !doesNodeExist(b)) {
		return;
	}

	if (doesDirectedEdgeExist(a, b) && directed == Directed::Yes) {
		directedEdges.find(Edge{ a, b })->weight = newWeight;
	}
	else if (doesUndirectedEdgeExist(a, b) && directed == Directed::No) {
		undirectedEdges.find(Edge{ std::min(a, b), std::max(a, b) })->weight = newWeight;
	}
}

int Graph::getEdgeWeight(int a, int b) const
{
	if (!doesEdgeExist(a, b)) {
		return 0;
	}

	if (isWeighted()) {
		if (isDirected()) {
			return directedEdges.find({ a, b })->weight;
		}
		else {
			return undirectedEdges.find({ std::min(a, b), std::max(a, b) })->weight;
		}
	}
	else {
		return 1;
	}
}

int Graph::createNode()
{
	Node node{ nextNodeId++ };
	nodes.insert(std::move(node));
	return nextNodeId - 1;
}

void Graph::setNextNodeId(int newNextNodeId)
{
	nextNodeId = newNextNodeId;
}

void Graph::deleteNode(int nodeId)
{
	if (!doesNodeExist(nodeId)) {
		return;
	}

	nodes.erase(nodeId);

	auto updateAdjList = [nodeId](auto& adjList) {
		adjList.erase(nodeId);
		for (auto& [id, connectedIds] : adjList) {
			connectedIds.erase(nodeId);
		}
	};
	updateAdjList(directedAdjList);
	updateAdjList(undirectedAdjList);

	std::vector<std::pair<int, int>> deletedDirectedEdges;
	std::vector<std::pair<int, int>> deletedUndirectedEdges;

	auto deleteEdges = [nodeId](auto& edges, auto& deletedEdges) {
		auto it{ edges.begin() };
		while (it != edges.end()) {
			if (it->a == nodeId || it->b == nodeId) {
				deletedEdges.push_back({ it->a, it->b });
				it = edges.erase(it);
			}
			else {
				++it;
			}
		}
	};
	deleteEdges(directedEdges, deletedDirectedEdges);
	deleteEdges(undirectedEdges, deletedUndirectedEdges);

	onDirectedEdgesDeleted.emit(std::move(deletedDirectedEdges));
	onUndirectedEdgesDeleted.emit(std::move(deletedUndirectedEdges));
}

void Graph::deleteEdge(int a, int b)
{
	if (!doesNodeExist(a) || !doesNodeExist(b)) {
		return;
	}

	auto updateAdjList = [a, b](auto& adjList) {
		for (auto& [id, connectedIds] : adjList) {
			if (id == a || id == b) {
				auto it{ connectedIds.begin() };
				while (it != connectedIds.end()) {
					if ((id == a && *it == b) || (id == b && *it == a)) {
						it = connectedIds.erase(it);
					}
					else {
						++it;
					}
				}
			}
		}
	};

	auto deleteEdges = [a, b](auto& edges, auto& deletedEdges) {
		auto it{ edges.begin() };
		while (it != edges.end()) {
			if ((it->a == a && it->b == b) || (it->a == b && it->b == a)) {
				deletedEdges.push_back({ it->a, it->b });
				it = edges.erase(it);
			}
			else {
				++it;
			}
		}
	};

	std::vector<std::pair<int, int>> deletedDirectedEdges;
	std::vector<std::pair<int, int>> deletedUndirectedEdges;

	if (isDirected()) {
		// If there exists another edge connecting the A and B nodes, don't delete the undirected edge connecting them
		if (doesDirectedEdgeExist(b, a)) {
			for (auto& [id, connectedIds] : directedAdjList) {
				if (id == a) {
					auto it{ connectedIds.begin() };
					while (it != connectedIds.end()) {
						if (*it == b) {
							it = connectedIds.erase(it);
						}
						else {
							++it;
						}
					}
				}
			}

			auto it{ directedEdges.begin() };
			while (it != directedEdges.end()) {
				if (it->a == a && it->b == b) {
					deletedDirectedEdges.push_back({ a, b });
					it = directedEdges.erase(it);
				}
				else {
					++it;
				}
			}
		}
		else {
			updateAdjList(directedAdjList);
			updateAdjList(undirectedAdjList);

			deleteEdges(directedEdges, deletedDirectedEdges);
			deleteEdges(undirectedEdges, deletedUndirectedEdges);
		}
	}
	// If graph is undirected, delete all directed edges connecting the A and B nodes
	else {
		updateAdjList(directedAdjList);
		updateAdjList(undirectedAdjList);

		deleteEdges(directedEdges, deletedDirectedEdges);
		deleteEdges(undirectedEdges, deletedUndirectedEdges);
	}

	onDirectedEdgesDeleted.emit(std::move(deletedDirectedEdges));
	onUndirectedEdgesDeleted.emit(std::move(deletedUndirectedEdges));
}

void Graph::reset()
{
	directed = Directed::No;
	weighted = Weighted::No;
	nextNodeId = 1;
	nodes.clear();
	directedAdjList.clear();
	undirectedAdjList.clear();
	directedEdges.clear();
	undirectedEdges.clear();
}

void Graph::makeDirected()
{
	directed = Directed::Yes;
}

void Graph::makeUndirected()
{
	directed = Directed::No;
}

void Graph::makeWeighted()
{
	if (isWeighted()) {
		return;
	}

	weighted = Weighted::Yes;
	onWeightedValueChanged.emit();
}

void Graph::makeUnweighted()
{
	if (!isWeighted()) {
		return;
	}

	weighted = Weighted::No;
	onWeightedValueChanged.emit();
}

bool Graph::doesNodeExist(int nodeId) const
{
	return nodes.find(nodeId) != nodes.end();
}

bool Graph::doesEdgeExist(int a, int b) const
{
	return isDirected() ? doesDirectedEdgeExist(a, b) : doesUndirectedEdgeExist(a, b);
}

std::vector<std::string> Graph::getNodesIdsStrings() const
{
	std::vector<std::string> result;
	for (const Node& node : nodes) {
		result.push_back(std::to_string(node.id));
	}
	return result;
}

bool Graph::doesDirectedEdgeExist(int a, int b) const
{
	return directedEdges.find({ a, b }) != directedEdges.end();
}

bool Graph::doesUndirectedEdgeExist(int a, int b) const
{
	return undirectedEdges.find({ std::min(a, b), std::max(a, b) }) != undirectedEdges.end();
}
