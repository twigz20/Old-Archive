#include "GameServer.h"
#include <iostream>

#include <SFML/Network/Packet.hpp>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

using namespace std;

GameServer::RemotePeer::RemotePeer()
	: ready(false)
	, timedOut(false)
{
	playerSocket.setBlocking(false);
}

GameServer::GameServer()
	: listeningState(false)
	, clientTimeoutTime(sf::seconds(3.f))
	, maxConnectedPlayers(10)
	, connectedPlayers(0)
	, paddleCount(0)
	, peers(1)
	, paddleIdentifierCounter(1)
	, lastSpawnTime(sf::Time::Zero)
	, p1Acknowledgement(false)
	, p2Acknowledgement(false)
	, ballPort(9999)
	, player1(new Paddle())
	, player2(new Paddle())
	, serverHas2Players(false)
	, serverThread(&GameServer::executionThread, this)
{
	peers[0].reset(new RemotePeer());
	serverThread.launch();
}

GameServer::~GameServer()
{
}

void GameServer::setListening(bool enable)
{
	// Check if it isn't already listening
	if (enable)
	{
		if (!listeningState) {
			connectionSocket.bind(9094);
			connectionSocket.setBlocking(false);
		}
	}
	else
	{
		connectionSocket.unbind();
		listeningState = false;
	}
}

void GameServer::handleIncomingConnections()
{
	sf::Packet packet;
	sf::IpAddress ip;
	unsigned short port;

	if (connectionSocket.receive(packet, ip, port) == sf::Socket::Done)
	{
		// Interpret packet and react to it
		sf::Int32 packetType;
		packet >> packetType;

		if (packetType == Client::NewConnection)
		{
			computerID[port] = ip;
			PeerPtr p(new RemotePeer);
			p->sendingPort = port;
			p->ip = ip;
			packet >> p->id;
			peers.push_back(std::move(p));

			std::cout << "Player Connected to Server" << endl;

			if (connectedPlayers == 0) {
				std::cout << "Setting Player 1 Paddle Position" << endl;
				// order the new client to spawn its own plane ( player 1 )
				paddleInfo[paddleIdentifierCounter].position = sf::Vector2f(SCREEN_WIDTH*.02, SCREEN_HEIGHT / 2);
				peers[connectedPlayers]->recievingPort = ports[0];
				peers[connectedPlayers]->playerSocket.bind(peers[connectedPlayers]->recievingPort);
			}
			else {
				std::cout << "Setting Player 2 Paddle Position" << endl;
				// order the new client to spawn its own plane ( player 2 )
				paddleInfo[paddleIdentifierCounter].position = sf::Vector2f(SCREEN_WIDTH*.95, SCREEN_HEIGHT / 2);
				peers[connectedPlayers]->recievingPort = ports[1];
				peers[connectedPlayers]->playerSocket.bind(peers[connectedPlayers]->recievingPort);
			}

			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::SpawnSelf);
			packet << ports[connectedPlayers];
			packet << paddleIdentifierCounter;
			packet << paddleInfo[paddleIdentifierCounter].position.x;
			packet << paddleInfo[paddleIdentifierCounter].position.y;


			peers[connectedPlayers]->paddleIdentifiers.push_back(paddleIdentifierCounter);

			if (connectedPlayers == 1) {
				notifyPlayerSpawn(paddleIdentifierCounter);
			}

			if (peers[connectedPlayers]->playerSocket.send(packet, ip, port) != sf::Socket::Done) {
				cout << "Sent Packet Spawning Packet" << endl;
			}

			paddleIdentifierCounter++;
			peers[connectedPlayers]->ready = true;
			peers[connectedPlayers]->lastPacketTime = now(); // prevent initial timeouts
			paddleCount++;
			connectedPlayers++;
		}

		packet.clear();
	}
}

