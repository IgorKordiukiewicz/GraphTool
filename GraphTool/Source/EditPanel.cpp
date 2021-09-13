#include "..\Include\EditPanel.hpp"
#include <imgui.h>

EditPanel::EditPanel(Graph& graph)
	: graph(graph)
{
}

void EditPanel::run()
{
	const char* directedName = graph.isDirected() ? "Directed" : "Undirected";
	int directedInt = graph.isDirected() ? 1 : 0;
	ImGui::SliderInt("##GraphDirectedSlider", &directedInt, 0, 1, directedName);
	if (directedInt == 1) {
		graph.makeDirected();
	}
	else {
		graph.makeUndirected();
	}

	const char* weightedName = graph.isWeighted() ? "Weighted" : "Unweighted";
	int weightedInt = graph.isWeighted() ? 1 : 0;
	ImGui::SliderInt("##GraphWeightedSlider", &weightedInt, 0, 1, weightedName);
	if (weightedInt == 1) {
		graph.makeWeighted();
	}
	else {
		graph.makeUnweighted();
	}

	ImGui::Separator();
	ImGui::Text("Double LMB to add new node");
	ImGui::Text("LMB on node to attach edge to it");
	ImGui::Text("RMB on node to delete it");
	ImGui::Text("RMB on edge to delete it");
	ImGui::Text("LMB on edge weight to edit it \n\t(Any MB or Enter to stop editing it)");
}
