#include "../Include/Application.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include "../Include/ResourceManager.hpp"
#include <iostream>

Application::Application()
	: graph(), editPanel(graph), algorithmsPanel(graph)
{
	// Initialize window
	sf::ContextSettings contextSettings;
	contextSettings.antialiasingLevel = 8;
	window.create(sf::VideoMode(1600, 900), "Graph Tool", sf::Style::Default, contextSettings);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(30);
	ImGui::SFML::Init(window);

	// Add custom font
	font = ResourceManager::instance().getImGuiFont();
	ImGui::SFML::UpdateFontTexture();

	graphEditor = std::make_unique<GraphEditor>(graph, window);
}

void Application::run()
{
	sf::Clock clock;

	while (window.isOpen()) {
		const float deltaTime = clock.restart().asSeconds();

		processEvents();

		ImGui::SFML::Update(window, sf::seconds(deltaTime));
		graphEditor->update(deltaTime);

		// Fixed imgui window size & position
		ImGui::SetNextWindowSize(ImVec2{ static_cast<float>(window.getSize().x / 4), static_cast<float>(window.getSize().y) });
		ImGui::SetNextWindowPos({ 0,0 });

		if (font) {
			ImGui::PushFont(font);
		}

		// Setup main imgui window
		ImGui::Begin("Main Window", (bool*)0,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse);

		if (ImGui::BeginTabBar("MainTabBar")) {
			// Edit panel
			if (ImGui::BeginTabItem("Edit")) {
				editPanel();
				ImGui::EndTabItem();
			}
			// Algorithms panel
			if (ImGui::BeginTabItem("Algorithms")) {
				algorithmsPanel();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		if (font) {
			ImGui::PopFont();
		}

		ImGui::End();

		window.clear(backgroundColor);

		graphEditor->draw(window);

		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();
}

void Application::processEvents()
{
	sf::Event event;
	while (window.pollEvent(event)) {
		ImGui::SFML::ProcessEvent(event);

		graphEditor->processEvents(event);

		if (event.type == sf::Event::Closed) {
			window.close();
		}
	}
}