#include "Application.h"
#include "scenes/Scenes.h"

pong::Application::Application()
{
	sf::ContextSettings context;
	context.antialiasingLevel = 8;

	m_Window = std::make_unique<sf::RenderWindow>(sf::VideoMode(600, 480),
		"Multipong v1.0", sf::Style::Close | sf::Style::Titlebar, context);

	m_Window->setFramerateLimit(60);
}

void pong::Application::Run()
{
	Initialize();

	sf::Clock clock;
	while (m_Window->isOpen())
	{
		float dt = clock.restart().asSeconds();

		HandleInput();

		Update(dt);

		Render();
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
	m_FontManager = std::make_unique<ResourceManager<sf::Font>>();
	m_FontManager->Add("free_pixel", "./res/fonts/free_pixel.ttf");

	auto font = m_FontManager->Get("free_pixel");

	m_SceneHandler.AddScene("main_menu",
		std::make_unique<MainMenuScene>(*m_Window.get(), m_SceneHandler, font));

	m_SceneHandler.AddScene("play_test",
		std::make_unique<PlayTestScene>(*m_Window.get(), m_SceneHandler, font));

	m_SceneHandler.AddScene("host_menu",
		std::make_unique<PlayTestScene>(*m_Window.get(), m_SceneHandler, font, true));

	m_SceneHandler.AddScene("join_menu",
		std::make_unique<PlayTestScene>(*m_Window.get(), m_SceneHandler, font, false));

	m_SceneHandler.SetScene("main_menu");
}
