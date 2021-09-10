#pragma once

#include <SFML/Graphics/Font.hpp>

struct ImFont;

class ResourceManager
{
public:
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
	ResourceManager(ResourceManager&&) = delete;
	ResourceManager& operator=(ResourceManager&&) = delete;

	static ResourceManager& instance()
	{
		static ResourceManager instance;
		return instance;
	}

	ImFont* getImGuiFont() { return imGuiFont; }
	const sf::Font& getSFMLFont() const { return sfmlFont; }

private:
	ResourceManager();
	~ResourceManager() = default;

	ImFont* imGuiFont;
	sf::Font sfmlFont;
};