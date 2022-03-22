#include "../3rdParty/doctest.h"
#include "../Include/GraphNodeShape.hpp"

TEST_CASE("Graph Node Shape") {
	sf::Vector2f position{ 10.f, 10.f };
	GraphNodeShape graphNodeShape{ 0, position };

	SUBCASE("Shape position") {
		CHECK(graphNodeShape.getShape().getPosition() == position);
		position = { 20.f, 20.f };
		graphNodeShape.setPosition(position);
		CHECK(graphNodeShape.getShape().getPosition() == position);
	}

	SUBCASE("Shape color") {
		const sf::Color color{ 150, 100, 50 };
		CHECK(graphNodeShape.getShape().getFillColor() == sf::Color{ 255, 255, 255 });
		graphNodeShape.makeColored(color);
		CHECK(graphNodeShape.getShape().getFillColor() == color);
		graphNodeShape.resetColor();
		CHECK(graphNodeShape.getShape().getFillColor() == sf::Color{ 255, 255, 255 });
	}
}