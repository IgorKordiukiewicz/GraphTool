#include "../Include/ResourceManager.hpp"
#include <imgui.h>

ResourceManager::ResourceManager()
{
	// Load ImGui font
	ImGuiIO& io = ImGui::GetIO();
	imGuiFont = io.Fonts->AddFontFromFileTTF("Resources/consola.ttf", 14.f);

	// Load SFML font
	sfmlFont.loadFromFile("Resources/consola.ttf");
}