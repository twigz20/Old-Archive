#include "GameServer.h"
#include "Foreach.hpp"
#include "NetworkProtocol.h"

#include <SFML/Network/Packet.hpp>
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600


GameServer::GameServer()
	: mThread(&GameServer::executionThread, this)
	, mListeningState(false)
	, mClientTimeoutTime(sf::seconds(3.f))
	, mMaxConnectedPlayers(10)
	, mConnectedPlayers(0)
	, mPaddleCount(0)
	, mPeers(1)
	, mPaddleIdentifierCounter(1)
	, mWaitingThreadEnd(false)
	, mLastSpawnTime(sf::Time::Zero)
	, mTimeForNextSpawn(sf::seconds(5.f))
	, newConnect(false)
{
	ball = new Ball();
	ball->LoadFromFile("ball.png");
	ball->SetPosition(SCREEN_WIDTH / 2 - (ball->GetHeight() / 2), SCREEN_HEIGHT / 2 - (ball->GetHeight() / 2));

	mListenerSocket.setBlocking(false);
	mPeers[0].reset(new RemotePeer());
	mThread.launch();
}

GameServer::RemotePeer::RemotePeer()
	: ready(false)
	, timedOut(false)
{
	socket.setBlocking(false);
}

void GameServer::notifyPlayerRealtimeChange(sf::Int32 paddleIdentifier, sf::Int32 action, bool actionEnabled)
{
	for (std::size_t i = 0; i < mConnectedPlayers; ++i)
	{
		if (mPeers[i]->ready)
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::PlayerRealtimeChange);
			packet << paddleIdentifier;
			packet << action;
			packet << actionEnabled;

			mPeers[i]->socket.send(packet);
		}
	}
}

void GameServer::notifyPlayerEvent(sf::Int32 aircraftIdentifier, sf::Int32 action)
{
	for (std::size_t i = 0; i < mConnectedPlayers; ++i)
	{
		if (mPeers[i]->ready)
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::PlayerEvent);
			packet << aircraftIdentifier;
			packet << action;

			mPeers[i]->socket.send(packet);
		}
	}
}

void GameServer::notifyPlayerSpawn(sf::Int32 paddleIdentifier)
{
	for (std::size_t i = 0; i < mConnectedPlayers; ++i)
	{
		if (mPeers[i]->ready)
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::PlayerConnect);
			packet << paddleIdentifier << mPaddleInfo[paddleIdentifier].position.x << mPaddleInfo[paddleIdentifier].position.y;
			mPeers[i]->socket.send(packet);
		}
	}
}

GameServer::~GameServer()
{
	mWaitingThreadEnd = true;
	mThread.wait();
}

void GameServer::setListening(bool enable)
{
	// Check if it isn't already listening
	if (enable)
	{
		if (!mListeningState)
			mListeningState = (mListenerSocket.listen(9094) == sf::TcpListener::Done);
	}
	else
	{
		mListenerSocket.close();
		mListeningState = false;
	}
}

void GameServer::executionThread()
{
	setListening(true);

	sf::Time stepInterval = sf::seconds(1.f / 60.f);
	sf::Time stepTime = sf::Time::Zero;
	sf::Time tickInterval = sf::seconds(1.f / 20.f);
	sf::Time tickTime = sf::Time::Zero;
	sf::Clock stepClock, tickClock;

	while (!mWaitingThreadEnd)
	{
		handleIncomingPackets();
		handleIncomingConnections();

		stepTime += stepClock.getElapsedTime();
		stepClock.restart();

		tickTime += tickClock.getElapsedTime();
		tickClock.restart();

		// Fixed update step
		while (stepTime >= stepInterval)
		{
			ball->Update(stepInterval.asSeconds());
			//mBattleFieldRect.top += mBattleFieldScrollSpeed * stepInterval.asSeconds();
			//stepTime -= stepInterval;
		}

		// Fixed tick step
		while (tickTime >= tickInterval)
		{
			tick();
			tickTime -= tickInterval;
		}

		// Sleep to prevent server from consuming 100% CPU
		sf::sleep(sf::milliseconds(100));
	}
}

