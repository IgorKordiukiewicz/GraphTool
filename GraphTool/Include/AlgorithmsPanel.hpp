#pragma once

#include "GraphEditor.hpp"
#include <vector>
#include <string>

class AlgorithmsPanel
{
public:
	AlgorithmsPanel(Graph& graph, GraphEditor& graphEditor);

	void run();

private:
	void selectNode(const std::vector<std::string>& nodesIds, int& outNodeIdx, const char* label = "##SelectStartNodeCombo");
	void showDfsOptions();
	void showBfsOptions();
	void showDijkstraOptions();
	void showColoringOptions();
	void showFindIslandsOptions();

private:
	Graph& graph;
	GraphEditor& graphEditor;

	enum class SelectedAlgorithm
	{
		None,
		DFS,
		BFS,
		Dijkstra,
		Coloring,
		FindIslands
	} selectedAlgorithm{ SelectedAlgorithm::None };
	int selectedAlgorithmIdx{ 0 };

	int startNode{ 0 };
	int endNode{ 0 };
};