void GameServer::executionThread()
{
	ports[0] = 9095;
	ports[1] = 9096;

	ball = new Ball();
	ball->LoadFromFile("ball.png");
	ball->SetPosition(SCREEN_WIDTH / 2 - (ball->GetHeight() / 2), SCREEN_HEIGHT / 2 - (ball->GetHeight() / 2));

	setListening(true);
	while (connectedPlayers < 1)
	{
		handleIncomingConnections();
	}

	sf::Thread player1Thread([=] { handleIncomingPacket(peers[0]); });
	player1Thread.launch();

	//sf::Thread player2Thread(&GameServer::handleIncomingPacket, peers[1]);
	//player2Thread.launch();

	while (true)
	{
		if (p1Acknowledgement && p2Acknowledgement) {
			HandleCollisions();
			sf::Time elapsedTime = serverClock.getElapsedTime();
			ball->Update(elapsedTime.asSeconds());
			serverClock.restart();

			sf::Packet ballMovement;
			ballMovement << Server::BallUpdate;
			ballMovement << ball->GetPositionX() << ball->GetPositionY();
			//connectionSocket.send(ballMovement, peers[0]->ip, ballPort);
			//connectionSocket.send(ballMovement, peers[1]->ip, ballPort);

			cout << ball->GetPositionX() << " " << ball->GetPositionY() << endl;
		}
	}

	setListening(false);
}

void GameServer::handleIncomingPacket(PeerPtr &peer)
{
	sf::Packet packet;
	sf::IpAddress ip;

	while (!serverHas2Players) {

		if (peer->playerSocket.receive(packet, ip, peer->recievingPort) == sf::Socket::Done)
		{
			// Interpret packet and react to it
			handleIncomingPackets(packet);
			packet.clear();
		}

	}
}

void GameServer::handleIncomingPackets(sf::Packet& packet)
{
	sf::Int32 packetType;
	packet >> packetType;

	switch (packetType)
	{
	case Client::PositionUpdate:
	{
		if (connectedPlayers >= 2) {
			sf::Int32 playerUpdate;
			packet >> playerUpdate;

			if (playerUpdate == PacketTags::Player1)
			{
				sf::Vector2f position;
				packet >> position.x >> position.y;
				 
				player1->SetPosition(position.x, position.y);

				std::cout << position.x << " " << position.y << std::endl;

				sf::Packet posUpdatePacket;
				posUpdatePacket << static_cast<sf::Int32>(Server::PlayerRealtimeChange);
				posUpdatePacket << position.x << position.y;

				if (peers[1]->playerSocket.send(posUpdatePacket, peers[1]->ip, peers[1]->sendingPort) != sf::Socket::Done)
					std::cout << "Failed to Send P1 Udp Packet" << std::endl;
			}
			else if (playerUpdate == PacketTags::Player2)
			{
				sf::Vector2f position;
				packet >> position.x >> position.y;

				player2->SetPosition(position.x, position.y);

				std::cout << position.x << " " << position.y << std::endl;

				sf::Packet posUpdatePacket;
				posUpdatePacket << static_cast<sf::Int32>(Server::PlayerRealtimeChange);
				posUpdatePacket << position.x << position.y;

				if (peers[0]->playerSocket.send(posUpdatePacket, peers[0]->ip, peers[0]->sendingPort) != sf::Socket::Done)
					std::cout << "Failed to Send P1 Udp Packet" << std::endl;
			}
		}
	} break;

	case Client::Acknowledgement:
	{
		int playerAcknowledgement;
		packet >> playerAcknowledgement;

		cout << "Acknowledgement Received" << endl;
		if (playerAcknowledgement == PacketTags::Player1)
			p1Acknowledgement = true;

		if (playerAcknowledgement == PacketTags::Player2)
			p2Acknowledgement = true;

		if (p1Acknowledgement && p2Acknowledgement) {
			sf::Packet acknowledgementPacket;
			acknowledgementPacket << Client::Acknowledgement;

			//sf::sleep(sf::seconds(3));
			peers[0]->playerSocket.send(acknowledgementPacket, peers[0]->ip, peers[0]->sendingPort);
			peers[1]->playerSocket.send(acknowledgementPacket, peers[1]->ip, peers[1]->sendingPort);
		}
	}
	}
}

sf::Time GameServer::now()
{
	return serverClock.getElapsedTime();
}