void GameServer::tick()
{
	updateClientState();

	// Check for mission success = all planes with position.y < offset
	bool allAircraftsDone = true;
	
	FOREACH(auto pair, mPaddleInfo)
	{
		// As long as one player has not crossed the finish line yet, set variable to false
		if (pair.second.position.y > 0.f)
			allAircraftsDone = false;
	}

	if (allAircraftsDone)
	{
		sf::Packet pointScoredPacket;
		pointScoredPacket << static_cast<sf::Int32>(Server::PointScored);
		sendToAll(pointScoredPacket);
	}
}

sf::Time GameServer::now() const
{
	return mClock.getElapsedTime();
}

void GameServer::handleIncomingPackets()
{
	bool detectedTimeout = false;

	FOREACH(PeerPtr& peer, mPeers)
	{
		if (peer->ready)
		{
			sf::Packet packet;
			while (peer->socket.receive(packet) == sf::Socket::Done)
			{
				// Interpret packet and react to it
				handleIncomingPacket(packet, *peer, detectedTimeout);

				// Packet was indeed received, update the ping timer
				peer->lastPacketTime = now();
				packet.clear();
			}

			if (now() >= peer->lastPacketTime + mClientTimeoutTime)
			{
				peer->timedOut = true;
				detectedTimeout = true;
			}
		}
	}

	if (detectedTimeout)
		handleDisconnections();
}

void GameServer::handleIncomingPacket(sf::Packet& packet, RemotePeer& receivingPeer, bool& detectedTimeout)
{
	sf::Int32 packetType;
	packet >> packetType;

	switch (packetType)
	{
	case Client::Quit:
	{
		receivingPeer.timedOut = true;
		detectedTimeout = true;
	} break;

	case Client::PlayerEvent:
	{
		sf::Int32 paddleIdentifier;
		sf::Int32 action;
		packet >> paddleIdentifier >> action;

		notifyPlayerEvent(paddleIdentifier, action);
	} break;

	case Client::PlayerRealtimeChange:
	{
		sf::Int32 paddleIdentifier;
		sf::Int32 action;
		bool actionEnabled;
		packet >> paddleIdentifier >> action >> actionEnabled;
		mPaddleInfo[paddleIdentifier].realtimeActions[action] = actionEnabled;
		notifyPlayerRealtimeChange(paddleIdentifier, action, actionEnabled);
	} break;

	case Client::PositionUpdate:
	{
		/*sf::Int32 numAircrafts;
		packet >> numAircrafts;

		for (sf::Int32 i = 0; i < numAircrafts; ++i)
		{
			sf::Int32 aircraftIdentifier;
			sf::Int32 aircraftHitpoints;
			sf::Int32 missileAmmo;
			sf::Vector2f aircraftPosition;
			packet >> aircraftIdentifier >> aircraftPosition.x >> aircraftPosition.y >> aircraftHitpoints >> missileAmmo;
			mAircraftInfo[aircraftIdentifier].position = aircraftPosition;
			mAircraftInfo[aircraftIdentifier].hitpoints = aircraftHitpoints;
			mAircraftInfo[aircraftIdentifier].missileAmmo = missileAmmo;
		}*/
	} break;

	case Client::GameEvent:
	{
		/*sf::Int32 action;
		float x;
		float y;

		packet >> action;
		packet >> x;
		packet >> y;

		// Enemy explodes: With certain probability, drop pickup
		// To avoid multiple messages spawning multiple pickups, only listen to first peer (host)
		if (action == GameActions::EnemyExplode && randomInt(3) == 0 && &receivingPeer == mPeers[0].get())
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::SpawnPickup);
			packet << static_cast<sf::Int32>(randomInt(Pickup::TypeCount));
			packet << x;
			packet << y;

			sendToAll(packet);
		}*/
	}
	}
}

void GameServer::updateClientState()
{
	sf::Packet updateClientStatePacket;
	updateClientStatePacket << static_cast<sf::Int32>(Server::UpdateClientState);
	updateClientStatePacket << static_cast<float>(ball->GetPositionX()) << static_cast<float>(ball->GetPositionX());
	updateClientStatePacket << static_cast<sf::Int32>(mPaddleInfo.size());

	FOREACH(auto paddle, mPaddleInfo)
		updateClientStatePacket << paddle.first << paddle.second.position.x << paddle.second.position.y;

	sendToAll(updateClientStatePacket);
}

