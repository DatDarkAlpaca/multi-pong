#pragma once
#include "IScene.h"
#include "resources/ResourceManager.h"
#include "SceneHandler.h"
#include "gui/TextButton.h"

namespace pong
{
	class MainMenuScene : public IScene
	{
	public:
		MainMenuScene(sf::RenderWindow& window, SceneHandler& sceneHandler, sf::Font* font)
			: refWindow(window), sceneHandler(sceneHandler), m_Font(font) { }

	public:
		void OnSceneEnter() override
		{
			m_Title = sf::Text("MultiPong", *m_Font, 48);
			m_Title.setPosition(
				600.f / 2.f - (m_Title.getLocalBounds().width / 2.f),
				100.f
			);

			m_Host.text = sf::Text("Host", *m_Font, 32);
			m_Host.text.setPosition(
				600.f / 2.f - (m_Host.text.getLocalBounds().width / 2.f),
				230.f
			);

			m_Join.text = sf::Text("Join", *m_Font, 32);
			m_Join.text.setPosition(
				600.f / 2.f - (m_Join.text.getLocalBounds().width / 2.f),
				230.f + 50.f
			);

			m_Exit.text = sf::Text("Exit", *m_Font, 32);
			m_Exit.SetFunction([&]() { refWindow.close(); });
			m_Exit.text.setPosition(
				600.f / 2.f - (m_Exit.text.getLocalBounds().width / 2.f),
				230.f + 50.f + 50.f
			);
		}

		void Render(sf::RenderTarget& target) override
		{
			target.draw(m_Title);
			target.draw(m_Host.text);
			target.draw(m_Join.text);
			target.draw(m_Exit.text);
		}

		void Update(float dt) override
		{
			m_Host.Update(refWindow);
			m_Join.Update(refWindow);
			m_Exit.Update(refWindow);
		}

		void HandleInput(sf::Event event) override
		{
			m_Host.HandleInput(event);
			m_Join.HandleInput(event);
			m_Exit.HandleInput(event);
		}

	private:
		sf::Text m_Title;
		TextButton m_Host, m_Join, m_Exit;

	private:
		sf::RenderWindow& refWindow;
		SceneHandler& sceneHandler;
		sf::Font* m_Font;
	};
}