void GameServer::notifyPlayerSpawn(sf::Int32 paddleIdentifier)
{
	// Sending Player 2 Info to Player 1
	sf::Packet packet;
	packet << static_cast<sf::Int32>(Server::PlayerConnect);
	packet << paddleIdentifierCounter;
	packet << peers[paddleIdentifier]->id << paddleInfo[paddleIdentifier].position.x << paddleInfo[paddleIdentifier].position.y;
	peers[0]->playerSocket.send(packet, peers[0]->ip, peers[0]->sendingPort);

	// Sending Player 1 Info to Player 2
	packet.clear();
	packet << static_cast<sf::Int32>(Server::PlayerConnect);
	packet << 0;
	packet << peers[0]->id << paddleInfo[0].position.x << paddleInfo[0].position.y;
	peers[1]->playerSocket.send(packet, peers[1]->ip, peers[1]->sendingPort);
}

void GameServer::HandleCollisions()
{
	// Top and bottom
	if (ball->GetPositionY() < 0)
	{
		// Top
		// Keep the ball within the playing field and reflect the y-direction
		ball->SetPositionY(0);
		ball->SetVelocity(sf::Vector2f(ball->GetVelocity().x, -1 * ball->GetVelocity().y));
	}
	else if (ball->GetPositionY() + ball->GetHeight() >= SCREEN_HEIGHT)
	{
		// Bottom
		// Keep the ball within the playing field and reflect the y-direction
		ball->SetPositionY(SCREEN_HEIGHT - ball->GetHeight());
		ball->SetVelocity(sf::Vector2f(ball->GetVelocity().x, -1 * ball->GetVelocity().y));
	}

	//Make the ball bounce if it hits the left or right
	//Right
	if ((ball->GetPositionX() + ball->GetWidth() >= SCREEN_WIDTH))
	{
		//ball->changeBallSpeed(ballProperties.speed);

		// Set Ball to Center vertically and horizontally to the right Screen
		ball->SetPositionX(SCREEN_WIDTH - ball->GetWidth());
		ball->SetVelocity(sf::Vector2f(-1 * ball->GetVelocity().x, ball->GetVelocity().y));

		//ball->SetPosition(SCREEN_WIDTH / 2 - (ball->GetHeight() / 2), SCREEN_HEIGHT / 2 - (ball->GetHeight() / 2));
		//ball->setBallDirection(-1, -1);
	}
	//Left
	else if (ball->GetPositionX() <= 0)
	{
		//ball->changeBallSpeed(ballProperties.speed);

		// Set Ball to Center vertically and horizontally to the right Screen
		ball->SetPositionX(0);
		ball->SetVelocity(sf::Vector2f(-1 * ball->GetVelocity().x, ball->GetVelocity().y));

		//ball->setBallDirection(1, 1);
	}

	//Ball Paddle Collision p1
	if (ball->Collides(player1))
	{
		/*if (ballProperties.ballHits == 3)
		{
		ballProperties.ballHits = 0;
		ballProperties.speed += 30;
		ball->changeBallSpeed(ball->getBallSpeed()*ball->getSpeedMod());
		std::cout << ball->getBallSpeed() << std::endl;
		}*/

		ball->SetPositionX(player1->GetPositionX() + player1->GetWidth());
		ball->setBallDirection(1, GetReflection(player1, (ball->GetPositionY() + ball->GetHeight() / 2) - player1->GetPositionY()));

	}

	//Ball Paddle Collision p2
	if (ball->Collides(player2))
	{
		//ballProperties.ballHits++;
		//ball->changeBallSpeed(ball->getBallSpeed()*ball->getSpeedMod());
		//std::cout << ball->getBallSpeed() << std::endl;

		ball->SetPositionX(player2->GetPositionX() - ball->GetWidth());
		ball->setBallDirection(-1, GetReflection(player2, (ball->GetPositionY() + ball->GetHeight() / 2) - player2->GetPositionY()));
	}
}

float GameServer::GetReflection(Paddle *player, float hity) {
	// Make sure the hity variable is within the height of the paddle
	if (hity < 0)
	{
		hity = 0;
	}
	else if (hity > player->GetHeight())
	{
		hity = player->GetHeight();
	}

	// Everything above the center of the paddle is reflected upward
	// while everything below the center is reflected downward
	hity -= player->GetHeight() / 2.0f;

	// Scale the reflection, making it fall in the range -2.0f to 2.0f
	return 2.0f * (hity / (player->GetHeight() / 2.0f));
}