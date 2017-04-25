#pragma once
#include <SFML/Network.hpp>

#include "Game.h"
#include "GameState.h"

#include "Paddle.h"
#include "Ball.h"


class MultiplayerState :
	public GameState
{
public:
	MultiplayerState();
	~MultiplayerState();

	void HandleEvents(Game &game);
	void Update(Game &game);
	void Render(Game &game);
	void Clear();

private:
	sf::IpAddress ip;
	sf::TcpSocket socket;
	void connectToServer();

	float GetReflection(int index, float hity);
	void setPaddlePosition(float index, float y);
	void aiMove(sf::Time& elapsedTime);

	void StartPause();
	void EndPause();

	void HandleUpdates(Game &game);
	void HandleCollisions(Game &Game);
	void HandleDraws(Game &game);

	//SFML Stuff
	sf::Event event;
	sf::Clock clock;
	sf::Time elapsedTime;
	sf::Vector2i mousePos;
	Text enterToStart;
	Text escToExit;
	Text pause;
	Text playerName;
	Text otherPlayerName;

	//integer
	int volume;
	int points = 0;
	int paddle;

	//float
	float bgSpeed = 0.3f;
	float aiSpeed;

	//bool
	bool paused;
	bool firstTouch;
	bool update;
	char connectionType;
	bool startOtherPlayer;

	Background bg;
	std::vector<Paddle *>	player;

	Ball *ball;
	struct BallProperties {
		float speed;
		int ballHits;
		bool delayStart;
	} ballProperties;
};

