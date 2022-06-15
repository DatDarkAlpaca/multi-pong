#pragma once
#include "IScene.h"
#include "resources/ResourceManager.h"
#include "SceneHandler.h"
#include "gui/TextButton.h"

namespace pong
{
	// Control Helper:
	struct Controls
	{
		sf::Keyboard::Key moveUp = sf::Keyboard::Key::W,
						  moveDown = sf::Keyboard::Key::A;
	};

	/*
	*	Objects:
	*/
	class Paddle : public sf::RectangleShape
	{
	public:
		Paddle(const Controls& controls) : controls(controls) { }

	public:
		void Update(float dt)
		{
			move(0.f, m_VerticalMovement * m_Speed * dt);

			auto y = std::clamp(getPosition().y, 0.f, 480.f - getSize().y);
			setPosition(getPosition().x, y);
		}

		void HandleInput(sf::Event event)
		{
			if(sf::Keyboard::isKeyPressed(controls.moveUp))
				m_VerticalMovement = -1;

			else if (sf::Keyboard::isKeyPressed(controls.moveDown))
				m_VerticalMovement = 1;

			else if (event.type == sf::Event::KeyReleased)
				m_VerticalMovement = 0;
		}

	private:
		int m_VerticalMovement = 0;
		float m_Speed = 300.f;
		Controls controls;
	};

	class Ball : public sf::CircleShape
	{
	public:
		Ball()
		{
			SetRandomVelocity();
			setRadius(10.f);
			setFillColor(sf::Color::White);
		}

	public:
		void Update(float dt)
		{
			move(velocity * speed * dt);
			speed += dt * 10.f;
		}

	public:
		void SetRandomVelocity()
		{
			// Todo: set a random generator up.
			float angle = 30;
			float to_rad = -std::numbers::pi / 180.f;

			velocity.x = std::cosf(angle * to_rad);
			velocity.y = std::sinf(angle * to_rad);
		}

	public:
		sf::Vector2f velocity;
		float speed = 250.f;
	};

	enum class GameOverState { None = 0, LeftWins, RightWins };

	/*
	*	Collision:
	*/
	GameOverState WorldBallCollision(float worldHeight, float leftLose, float rightLose, Ball& ball)
	{
		if (ball.getPosition().y <= 0 || ball.getPosition().y >= worldHeight - ball.getRadius() * 2.f)
		{
			ball.velocity.y *= -1;
			return GameOverState::None;
		}

		else if (ball.getPosition().x <= leftLose)
			return GameOverState::RightWins;

		else if (ball.getPosition().x >= rightLose)
			return GameOverState::LeftWins;
	}

	void BallPaddleCollision(Paddle& paddle, Ball& ball)
	{
		float delta = 0.5f;

		auto ballBounds = ball.getGlobalBounds();
		sf::FloatRect bounds = {
			ballBounds.left + delta,
			ballBounds.top + delta,
			ballBounds.width + delta,
			ballBounds.height + delta
		};

		if (paddle.getGlobalBounds().intersects(bounds))
			ball.velocity.x *= -1.f;
	}

	/*
	*	Main Game:
	*/
	class PlayTestScene : public IScene
	{
	public:
		PlayTestScene(sf::RenderWindow& window, SceneHandler& sceneHandler, sf::Font* font)
			: refWindow(window), sceneHandler(sceneHandler), m_Font(font)
		{ }

	public:
		void OnSceneEnter() override
		{
			// Ball:
			ball = new Ball();
			ball->setPosition(600.f / 2.f - ball->getRadius() / 2.f,
				480.f / 2.f - ball->getRadius() / 2.f);

			// Left Paddle:
			left = new Paddle({ sf::Keyboard::Key::W, sf::Keyboard::Key::S });
			left->setSize({ 30.f, 150.f });
			left->setPosition(
				50.f - left->getSize().x / 2.f,
				480.f / 2.f - left->getSize().y / 2.f
			);

			right = new Paddle({ sf::Keyboard::Key::Up, sf::Keyboard::Key::Down });
			right->setSize({ 30.f, 150.f });
			right->setPosition(
				550.f - left->getSize().x / 2.f,
				480.f / 2.f - left->getSize().y / 2.f
			);

			// Score Texts:
			m_LeftScoreText = new sf::Text("", *m_Font, 24);
			m_LeftScoreText->setPosition(
				50.f - m_LeftScoreText->getGlobalBounds().width / 2.f, 30.f
			);

			m_RightScoreText = new sf::Text("", *m_Font, 24);
			m_RightScoreText->setPosition(
				550.f - m_LeftScoreText->getGlobalBounds().width / 2.f, 30.f
			);

			// Go Back text:
			m_GoBackText = new sf::Text("Press Backspace to return.", *m_Font, 16);
			m_GoBackText->setFillColor(sf::Color(200, 20, 20));
			m_GoBackText->setPosition(
				5.f, 480.f - m_GoBackText->getGlobalBounds().height * 2.f
			);

			UpdateScoreText();
		}

		void Render(sf::RenderTarget& target) override
		{
			target.draw(*ball);
			target.draw(*left);
			target.draw(*right);

			target.draw(*m_LeftScoreText);
			target.draw(*m_RightScoreText);

			target.draw(*m_GoBackText);
		}

		void Update(float dt) override
		{
			if (m_Paused)
				return;

			ball->Update(dt);
			left->Update(dt);
			right->Update(dt);

			BallPaddleCollision(*left, *ball);
			BallPaddleCollision(*right, *ball);

			auto result = WorldBallCollision(480.f, 0, 600.f, *ball);

			switch (result)
			{
			case GameOverState::None:
				break;
			case GameOverState::LeftWins:
				m_ScoreLeft += 1;
				Reset();
				UpdateScoreText();
				break;
			case GameOverState::RightWins:
				m_ScoreRight += 1;
				Reset();
				UpdateScoreText();
				break;
			}
		}

		void HandleInput(sf::Event event) override
		{
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Key::Space)
					m_Paused = false;

				if (event.key.code == sf::Keyboard::Key::BackSpace)
				{
					sceneHandler.SetScene("main_menu");
					return;
				}
			}

			left->HandleInput(event);
			right->HandleInput(event);
		}

		void OnSceneLeave() override
		{
			delete ball;
			delete left;
			delete right;

			delete m_LeftScoreText;
			delete m_RightScoreText,
			delete m_GoBackText;

			m_ScoreLeft = 0;
			m_ScoreRight = 0;

			Reset();
		}

	private:
		void Reset()
		{
			m_Paused = true;

			ball->SetRandomVelocity();
			ball->setPosition(600.f / 2.f - ball->getRadius() / 2.f,
				480.f / 2.f - ball->getRadius() / 2.f);

			left->setPosition(
				50.f - left->getSize().x / 2.f,
				480.f / 2.f - left->getSize().y / 2.f
			);
			right->setPosition(
				550.f - left->getSize().x / 2.f,
				480.f / 2.f - left->getSize().y / 2.f
			);
		}

		void UpdateScoreText()
		{
			m_LeftScoreText->setString(std::to_string(m_ScoreLeft));
			m_RightScoreText->setString(std::to_string(m_ScoreRight));
		}

	private:
		sf::RenderWindow& refWindow;
		SceneHandler& sceneHandler;
		sf::Font* m_Font;

	private:
		unsigned int m_ScoreLeft = 0, m_ScoreRight = 0;
		bool m_Paused = true;

	private:
		sf::Text *m_LeftScoreText = nullptr, *m_RightScoreText = nullptr, *m_GoBackText = nullptr;
		Paddle *left = nullptr, *right = nullptr;
		Ball* ball = nullptr;
	};
}