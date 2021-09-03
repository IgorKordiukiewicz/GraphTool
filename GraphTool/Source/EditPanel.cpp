#include "..\Include\EditPanel.hpp"
#include <imgui.h>

EditPanel::EditPanel(Graph& graph)
	: graph(graph)
{
}

void EditPanel::operator()()
{
	ImGui::Text("Type");
	ImGui::SameLine();

	const char* typeName = graph.isDirected() ? "Directed" : "Undirected";
	int typeInt = graph.isDirected() ? 0 : 1;
	ImGui::SliderInt("##GraphTypeSlider", &typeInt, 0, 1, typeName);
	if (typeInt == 0) {
		graph.makeDirected();
	}
	else {
		graph.makeUndirected();
	}

	ImGui::Separator();
	ImGui::Text("Double click to add new nodes");
}
