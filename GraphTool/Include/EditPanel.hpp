#pragma once
#include "Graph.hpp"

class EditPanel
{
public:
	EditPanel(Graph& graph);

	void run();

private:
	Graph& graph;
};