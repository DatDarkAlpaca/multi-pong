#pragma once
#include "pch.h"
#include "IScene.h"

namespace pong
{
	class SceneHandler
	{
	public:
		void AddScene(const std::string& sceneName, std::unique_ptr<IScene> scene);

		void SetScene(const std::string& sceneName);

	public:
		std::string CurrentSceneName() const { return m_CurrentSceneName; }

	public:
		void Render(sf::RenderTarget& target);

		void Update(float dt);

		void HandleInput(sf::Event event);

	private:
		std::unordered_map<std::string, std::unique_ptr<IScene>> m_Scenes;

		std::string m_CurrentSceneName = "";
		IScene* m_CurrentScene = nullptr;
	};
}