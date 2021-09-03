#pragma once
#include "Graph.hpp"

class EditPanel
{
public:
	EditPanel(Graph& graph);

	void operator()();

private:
	Graph& graph;
};