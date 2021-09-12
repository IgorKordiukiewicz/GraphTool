#include "../Include/AlgorithmsPanel.hpp"
#include "../Include/GraphAlgorithms.hpp"
#include <imgui.h>
#include <iostream>
#include <string>

namespace ga = GraphAlgorithms;

AlgorithmsPanel::AlgorithmsPanel(Graph& graph)
	: graph(graph)
{
}

void AlgorithmsPanel::operator()()
{
	ImGui::Text("Select Algorithm");
	if (ImGui::Button("Depth First Search")) {
		selectedAlgorithm = SelectedAlgorithm::DFS;
	}
	if (ImGui::Button("Breadth First Search")) {
		selectedAlgorithm = SelectedAlgorithm::BFS;
	}

	ImGui::Separator();
	if (selectedAlgorithm == SelectedAlgorithm::DFS) {
		const auto nodesIds = graph.getNodesIdsStrings();
		selectNode(nodesIds, startNode);

		if (ImGui::Button("Execute")) {
			auto result = ga::dfs(graph, std::stoi(nodesIds[startNode]));
			for (const auto node : result) {
				std::cout << node << " ";
			}
			std::cout << '\n';
		}
	}
	else if (selectedAlgorithm == SelectedAlgorithm::BFS) {
		const auto nodesIds = graph.getNodesIdsStrings();
		selectNode(nodesIds, startNode);

		if (ImGui::Button("Execute")) {
			auto result = ga::bfs(graph, std::stoi(nodesIds[startNode]));
			for (const auto node : result) {
				std::cout << node << " ";
			}
			std::cout << '\n';
		}
	}
}

void AlgorithmsPanel::selectNode(const std::vector<std::string>& nodesIds, int& outNodeIdx)
{
	const char* comboLabel = nodesIds.empty() ? "" : nodesIds[outNodeIdx].c_str();

	if (ImGui::BeginCombo("##SelectStartNodeCombo", comboLabel)) {
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
