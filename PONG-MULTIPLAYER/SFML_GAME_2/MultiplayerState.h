#pragma once
#include <SFML/Network.hpp>

#include "Game.h"
#include "GameState.h"
#include "GameServer.h"

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
	sf::UdpSocket udpSocket;
	sf::UdpSocket ballSocket;
	void connectToServer();

	float GetReflection(Paddle *player, float hity);
	void setPaddlePosition(Paddle *player, float y);

	void StartPause();
	void EndPause();

	void HandleUpdates(Game &game);
	void HandleCollisions(Game &Game);
	void HandleDraws(Game &game);
	void handlePacket(sf::Int32 packetType, sf::Packet& packet, sf::Time elapsedTime, sf::Vector2f prevPosition = sf::Vector2f());

	void executeBallThread();

	void interpolation(sf::Time& elapsedTime);
	float difference, threshold;
	int interpolation_constant;

	//SFML Stuff
	sf::Event event;
	sf::Clock clock;
	sf::Time elapsedTime;
	sf::Vector2i mousePos;
	Text enterToStart;
	Text escToExit;
	Text pause;
	Text playerName;
	Text opponentName;

	//integer
	int volume;
	int points = 0;
	int playerPos;
	unsigned short port;
	unsigned short sendingPort;
	unsigned short ballPort;

	//float
	float bgSpeed = 0.3f;
	float aiSpeed;

	//bool
	bool paused;
	bool update;
	bool connected;
	bool gameStarted;
	bool pAcknowledgmentSent;
	bool opponentAcknowledgementReceived;
	bool host;

	//string
	std::string playerId;

	Background bg;
	Paddle	*player;
	Paddle *opponent;
	sf::Vector2f opponentRemotePosition;

	Ball *ball;
	struct BallProperties {
		float speed;
		int ballHits;
		bool delayStart;
	} ballProperties;

	sf::Time clientTimeout;
	sf::Time timeSinceLastPacket;

	sf::Thread ballThread;

	std::unique_ptr<GameServer> gameServer;
};

