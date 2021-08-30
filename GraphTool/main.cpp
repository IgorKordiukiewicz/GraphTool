#include "Include/Application.hpp"
#include "Include/Graph.hpp"

int main()
{
	Graph<Directed::Yes> graph;
	graph.addEdge(1, 2);
	graph.addEdge(2, 3);
	graph.addEdge(1, 3);
	graph.addNode(4);
	graph.print();
	
	Application app;
	app.run();

	return 0;
}