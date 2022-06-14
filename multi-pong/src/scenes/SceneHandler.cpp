#include "pch.h"
#include "SceneHandler.h"

void pong::SceneHandler::AddScene(const std::string& sceneName, std::unique_ptr<IScene> scene)
{
	auto it = m_Scenes.find(sceneName);

	if (it != m_Scenes.end())
		std::cerr << "The scene " << sceneName << " is being replaced.\n";

	m_Scenes[sceneName] = std::move(scene);
}

void pong::SceneHandler::SetScene(const std::string& sceneName)
{
	auto it = m_Scenes.find(sceneName);
	if (it != m_Scenes.end())
	{
		m_CurrentScene = m_Scenes[sceneName].get();
		m_CurrentSceneName = sceneName;
		m_CurrentScene->OnSceneEnter();
	}
}

void pong::SceneHandler::Render(sf::RenderTarget& target)
{
	if (m_CurrentScene)
		m_CurrentScene->Render(target);
}

void pong::SceneHandler::Update(float dt)
{
	if (m_CurrentScene)
		m_CurrentScene->Update(dt);
}

void pong::SceneHandler::HandleInput(sf::Event event)
{
	if (m_CurrentScene)
		m_CurrentScene->HandleInput(event);
}
