#include "..\Include\EditPanel.hpp"
#include <imgui.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

EditPanel::EditPanel(Graph& graph, GraphEditor& graphEditor)
	: graph(graph), graphEditor(graphEditor)
{
	// Find all graphs files names
	auto filesPath = std::filesystem::current_path() / "GraphsFiles";
	if (std::filesystem::exists(filesPath) && std::filesystem::is_directory(filesPath)) {
		for (const auto& path : std::filesystem::recursive_directory_iterator(filesPath)) {
			if (std::filesystem::is_regular_file(path) && path.path().extension() == ".graph") {
				filesNames.push_back(path.path().stem().string());
			}
		}
	}
}

void EditPanel::run()
{
	// Take whole available width
	ImGui::PushItemWidth(-FLT_MIN);

	if (ImGui::Button("Load")) {
		loadGraphFromFile(selectedFileName);
		selectedFileName.clear();
	}
	ImGui::SameLine();

	const char* fileNameComboLabel = selectedFileName.empty() ? "" : selectedFileName.c_str();
	if (ImGui::BeginCombo("##FileNameCombo", fileNameComboLabel)) {
		for (const auto& fileName : filesNames) {
			bool isSelected = (fileName == selectedFileName);
			if (ImGui::Selectable(fileName.c_str(), isSelected)) {
				selectedFileName = fileName;
			}

			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}
	ImGui::Separator();

	if (ImGui::Button("Save")) {
		saveGraphToFile(inputFileNameBuffer);
	}
	ImGui::SameLine();
	ImGui::InputTextWithHint("##FileNameInputText", "Enter file name", inputFileNameBuffer, IM_ARRAYSIZE(inputFileNameBuffer));
	ImGui::Separator();
	
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

	ImGui::PopItemWidth();

	ImGui::Separator();
	ImGui::Text("Double click LMB to add new node");
	ImGui::Text("LMB on node to attach edge to it");
	ImGui::Text("RMB on node to delete it");
	ImGui::Text("RMB on edge to delete it");
	ImGui::Text("LMB on edge weight to edit it \n\t(Any MB or Enter to stop editing it)");
}

void EditPanel::saveGraphToFile(const std::string& name)
{
	if (name.empty()) {
		return;
	}

	// Create GraphsFiles directory if it doesn't exist
	auto filesPath = std::filesystem::current_path() / "GraphsFiles";
	if (!std::filesystem::is_directory(filesPath)) {
		std::filesystem::create_directory(filesPath);
	}

	std::ofstream file("GraphsFiles/" + name + ".graph");

	file << graph.isDirected() << '\n';
	file << graph.isWeighted() << '\n';
	for (const auto& nodeShape : graphEditor.getNodesShapes()) {
		file << nodeShape.getNodeId() << " " << static_cast<int>(nodeShape.getShape().getPosition().x) << " " << static_cast<int>(nodeShape.getShape().getPosition().y) << " ";
	}
	file << '\n';
	for (const auto& edge : graph.getDirectedEdges()) {
		file << edge.a << " " << edge.b << " " << edge.weight << " ";
	}
	file << '\n';
	for (const auto& edge : graph.getUndirectedEdges()) {
		file << edge.a << " " << edge.b << " " << edge.weight << " ";
	}

	file.close();
}

void EditPanel::loadGraphFromFile(const std::string& name)
{
	if (name.empty()) {
		return;
	}

	// Create GraphsFiles directory if it doesn't exist
	auto filesPath = std::filesystem::current_path() / "GraphsFiles";
	if (!std::filesystem::is_directory(filesPath)) {
		std::filesystem::create_directory(filesPath);
	}

	graph.reset();

	std::ifstream file("GraphsFiles/" + name + ".graph");

	std::string line;
	int lineIdx{ 0 };

	auto splitLine = [&line]() {
		std::vector<int> tokens;
		std::string token;
		std::istringstream iss(line);
		while (std::getline(iss, token, ' ')) {
			tokens.push_back(std::stoi(token));
		}
		return tokens;
	};

	while (std::getline(file, line)) {
		std::istringstream iss;

		if (lineIdx == 0) {
			bool isDirected{ false };
			iss >> isDirected;
			isDirected ? graph.makeDirected() : graph.makeUndirected();
		}
		else if (lineIdx == 1) {
			bool isWeighted;
			iss >> isWeighted;
			isWeighted ? graph.makeWeighted() : graph.makeUnweighted();
		}
		else if (lineIdx == 2) {
			// Create nodes
			const auto nodes = splitLine();
			graph.setNextNodeId(nodes.front());
			for (int i = 0; i < nodes.size(); i += 3) {
				graphEditor.createNewNode({ static_cast<float>(nodes[i + 1]), static_cast<float>(nodes[i + 2]) });
			}
		}
		else if (lineIdx == 3) {
			const auto directedEdges = splitLine();
			std::getline(file, line);
			const auto undirectedEdges = splitLine();

			for (int i = 0; i < directedEdges.size(); i += 3) {
				auto[directedEdgeShape, undirectedEdgeShape] = graphEditor.createNewEdge(directedEdges[i], directedEdges[i + 1]);
				// Update directed edge weight
				if (const int directedEdgeWeight = directedEdges[i + 2]; directedEdgeWeight != 0) {
					graph.setEdgeWeight(directedEdges[i], directedEdges[i + 1], directedEdgeWeight, Directed::Yes);
					directedEdgeShape->setWeight(directedEdgeWeight);
				}

				if (!undirectedEdgeShape) {
					continue;
				}

				// Update undirected edge weight
				for (int j = 0; j < undirectedEdges.size(); j += 3) {
					if (const int undirectedEdgeWeight = undirectedEdges[j + 2]; undirectedEdgeWeight != 0) {
						if ((undirectedEdgeShape->getStartNodeId() == undirectedEdges[j] && undirectedEdgeShape->getEndNodeId() == undirectedEdges[j + 1])
							|| (undirectedEdgeShape->getStartNodeId() == undirectedEdges[j + 1] && undirectedEdgeShape->getEndNodeId() == undirectedEdges[j])) {
							graph.setEdgeWeight(undirectedEdgeShape->getStartNodeId(), undirectedEdgeShape->getEndNodeId(), undirectedEdgeWeight, Directed::No);
							undirectedEdgeShape->setWeight(undirectedEdgeWeight);
						}
					}
				}
			}
		}

		++lineIdx;
	}
}
