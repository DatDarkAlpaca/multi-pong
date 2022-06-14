#pragma once
#include "pch.h"

namespace pong
{
	class TextButton
	{
	public:
		TextButton() = default;
		TextButton(const std::function<void()>& function)
			: m_Function(function) {}

	public:
		void SetFunction(const std::function<void()>& function) { m_Function = function; }

	public:
		void Update(sf::RenderWindow& window)
		{
			auto pos = sf::Mouse::getPosition(window);
			sf::Vector2f mouse = { (float)pos.x, (float)pos.y };

			if (text.getGlobalBounds().contains(mouse))
				m_Hovered = true;
			else
				m_Hovered = false;
		}

		void HandleInput(sf::Event event)
		{
			HandleColor(event);

			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (m_Hovered && m_Function)
					m_Function();
			}
		}

	private:
		void HandleColor(sf::Event event)
		{
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (m_Hovered)
					text.setFillColor(sf::Color::Red);
			}

			else if (event.type == sf::Event::MouseButtonReleased)
			{
				if (m_Hovered)
					text.setFillColor(sf::Color(100, 100, 100));
				else
					text.setFillColor(sf::Color::White);
			}

			else
			{
				if (m_Hovered)
					text.setFillColor(sf::Color(100, 100, 100));
				else
					text.setFillColor(sf::Color::White);
			}
		}

	public:
		sf::Text text;

	private:
		std::function<void()> m_Function;
		bool m_Hovered = false;
	};
}