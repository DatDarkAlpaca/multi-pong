#pragma once
#include <SFML/Graphics.hpp>

namespace pong
{
	struct IScene
	{
		virtual void Render() = 0;

		virtual void Update(float) = 0;

		virtual void HandleInput(sf::Event event) = 0;
	};
}