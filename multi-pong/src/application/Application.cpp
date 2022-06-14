#include "Application.h"
#include "scenes/Scenes.h"
#include "resources/FontManager.h"

pong::Application::Application()
{
	m_Window = std::make_unique<sf::RenderWindow>(sf::VideoMode(600, 600), "Multipong v1.0");
}

void pong::Application::Run()
{
	Initialize();

	while (m_Window->isOpen())
	{
		Update(0.f);

		Render();

		HandleInput();
	}
}

void pong::Application::Render()
{
	m_Window->clear();

	m_SceneHandler.Render(*m_Window);

	m_Window->display();
}

void pong::Application::Update(float dt)
{
	m_SceneHandler.Update(dt);
}

void pong::Application::HandleInput()
{
	sf::Event event;
	while (m_Window->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			m_Window->close();

		m_SceneHandler.HandleInput(event);
	}
}

void pong::Application::Initialize()
{
	FontManager::GetInstance()->Add("pixel-font", "./res/fonts/free_pixel.ttf");

	m_SceneHandler.AddScene("main_menu", std::make_unique<MainMenuScene>());
	m_SceneHandler.SetScene("main_menu");
}
