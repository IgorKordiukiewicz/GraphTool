#pragma once

#include "GraphEditor.hpp"

class EditPanel
{
public:
	EditPanel(Graph& graph, GraphEditor& graphEditor);

	void run();

private:
	void saveGraphToFile(const std::string& name);
	void loadGraphFromFile(const std::string& name);

private:
	Graph& graph;
	GraphEditor& graphEditor;

	char inputFileNameBuffer[128]{ "" };
	std::vector<std::string> filesNames;
	std::string selectedFileName;
};