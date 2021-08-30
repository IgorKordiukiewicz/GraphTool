#include "../Include/Application.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>

Application::Application()
	: window(sf::VideoMode(1600, 900), "Graph Tool")
{
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(30);

	// Initialize ImGui
	ImGui::SFML::Init(window);
}

void Application::run()
{
	sf::Clock clock;

	while (window.isOpen()) {
		processEvents();

		ImGui::SFML::Update(window, clock.restart());

		// Fixed imgui window size & position
		ImGui::SetNextWindowSize(ImVec2{ static_cast<float>(window.getSize().x / 4), static_cast<float>(window.getSize().y) });
		ImGui::SetNextWindowPos({ 0,0 });

		// Setup main imgui window
		ImGui::Begin("Main Window", (bool*)0,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse);

		if (ImGui::BeginTabBar("MainTabBar")) {
			if (ImGui::BeginTabItem("Graph Edit")) {
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Algorithms")) {
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
		ImGui::End();

		window.clear(backgroundColor);

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

		if (event.type == sf::Event::Closed) {
			window.close();
		}
	}
}