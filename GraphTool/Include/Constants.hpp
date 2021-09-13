#pragma once

#include <SFML/Graphics/Color.hpp>

namespace Constants
{
	static const sf::Color mainColor{ 220, 60, 60 };
	static const float nodeRadius{ 16.f };
	static const float nodeOutlineThickness{ 2.f };
	// fraction of the whole application window
	static const float imGuiWindowWidth{ 0.25f };
	static const float traversalAnimationTime{ 0.5f }; // in seconds
}
