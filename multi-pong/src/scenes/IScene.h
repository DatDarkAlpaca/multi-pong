#pragma once
#include <SFML/Graphics.hpp>

namespace pong
{
	struct IScene
	{
	public:
		virtual ~IScene() = default;

	public:
		virtual void OnSceneEnter() { }

		virtual void Render(sf::RenderTarget& target) = 0;

		virtual void Update(float) = 0;

		virtual void HandleInput(sf::Event event) = 0;
	};
}