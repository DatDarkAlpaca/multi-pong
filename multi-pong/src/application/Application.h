#pragma once
#include <SFML/Graphics.hpp>


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
		std::unique_ptr<sf::RenderWindow> m_Window;
	};
}