void GameServer::handleIncomingConnections()
{
	if (!mListeningState)
		return;

	if (mListenerSocket.accept(mPeers[mConnectedPlayers]->socket) == sf::TcpListener::Done)
	{
		newConnect = true;
		if (mConnectedPlayers == 1) {
			// order the new client to spawn its own plane ( player 1 )
			mPaddleInfo[mPaddleIdentifierCounter].position = sf::Vector2f(SCREEN_WIDTH*.02, SCREEN_HEIGHT / 2);
		}
		else {
			// order the new client to spawn its own plane ( player 1 )
			mPaddleInfo[mPaddleIdentifierCounter].position = sf::Vector2f(SCREEN_WIDTH*.95, SCREEN_HEIGHT / 2);
		}
		sf::Packet packet;
		packet << static_cast<sf::Int32>(Server::SpawnSelf);
		packet << mPaddleIdentifierCounter;
		packet << mPaddleInfo[mPaddleIdentifierCounter].position.x;
		packet << mPaddleInfo[mPaddleIdentifierCounter].position.y;

		mPeers[mConnectedPlayers]->paddleIdentifiers.push_back(mPaddleIdentifierCounter);

		broadcastMessage("New player!");
		informWorldState(mPeers[mConnectedPlayers]->socket);
		notifyPlayerSpawn(mPaddleIdentifierCounter++);

		mPeers[mConnectedPlayers]->socket.send(packet);
		mPeers[mConnectedPlayers]->ready = true;
		mPeers[mConnectedPlayers]->lastPacketTime = now(); // prevent initial timeouts
		mPaddleCount++;
		mConnectedPlayers++;

		if (mConnectedPlayers >= mMaxConnectedPlayers)
			setListening(false);
		else // Add a new waiting peer
			mPeers.push_back(PeerPtr(new RemotePeer()));
	}
}

void GameServer::handleDisconnections()
{
	for (auto itr = mPeers.begin(); itr != mPeers.end(); )
	{
		if ((*itr)->timedOut)
		{
			// Inform everyone of the disconnection, erase 
			FOREACH(sf::Int32 identifier, (*itr)->paddleIdentifiers)
			{
				sendToAll(sf::Packet() << static_cast<sf::Int32>(Server::PlayerDisconnect) << identifier);

				mPaddleInfo.erase(identifier);
			}

			mConnectedPlayers--;
			mPaddleCount -= (*itr)->paddleIdentifiers.size();

			itr = mPeers.erase(itr);

			// Go back to a listening state if needed
			if (mConnectedPlayers < mMaxConnectedPlayers)
			{
				mPeers.push_back(PeerPtr(new RemotePeer()));
				setListening(true);
			}

			broadcastMessage("Opponent has disconnected.");
		}
		else
		{
			++itr;
		}
	}
}

// Tell the newly connected peer about how the world is currently
void GameServer::informWorldState(sf::TcpSocket& socket)
{
	sf::Packet packet;
	packet << static_cast<sf::Int32>(Server::InitialState);
	packet << ball->GetPositionX() << ball->GetPositionY();
	packet << static_cast<sf::Int32>(mPaddleCount);

	for (std::size_t i = 0; i < mConnectedPlayers; ++i)
	{
		if (mPeers[i]->ready)
		{
			FOREACH(sf::Int32 identifier, mPeers[i]->paddleIdentifiers)
				packet << identifier << mPaddleInfo[identifier].position.x << mPaddleInfo[identifier].position.y;
		}
	}

	socket.send(packet);
}

void GameServer::broadcastMessage(const std::string& message)
{
	for (std::size_t i = 0; i < mConnectedPlayers; ++i)
	{
		if (mPeers[i]->ready)
		{
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Server::BroadcastMessage);
			packet << message;

			mPeers[i]->socket.send(packet);
		}
	}
}

void GameServer::sendToAll(sf::Packet& packet)
{
	FOREACH(PeerPtr& peer, mPeers)
	{
		if (peer->ready)
			peer->socket.send(packet);
	}
}