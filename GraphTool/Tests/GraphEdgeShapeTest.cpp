#include "../3rdParty/doctest.h"
#include "../Include/GraphEdgeShape.hpp"
#include <imgui.h>
#include <imgui-SFML.h>

TEST_CASE("Graph Edge Shape") {
	sf::RenderWindow window;
	ImGui::SFML::Init(window);
	sf::Vector2f startPos{ 10.f, 10.f };
	sf::Vector2f endPos{ 20.f, 20.f };
	const int startNodeId{ 1 };
	const int endNodeId{ 2 };
	GraphEdgeShape graphEdgeShape{ startPos, endPos, startNodeId, endNodeId, Directed::No, Weighted::No };

	SUBCASE("Weighted text") {
		CHECK(graphEdgeShape.getWeightText().getString() == "0");
		graphEdgeShape.makeWeighted();
		graphEdgeShape.setWeight(8);
		CHECK(graphEdgeShape.getWeightText().getString() == "8");
	}
}