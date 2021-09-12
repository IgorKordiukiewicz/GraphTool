#pragma once

#include "Graph.hpp"
#include <vector>
#include <string>

class AlgorithmsPanel
{
public:
	AlgorithmsPanel(Graph& graph);

	void operator()();

private:
	void selectNode(const std::vector<std::string>& nodesIds, int& outNodeIdx);

private:
	Graph& graph;

	enum class SelectedAlgorithm
	{
		None,
		DFS,
		BFS
	} selectedAlgorithm{ SelectedAlgorithm::None };
	int startNode = 0;
};
