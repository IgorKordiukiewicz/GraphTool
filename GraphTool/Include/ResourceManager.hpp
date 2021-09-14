#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

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
	const sf::Texture& getEditorBackgroundTexture() const { return editorBackgroundTexture; }

private:
	ResourceManager();
	~ResourceManager() = default;

	ImFont* imGuiFont;
	sf::Font sfmlFont;
	sf::Texture editorBackgroundTexture;
};