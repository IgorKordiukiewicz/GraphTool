#include "../Include/Application.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include "../Include/ResourceManager.hpp"
#include <iostream>

Application::Application()
	: graph()
{
	// Initialize window
	sf::ContextSettings contextSettings;
	contextSettings.antialiasingLevel = 8;
	window.create(sf::VideoMode(1600, 900), "Graph Tool", sf::Style::Default, contextSettings);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(30);
	ImGui::SFML::Init(window);

	initializeImGuiStyle();

	// Set window icon
	sf::Image icon;
	icon.loadFromFile("Resources/icon.png");
	window.setIcon(64, 64, icon.getPixelsPtr());

	// Add custom font
	font = ResourceManager::instance().getImGuiFont();
	ImGui::SFML::UpdateFontTexture();

	graphEditor = std::make_unique<GraphEditor>(graph, window);
	editPanel = std::make_unique<EditPanel>(graph);
	algorithmsPanel = std::make_unique<AlgorithmsPanel>(graph, *graphEditor);
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
		//ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(ImColor(220, 60, 60)));

		// Setup main imgui window
		ImGui::Begin("Main Window", (bool*)0,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse);

		if (ImGui::BeginTabBar("MainTabBar")) {
			// Edit panel
			if (ImGui::BeginTabItem("Edit")) {
				if (currentPanel != Panel::EditPanel) {
					graphEditor->setCurrentPanel(Panel::EditPanel);
					currentPanel = Panel::EditPanel;
				}

				editPanel->run();
				ImGui::EndTabItem();
			}
			// Algorithms panel
			if (ImGui::BeginTabItem("Algorithms")) {
				if (currentPanel != Panel::AlgorithmsPanel) {
					graphEditor->setCurrentPanel(Panel::AlgorithmsPanel);
					currentPanel = Panel::AlgorithmsPanel;
					algorithmsPanel->reset();
				}

				algorithmsPanel->run();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		if (font) {
			ImGui::PopFont();
		}
		//ImGui::PopStyleColor(1);

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

void Application::initializeImGuiStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();

	auto RGB = [](int r, int g, int b, float alpha) {
		return ImVec4(static_cast<float>(r) / 255.f, static_cast<float>(g) / 255.f, static_cast<float>(b) / 255.f, alpha);
	};
	
	// Colors
	style.Colors[ImGuiCol_WindowBg] =			RGB(20, 20, 25, 1.0f);
	style.Colors[ImGuiCol_FrameBg] =			RGB(50, 30, 30, 0.9f);
	style.Colors[ImGuiCol_FrameBgHovered] =		RGB(160, 70, 70, 0.8f);
	style.Colors[ImGuiCol_FrameBgActive] =		RGB(210, 90, 90, 0.9f);
	style.Colors[ImGuiCol_SliderGrab] =			RGB(220, 60, 60, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] =	RGB(220, 60, 60, 1.0f);
	style.Colors[ImGuiCol_Separator] =			RGB(220, 60, 60, 1.0f);
	style.Colors[ImGuiCol_Tab] =				RGB(132, 50, 50, 1.0f);
	style.Colors[ImGuiCol_TabHovered] =			RGB(229, 104, 104, 1.0f);
	style.Colors[ImGuiCol_TabActive] =			RGB(220, 60, 60, 1.0f);
	style.Colors[ImGuiCol_Button] =				RGB(220, 60, 60, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] =		RGB(229, 104, 104, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] =		RGB(217, 41, 41, 1.0f);
	style.Colors[ImGuiCol_CheckMark] =			RGB(220, 60, 60, 1.0f);
	style.Colors[ImGuiCol_Header] =				RGB(132, 50, 50, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] =		RGB(220, 60, 60, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] =		RGB(217, 41, 41, 1.0f);
	style.Colors[ImGuiCol_PopupBg] =			RGB(20, 20, 25, 0.95f);
	
	// Rounding
	style.FrameRounding =	4.f;
	style.GrabRounding =	4.f;
	style.TabRounding =		4.f;
}
