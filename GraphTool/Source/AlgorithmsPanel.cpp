#include "../Include/AlgorithmsPanel.hpp"
#include "../Include/GraphAlgorithms.hpp"
#include <imgui.h>
#include <iostream>
#include <string>

namespace ga = GraphAlgorithms;

AlgorithmsPanel::AlgorithmsPanel(Graph& graph, GraphEditor& graphEditor)
	: graph(graph), graphEditor(graphEditor)
{
}

void AlgorithmsPanel::run()
{
	ImGui::Text("Select Algorithm");
	ImGui::Spacing();
	const int oldSelectedAlgorithmIdx = selectedAlgorithmIdx;

	if (ImGui::Selectable("Depth First Search", selectedAlgorithmIdx == 1)) {
		selectedAlgorithm = SelectedAlgorithm::DFS;
		selectedAlgorithmIdx = 1;
	}
	if (ImGui::Selectable("Breadth First Search", selectedAlgorithmIdx == 2)) {
		selectedAlgorithm = SelectedAlgorithm::BFS;
		selectedAlgorithmIdx = 2;
	}
	if (ImGui::Selectable("Dijkstra's Shortest Path", selectedAlgorithmIdx == 3)) {
		selectedAlgorithm = SelectedAlgorithm::Dijkstra;
		selectedAlgorithmIdx = 3;
	}
	if (ImGui::Selectable("Graph Coloring", selectedAlgorithmIdx == 4)) {
		selectedAlgorithm = SelectedAlgorithm::Coloring;
		selectedAlgorithmIdx = 4;
	}
	if (ImGui::Selectable("Find Islands", selectedAlgorithmIdx == 5)) {
		selectedAlgorithm = SelectedAlgorithm::FindIslands;
		selectedAlgorithmIdx = 5;
	}

	if (selectedAlgorithmIdx != oldSelectedAlgorithmIdx) {
		graphEditor.deactivateTraversalOrderAnimation();
	}

	ImGui::Separator();
	switch (selectedAlgorithm) {
	case SelectedAlgorithm::DFS:
		showDfsOptions();
		break;
	case SelectedAlgorithm::BFS:
		showBfsOptions();
		break;
	case SelectedAlgorithm::Dijkstra:
		showDijkstraOptions();
		break;
	case SelectedAlgorithm::Coloring:
		showColoringOptions();
		break;
	case SelectedAlgorithm::FindIslands:
		showFindIslandsOptions();
		break;
	}
}

void AlgorithmsPanel::selectNode(const std::vector<std::string>& nodesIds, int& outNodeIdx, const char* label)
{
	const char* comboLabel = nodesIds.empty() ? "" : nodesIds[outNodeIdx].c_str();

	if (ImGui::BeginCombo(label, comboLabel)) {
		for (int i = 0; i < nodesIds.size(); ++i) {
			bool isSelected = (outNodeIdx == i);
			if (ImGui::Selectable(nodesIds[i].c_str(), isSelected)) {
				outNodeIdx = i;
			}

			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}
}

void AlgorithmsPanel::showDfsOptions()
{
	ImGui::Text("Depth First Search");
	const auto nodesIds = graph.getNodesIdsStrings();
	selectNode(nodesIds, startNode);

	if (ImGui::Button("Execute")) {
		const auto traversalOrder = ga::dfs(graph, std::stoi(nodesIds[startNode]));
		graphEditor.activateTraversalOrderAnimation(traversalOrder);
	}
}

void AlgorithmsPanel::showBfsOptions()
{
	ImGui::Text("Breadth First Search");
	const auto nodesIds = graph.getNodesIdsStrings();
	selectNode(nodesIds, startNode);

	if (ImGui::Button("Execute")) {
		const auto traversalOrder = ga::bfs(graph, std::stoi(nodesIds[startNode]));
		graphEditor.activateTraversalOrderAnimation(traversalOrder);
	}
}

void AlgorithmsPanel::showDijkstraOptions()
{
	ImGui::Text("Dijkstra's Shortest Path");
	const auto nodesIds = graph.getNodesIdsStrings();
	selectNode(nodesIds, startNode);
	selectNode(nodesIds, endNode, "##SelectEndNodeLabel");

	if (ImGui::Button("Execute")) {
		const auto traversalOrder = ga::dijkstra(graph, std::stoi(nodesIds[startNode]), std::stoi(nodesIds[endNode]));
		graphEditor.activateTraversalOrderAnimation(traversalOrder);
	}
}

void AlgorithmsPanel::showColoringOptions()
{
	ImGui::Text("Graph Coloring");

	if (ImGui::Button("Execute")) {
		const auto [traversalOrder, nodesColorsIdxs] = ga::coloring(graph);
		graphEditor.activateTraversalOrderAnimation(traversalOrder, nodesColorsIdxs);
	}
}

void AlgorithmsPanel::showFindIslandsOptions()
{
	ImGui::Text("Find Islands");

	if (ImGui::Button("Execute")) {
		const auto [traversalOrder, nodesColorsIdxs] = ga::findIslands(graph);
		graphEditor.activateTraversalOrderAnimation(traversalOrder, nodesColorsIdxs);
	}
}
