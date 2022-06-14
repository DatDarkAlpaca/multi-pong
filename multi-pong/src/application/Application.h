#pragma once
#include <SFML/Graphics.hpp>
#include "resources/ResourceManager.h"
#include "scenes/SceneHandler.h"

namespace pong
{
	class Application
	{
	public:
		Application();

	public:
		void Run();

	public:
		void Render();

		void Update(float dt);

		void HandleInput();

	private:
		void Initialize();

	private:
		std::unique_ptr<sf::RenderWindow> m_Window;
		SceneHandler m_SceneHandler;

	private:
		std::unique_ptr<ResourceManager<sf::Font>> m_FontManager;
	};
}


