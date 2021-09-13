#pragma once

#include <SFML/Graphics/Color.hpp>
#include <vector>

namespace Constants
{
	static const sf::Color mainColor{ 220, 60, 60 };
	static const float nodeRadius{ 16.f };
	// fraction of the whole application window
	static const float imGuiWindowWidth{ 0.25f };
	static const float traversalAnimationTime{ 1.0f }; // in seconds
}

namespace Colors
{
	// First color is always the Constants::mainColor;
	std::vector<sf::Color> generateColors(int count);
}
