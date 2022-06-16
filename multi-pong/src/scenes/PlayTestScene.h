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
			speed = 250.f;

			// Todo: set a random generator up.
			float angle = 0;
			float to_rad = -(float)std::numbers::pi / 180.f;

			velocity.x = std::cosf(angle * to_rad);
			velocity.y = std::sinf(angle * to_rad);
		}

	public:
		sf::Vector2f velocity;
		float speed;
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

		return GameOverState::None;
	}

	static void normalize(sf::Vector2f& vector)
	{
		auto m = std::sqrtf(vector.x * vector.x + vector.y * vector.y);
		vector.x /= m;
		vector.y /= m;
	}

	bool BallPaddleCollision(Paddle& paddle, Ball& ball)
	{
		sf::Vector2f paddleMiddle = {
			paddle.getGlobalBounds().left + paddle.getGlobalBounds().width / 2.f,
			paddle.getGlobalBounds().top + paddle.getGlobalBounds().height / 2.f
		};

		sf::Vector2f ballMiddle = {
			ball.getGlobalBounds().left + ball.getGlobalBounds().width / 2.f,
			ball.getGlobalBounds().top + ball.getGlobalBounds().height / 2.f
		};

		if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds()))
		{
			auto dir = paddleMiddle - ballMiddle;
			normalize(dir);

			ball.velocity = -dir;
			return true;
		}

		return false;
	}

	// Network:
	enum class ObjectType : sf::Uint8 { Ball = 0, Left, Right };

	/*
	*	Main Game:
	*/
	class PlayTestScene : public IScene
	{
	public:
		PlayTestScene(sf::RenderWindow& window, SceneHandler& sceneHandler, sf::Font* font, bool isHost = true)
			: refWindow(window), sceneHandler(sceneHandler), m_Font(font), m_IsHost(isHost)
		{
			m_HitBuffer.loadFromFile("./res/sounds/hit.wav");
			m_HitSound.setBuffer(m_HitBuffer);

			m_ScoredBuffer.loadFromFile("./res/sounds/scored.wav");
			m_ScoredSound.setBuffer(m_ScoredBuffer);
		}

	public:
		void OnSceneEnter() override
		{
			std::cout << "Scene loaded.\n";
			m_Socket.setBlocking(true);

			if (m_IsHost)
			{
				sf::TcpListener listener;
				std::cout << "Listening for incoming connections.\n";
				listener.listen(54000);
				listener.accept(m_Socket);

				LoadSceneAssets();
			}
			else
			{
				m_Socket.connect("26.32.89.95", 54000);
				std::cout << "Connected\n";
				LoadSceneAssets();
			}
		}

		void Render(sf::RenderTarget& target) override
		{
			if (m_Socket.isBlocking())
				return;

			target.draw(*ball);
			target.draw(*left);
			target.draw(*right);

			target.draw(*m_LeftScoreText);
			target.draw(*m_RightScoreText);

			target.draw(*m_GoBackText);
		}

		void Update(float dt) override
		{
			if (m_Socket.isBlocking())
				return;

			if (!m_IsHost && m_Paused)
			{
				sf::Packet packet;
				m_Socket.receive(packet);
				sf::String data;
				if (packet >> data && data == "play")
					m_Paused = false;
			}

			if (m_Paused)
				return;

			// Temporary solutions.
			if (BallPaddleCollision(*left, *ball))
				m_HitSound.play();
			if (BallPaddleCollision(*right, *ball))
				m_HitSound.play();

			// Updates:
			auto result = WorldBallCollision(480.f, 0, 600.f, *ball);

			switch (result)
			{
			case GameOverState::None:
				break;
			case GameOverState::LeftWins:
				m_ScoreLeft += 1;
				m_ScoredSound.play();
				Reset();
				UpdateScoreText();
				break;
			case GameOverState::RightWins:
				m_ScoreRight += 1;
				m_ScoredSound.play();
				Reset();
				UpdateScoreText();
				break;
			}

			// Networking:
			if(m_IsHost)
			{
				sf::Packet ballPacket;
				ball->Update(dt);
				ballPacket << (sf::Uint8)ObjectType::Ball <<  ball->getPosition().x << ball->getPosition().y;
				m_Socket.send(ballPacket);
			}
			else
			{
				sf::Vector2f ballPos;
				sf::Uint8 type;
				sf::Packet packet;
				m_Socket.receive(packet);
				if (packet >> type >> ballPos.x >> ballPos.y)
				{
					if(type == (sf::Uint8)ObjectType::Ball)
						ball->setPosition(ballPos);
				}
			}

			// Left:
			if (m_IsHost)
			{
				sf::Packet leftPacket;
				left->Update(dt);
				leftPacket << (sf::Uint8)ObjectType::Left << left->getPosition().x << left->getPosition().y;
				m_Socket.send(leftPacket);
			}
			else
			{
				sf::Vector2f leftPos;
				sf::Uint8 type;
				sf::Packet packet;
				m_Socket.receive(packet);
				if (packet >> type >> leftPos.x >> leftPos.y)
				{
					if (type == (sf::Uint8)ObjectType::Left)
						left->setPosition(leftPos);
				}
			}

			// Right:
			if (m_IsHost)
			{
				sf::Uint8 type;
				sf::Vector2f rightPos;
				sf::Packet packet;
				m_Socket.receive(packet);
				if (packet >> type >> rightPos.x >> rightPos.y)
				{
					if(type == (sf::Uint8)ObjectType::Right)
						right->setPosition(rightPos);
				}
			}
			else
			{
				sf::Packet rightPacket;
				right->Update(dt);
				rightPacket << (sf::Uint8)ObjectType::Right << right->getPosition().x << right->getPosition().y;
				m_Socket.send(rightPacket);
			}
		}

		void HandleInput(sf::Event event) override
		{
			if (m_Socket.isBlocking())
				return;

			if (event.type == sf::Event::KeyPressed)
			{
				if (m_IsHost)
				{
					if (event.key.code == sf::Keyboard::Key::Space)
					{
						m_Paused = false;
						sf::Packet packet;
						packet << sf::String("play");
						m_Socket.send(packet);
					}
				}

				if (event.key.code == sf::Keyboard::Key::BackSpace)
				{
					sceneHandler.SetScene("main_menu");
					return;
				}
			}

			if(m_IsHost)
				left->HandleInput(event);
			else
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

		void LoadSceneAssets()
		{
			m_Socket.setBlocking(false);

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

	private:
		sf::RenderWindow& refWindow;
		SceneHandler& sceneHandler;
		sf::Font* m_Font;

	// Testing:
	private:
		sf::SoundBuffer m_HitBuffer, m_ScoredBuffer;
		sf::Sound m_HitSound, m_ScoredSound;

	private:
		unsigned int m_ScoreLeft = 0, m_ScoreRight = 0;
		bool m_Paused = true;

	private:
		sf::Text *m_LeftScoreText = nullptr, *m_RightScoreText = nullptr, *m_GoBackText = nullptr;
		Paddle *left = nullptr, *right = nullptr;
		Ball* ball = nullptr;

	private:
		sf::TcpSocket m_Client;
		sf::TcpSocket m_Socket;
		bool m_IsHost;
	};
}